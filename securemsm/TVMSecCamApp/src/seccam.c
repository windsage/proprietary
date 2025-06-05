/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "seccam.h"
#include "ITEnv.h"

static bool g_camera_protected = false;
static Object g_cb_events_object = {0};
static Object g_sc_idl_obj = {0};

//=======================================================================================
/*
 * Description: Vendor owned function to take care of any required setup after
 *              camera HW has transitioned into secure streaming state
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure / to block transition.
 */
int32_t transition_to_secure_event()
{
    // setup anything which has to be initialized only AFTER secure camera
    // session was protected.
    // e.g: turning ON protected LED
    g_camera_protected = true;
    LOG_MSG("transition_to_secure_event");

exit:
    return SC_SUCCESS;
}

/*
 * Description: Vendor owned function to take care of any required teardown
 * after camera HW has transitioned out of streaming state
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure / to block transition.
 */
int32_t transition_to_non_secure_event()
{
    // release everything requiring de-initialization BEFORE secure camera
    // session will be unprotected.
    // e.g: turning OFF protected LED
    g_camera_protected = false;
    LOG_MSG("transition_to_non_secure_event");

exit:
    return SC_SUCCESS;
}

/*=======================================================================================
 *              REQUIRED Implementation of ISecureCamera2Notify
 *
 * Description: ISecureCamera2Notify will be called by Secure Camera Kernel
 *              Service. This notification is the only way a usecase app will
 *              know when the HW has successfully transitioned to
 *secure/non-secure mode
 *=======================================================================================*/
#define CSecureCamera2Notify_release(ctx) Object_OK
#define CSecureCamera2Notify_retain(ctx) Object_OK

int32_t CSecureCamera2Notify_event(void *self, uint32_t state)
{
    int32_t ret = SC_FAILURE;

    switch (state) {
        case ISecureCamera2Notify_EVENT_PROTECTED:
            LOG_MSG("camera has transitioned to secure streaming mode.");
            // Any vendor-usecase specific setup required after camera has
            // transitioned to secure mode
            T_CALL(transition_to_secure_event());
            break;
        case ISecureCamera2Notify_EVENT_UNPROTECTED:
            LOG_MSG("camera preparing to transition into non-secure streaming "
                    "mode.");
            // Any vendor-usecase specific setup required before camera has
            // transitioned to non-secure mode
            T_CALL(transition_to_non_secure_event());
            break;
        default:
            LOG_MSG("CSecureCamera2Notify_event unknown event:%d", state);
            ret = SC_FAILURE;
            break;
    }

exit:
    return (int32_t)ret;
}

static ISecureCamera2Notify_DEFINE_INVOKE(CSecureCamera2Notify_invoke,
                                          CSecureCamera2Notify_, void *);

int32_t CSecureCamera2Notify_open(Object *objOut)
{
    *objOut = (Object){CSecureCamera2Notify_invoke, NULL};

exit:
    return Object_OK;
}

/*=======================================================================================
 *              END Implementation of ISecureCamera2Notify END
 *=======================================================================================*/

/*
 * Description: Create and register callback object
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              SC_KERNEL_FAILURE on failure.
 */
int32_t register_callback()
{
    int32_t ret = SC_FAILURE;

    // Create and register callback object. Used by Secure Camera Kernel Service
    // to notify TA about streaming security state change
    T_CALL_ERR(CSecureCamera2Notify_open(&g_cb_events_object), SC_KERNEL_FAILURE);

    T_CALL(set_callback(g_cb_events_object));

exit:
    return ret;
}

/*
 * Description: Check the status of camera HW
 * In:          void
 * Out:         void
 * Return:      Object_OK on success.
 *              Object_ERROR on failure.
 */
int32_t check_camera_state()
{
    int32_t ret = SC_SUCCESS;

    // Make sure cameraHW in protected state
    T_CHECK_ERR(g_camera_protected, SC_CAMERA_NOT_PROTECTED);

exit:
    return ret;
}

/*
 * Description: Release callback and camera objects
 * In:          void
 * Out:         void
 * Return:      void
 */
void seccam_shutdown()
{
    // Release the event callback in Secure Camera Kernel Service
    set_callback(Object_NULL);
    Object_ASSIGN_NULL(g_cb_events_object);

    // Release ISecureCamera Object. This will trigger TZ kernel to notify
    // cleanup HW / Session
    Object_ASSIGN_NULL(g_sc_idl_obj);
}

/*
 * Description: A secure frame has been received from HLOS and the buffer
 *              parameters provided to this method for processing are
 *              mapped and validated to have proper security permission.
 *              This method will only be called if secure camera is in valid
 *              secure mode and it is safe to manipulate the buffers
 * In:          capture - data buffer containing secure frame data
 * Out:         rsp - response to be sent back to HLOS
 * Return:      Object_OK on success.
 *              Object_ERROR on failure.
 */
int32_t process_frame_algo(seccam_data_buf_t *capture,
                           seccam_frame_info_t *info, seccam_cmd_rsp_t *rsp)
{
    int32_t ret = SC_SUCCESS;

    T_CHECK_ERR(capture != NULL, SC_INVALID_PARAM);
    T_CHECK_ERR(info != NULL, SC_INVALID_PARAM);
    T_CHECK_ERR(capture->buf_size != 0, SC_INVALID_PARAM);

    LOG_MSG("process_frame: cam_id:%u, frame_num:%llu, timestamp:%lld, "
            "capture_buffer:0x%x, capture_buffer_size:0x%x.",
            info->cam_id, info->frame_number, info->time_stamp, capture->buf,
            capture->buf_size);
    // Authentication algorithm can be implemented here:
    // camera_buffer = capture->buf;
    // cam_buf_size = capture->buf_size;
    // status = authenticate(camera_buffer, cam_buf_size);

    // Make sure to not pass back any confidential data to HLOS!
    if (rsp != NULL) {
        rsp->ret = ret;
    }

exit:
    return ret;
}

/*
 * Description: Set OEM License for Secure Camera Feature
 * In:          license_cert - data buffer containing the license
 *              license_size - size of the buffer containing the license
 * Out:         void
 * Return:      SC_SUCCESS on success.
 *              SC_FAILURE on failure.
 */
int32_t set_license(const uint8_t *license_cert, size_t license_size)
{
    int32_t ret = SC_FAILURE;

    T_CHECK_ERR((license_size != 0) && (license_cert != NULL),
                SC_INVALID_PARAM);

    T_CALL_ERR(ISecureCamera2_setParam(g_sc_idl_obj, ISecureCamera2_PARAM_LICENSE,
               license_cert, license_size), SC_KERNEL_FAILURE);

exit:
    return ret;
}

/*
 * Description: Set Required Number of Sensors for Secure Camera Feature
 * In:          num_sensors - required number of sensors to be protected
 * Out:         void
 * Return:      SC_SUCCESS on success.
 *              SC_FAILURE on failure.
 */
int32_t set_sensors(uint32_t num_sensors)
{
    int32_t ret = SC_FAILURE;

    T_CHECK_ERR(num_sensors != 0, SC_INVALID_PARAM);

    T_CALL_ERR(ISecureCamera2_setParam(g_sc_idl_obj, ISecureCamera2_PARAM_NUM_SENSORS,
               &num_sensors, sizeof(num_sensors)), SC_KERNEL_FAILURE);

exit:
    return ret;
}

/*
 * Description: Set callback object for IDL to notify us of state changes
 * In:          cb - callback object to give IDL
 * Out:         void
 * Return:      SC_SUCCESS on success.
 *              SC_FAILURE on failure.
 */
int32_t set_callback(Object cb)
{
    int32_t ret = SC_FAILURE;

    T_CALL_ERR(ISecureCamera2_registerNotifyCB(g_sc_idl_obj, cb),
               SC_KERNEL_FAILURE);

exit:
    return ret;
}

/* Description: Function to get the camera UID object
 * In:          void
 * Out:         void
 * Return:      SC_SUCCESS in case of success
 *              Any other error code on failure
 */
int32_t open_sc_object()
{
    int32_t ret = SC_FAILURE;

    T_CHECK(Object_isNull(g_sc_idl_obj));

    // get service object
    T_CALL_ERR(ITEnv_open(gTVMEnv, CSecureCamera2_UID, &g_sc_idl_obj),
               SC_KERNEL_FAILURE);

exit:
    if (ret != SC_SUCCESS) {
        g_sc_idl_obj = Object_NULL;
    }

    return ret;
}
