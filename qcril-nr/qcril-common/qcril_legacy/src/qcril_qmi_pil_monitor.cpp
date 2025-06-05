/******************************************************************************
#  Copyright (c) 2012, 2017, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    qcril_qmi_pil_monitor.c

  DESCRIPTION
    monitor the state of ADSP from PIL ADSP state file and inform the changes
    to the interested clients

******************************************************************************/
#if !defined (QMI_RIL_UTF) && defined RIL_WEARABLES_TARGET_AON
#define __STDC_FORMAT_MACROS 1

//#include "qcrili.h"
//#include "qcril_other.h"
#include "qcril_dir_utils.h"
#include <cutils/uevent.h>
#include <linux/slatecom_interface.h>
#include <cutils/properties.h>

#ifdef FEATURE_TARGET_GLIBC_x86
   extern "C" size_t strlcat(char *, const char *, size_t);
   extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

#include <pthread.h>
#include <poll.h>
#include <fcntl.h>
#include <sstream>

#define TAG "PIL_Monitor"

#include <framework/Log.h>

#include "qcril_qmi_pil_monitor.h"
#include "qcril_qmi_singleton_agent.h"

#define QMI_RIL_IMS_PIL_MONITOR_THREAD_NAME             "pil_monitor"

class qcril_qmi_pil_monitor : public qcril_qmi_singleton_agent<qcril_qmi_pil_monitor>
{
public:
    const qcril_qmi_pil_state& get_pil_state() const { return pil_state; }
    int register_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr);
    int deregister_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr);

private:
    qcril_qmi_pil_monitor();
    void thread_func();

    void set_pil_state(qcril_qmi_pil_state_type new_pil_state);
    void inform_registered_client_pil_changed();
    void update_pil_state(qcril_qmi_pil_state_type new_state);

private: // data member
    qcril_qmi_pil_state pil_state;

    const static int MAX_REGISTERED_CLIENT = 4;
    qcril_qmi_pil_state_change_hdlr hdlr[MAX_REGISTERED_CLIENT];

friend class qcril_qmi_singleton_agent<qcril_qmi_pil_monitor>;
};

//===========================================================================
// qcril_qmi_pil_monitor::register_for_state_change
//===========================================================================
int qcril_qmi_pil_monitor::register_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr)
{
   int result = -1;

   for (int i=0; i<MAX_REGISTERED_CLIENT; i++)
   {
      if (NULL == this->hdlr[i])
      {
         this->hdlr[i] = hdlr;
         result = 0;
         break;
      }
   }

   QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
   return result;
} // qcril_qmi_pil_monitor::register_for_state_change

//===========================================================================
// qcril_qmi_pil_monitor::deregister_for_state_change
//===========================================================================
int qcril_qmi_pil_monitor::deregister_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr)
{
   int result = -1;

   for (int i=0; i<MAX_REGISTERED_CLIENT; i++)
   {
      if (hdlr == this->hdlr[i])
      {
         this->hdlr[i] = NULL;
         result = 0;
         break;
      }
   }

   QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
   return result;
} // qcril_qmi_pil_monitor::deregister_for_state_change

//===========================================================================
// qcril_qmi_pil_monitor::qcril_qmi_pil_monitor
//===========================================================================
qcril_qmi_pil_monitor::qcril_qmi_pil_monitor() : qcril_qmi_singleton_agent<qcril_qmi_pil_monitor>(QMI_RIL_IMS_PIL_MONITOR_THREAD_NAME)
{
   pil_state.state = QCRIL_QMI_PIL_STATE_UNKNOWN;
   memset(hdlr, 0, sizeof(hdlr));
} // qcril_qmi_pil_monitor::qcril_qmi_pil_monitor

//===========================================================================
// qcril_qmi_pil_monitor::update_pil_state
//===========================================================================
void qcril_qmi_pil_monitor::update_pil_state(qcril_qmi_pil_state_type new_state) {
    qcril_qmi_pil_state_type prev_pil_state = get_pil_state().state;

    if (prev_pil_state != new_state)
    {
      QCRIL_LOG_INFO("state changed from %d to %d. Informing client...", prev_pil_state, new_state);
      set_pil_state(new_state);
      inform_registered_client_pil_changed();
    }
} // qcril_qmi_pil_monitor::update_pil_state

//===========================================================================
// qcril_qmi_pil_monitor::thread_func
//===========================================================================
void qcril_qmi_pil_monitor::thread_func()
{
    QCRIL_LOG_FUNC_ENTRY();
    int device_fd = 0;
    int n = 0;
    int ssr_event = 0;
    char msg[UEVENT_MSG_LEN + 2];
    char slate_prop_value[PROPERTY_VALUE_MAX] = {'\0'};

    property_get(QCRIL_SLATE_PROPERTY, slate_prop_value, "0");

    if (!atoi(slate_prop_value)) {
      QCRIL_LOG_INFO("Slate is not present");
      return;
    }

    device_fd = uevent_open_socket(UEVENT_BUFFER_SIZE, true);
    if (device_fd < 0) {
      QCRIL_LOG_INFO("Failed to get PIL device...");
      return;
    }
    qcril_qmi_pil_state_type new_state = QCRIL_QMI_PIL_STATE_UNKNOWN;
    while ((n = uevent_kernel_multicast_recv(device_fd, msg, UEVENT_MSG_LEN)) > 0) {
      if (n <= 0 || n > UEVENT_MSG_LEN) {
        continue;
      }
      msg[n] = '\0';
      msg[n+1] = '\0';
      char *msg_ptr = (char *)msg;
      if (strstr(msg, QCRIL_QMI_PIL_DEVICE_NAME)) {
        while (*msg_ptr) {
          if (!strncmp(msg_ptr, SLATE_EVENT, SLATE_EVENT_STRING_LEN)) {
            msg_ptr += SLATE_EVENT_STRING_LEN;
            ssr_event = static_cast<slate_event_type>(atoi(msg_ptr));
            QCRIL_LOG_INFO("Received state event: %d ", ssr_event);
            switch ((int)ssr_event) {
              case SLATE_BEFORE_POWER_DOWN:
                new_state = QCRIL_QMI_PIL_STATE_OFFLINE;
                break;
              case  SLATE_AFTER_POWER_UP:
                new_state = QCRIL_QMI_PIL_STATE_ONLINE;
                break;
            }
            update_pil_state(new_state);
          }
          while(*msg_ptr++);
        }
      }
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_pil_monitor::thread_func

//===========================================================================
// qcril_qmi_pil_monitor::set_pil_state
//===========================================================================
void qcril_qmi_pil_monitor::set_pil_state(qcril_qmi_pil_state_type new_pil_state)
{
    QCRIL_LOG_INFO("set new pil state to %d", new_pil_state);
    pil_state.state = new_pil_state;
} // qcril_qmi_pil_monitor::set_pil_state

//===========================================================================
// qcril_qmi_pil_monitor::inform_register_client_pil_change
//===========================================================================
void qcril_qmi_pil_monitor::inform_registered_client_pil_changed()
{
    int i;
    for (i=0; i<MAX_REGISTERED_CLIENT ; i++)
    {
        if (NULL != hdlr[i])
        {
            hdlr[i](&get_pil_state());
        }
    }
} // inform_register_client_pil_change

//===========================================================================
// qcril_qmi_pil_monitor::deregister_for_state_change
//===========================================================================
int qcril_qmi_pil_init_monitor()
{
    return qcril_qmi_pil_monitor::get_instance()->init();
} // qcril_qmi_pil_init_monitor

//===========================================================================
// qcril_qmi_pil_register_for_state_change
//===========================================================================
int qcril_qmi_pil_register_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr)
{
    return qcril_qmi_pil_monitor::get_instance()->register_for_state_change(hdlr);
} // qcril_qmi_pil_register_for_state_change

//===========================================================================
// qcril_qmi_pil_deregister_for_state_change
//===========================================================================
int qcril_qmi_pil_deregister_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr)
{
    return qcril_qmi_pil_monitor::get_instance()->deregister_for_state_change(hdlr);
} // qcril_qmi_pil_deregister_for_state_change

//===========================================================================
// qcril_qmi_pil_get_pil_state
//===========================================================================
const qcril_qmi_pil_state* qcril_qmi_pil_get_pil_state()
{
    return &(qcril_qmi_pil_monitor::get_instance()->get_pil_state());
} // qcril_qmi_pil_get_pil_state

#endif
