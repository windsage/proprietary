/******************************************************************************
#  Copyright (c) 2012, 2017, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/*!
  @file
  qcril_qmi_pil_monitor.h

  @brief

*/


#ifndef QCRIL_QMI_PIL_MONITOR_H
#define QCRIL_QMI_PIL_MONITOR_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum
{
    QCRIL_QMI_PIL_STATE_MIN = -1,
    QCRIL_QMI_PIL_STATE_OFFLINE,
    QCRIL_QMI_PIL_STATE_ONLINE,
    QCRIL_QMI_PIL_STATE_UNKNOWN,
    QCRIL_QMI_PIL_STATE_MAX
} qcril_qmi_pil_state_type;

typedef struct {
    qcril_qmi_pil_state_type state;
} qcril_qmi_pil_state;

#define UEVENT_MSG_LEN  1024
#define SLATE_EVENT "SLATE_EVENT="
#define SLATE_EVENT_STRING_LEN 12 //length of SLATE_EVENT=
#define UEVENT_BUFFER_SIZE 64*1024
#define QCRIL_QMI_PIL_DEVICE_NAME "slate_com_dev"
#define QCRIL_SLATE_PROPERTY "ro.vendor.qc_aon_presence"

typedef void (*qcril_qmi_pil_state_change_hdlr)(const qcril_qmi_pil_state* cur_state);

int qcril_qmi_pil_init_monitor();
int qcril_qmi_pil_register_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr);
int qcril_qmi_pil_deregister_for_state_change(qcril_qmi_pil_state_change_hdlr hdlr);
const qcril_qmi_pil_state* qcril_qmi_pil_get_pil_state();

#ifdef  __cplusplus
}
#endif

#endif /* QCRIL_QMI_PIL_MONITOR_H */
