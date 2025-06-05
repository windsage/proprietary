/*===========================================================================

  devices-qmi-v2.c

  DESCRIPTION
  Mitigation action over QMI.

  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

#include "thermal.h"

#include "thermal_mitigation_device_service_v01.h"
#include "devices_manager_internal.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"

struct qmi_client_info;
struct qmi_dev_info {
	char			name[DEVICES_MAX_NAME_LEN];
	uint8_t			supported;
	int			cur_level;
	int			max_level;
	pthread_mutex_t 	mtx;
	void			*data;
	struct qmi_client_info	*parent;
};

struct qmi_client_info {
	char                *name;
	qmi_client_type      handle;
	qmi_service_instance instance_id;
	pthread_mutex_t      mtx;
	pthread_t            thread;
	struct qmi_dev_info *dev_info;
	uint8_t              dev_info_cnt;
};

static struct qmi_client_info modem_clnt = {
	.name  = "MODEM",
	.instance_id  = 0,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.dev_info     = NULL,
	.dev_info_cnt = 0,
};

static struct qmi_client_info fusion_clnt = {
	.name  = "FUSION",
	.instance_id  = 0x64,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.dev_info     = NULL,
	.dev_info_cnt = 0,
};

static struct qmi_client_info adsp_clnt = {
	.name         = "ADSP",
	.instance_id  = 0x1,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.dev_info     = NULL,
	.dev_info_cnt = 0,
};

static struct qmi_client_info cdsp_clnt = {
	.name         = "CDSP",
	.instance_id  = 0x43,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.dev_info     = NULL,
	.dev_info_cnt = 0,
};

static struct qmi_client_info *register_clnts[] = {
	&modem_clnt,
	&fusion_clnt,
	&adsp_clnt,
	&cdsp_clnt,
};


static qmi_idl_service_object_type tmd_service_object;

static void qmi_clnt_error_cb(qmi_client_type clnt, qmi_client_error_type error,
			      void *error_cb_data);

static void set_req_lvl(const struct qmi_client_info *clnt_info, const char *dev,
			int req)
{
	uint8_t idx;

	for (idx = 0; idx < clnt_info->dev_info_cnt; idx++) {
		if (0 == strncasecmp(clnt_info->dev_info[idx].name, dev,
				QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01)) {
			clnt_info->dev_info[idx].cur_level = req;
			return;
		}
	}

	msg("%s: Invalid device %s", __func__, dev);
}

static int request_common_qmi(qmi_client_type clnt,
			      const char *mitigation_dev_id, int level)
{
	int ret = -1;
	tmd_set_mitigation_level_req_msg_v01 data_req;
	tmd_set_mitigation_level_resp_msg_v01 data_resp;

	if (!clnt || !mitigation_dev_id)
		return ret;

	strlcpy(data_req.mitigation_dev_id.mitigation_dev_id, mitigation_dev_id,
		QMI_TMD_MITIGATION_DEV_ID_LENGTH_MAX_V01);
	data_req.mitigation_level = (uint8_t)level;
	ret = qmi_client_send_msg_sync((qmi_client_type) clnt,
				       QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01,
				       &data_req, sizeof(data_req),
				       &data_resp, sizeof(data_resp), 0);
	return ret;
}

static int qmi_set_cur_request(struct devices_manager_dev *dev_mgr)
{
	struct qmi_dev_info *qdev = (struct qmi_dev_info *)dev_mgr->data;
	int active_req = dev_mgr->active_req.value, ret = -1;

	if (active_req < 0) {
		msg("Incorrect value %d value for cooling dev:%s\n",
			active_req, qdev->name);
		return ret;
	}
	pthread_mutex_lock(&qdev->mtx);
	if (active_req > qdev->max_level)
		active_req = qdev->max_level;

	if (active_req != qdev->cur_level) {
		dbgmsg("ACTION: %s - Setting %s Level to %d\n",
				qdev->name, qdev->name, active_req);
		thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:%s:%d\n", MITIGATION, qdev->name, active_req);
		ret = request_common_qmi(qdev->parent->handle, qdev->name,
						active_req);
		if (ret)
			msg("QMI cdev:%s level:%d send error:%d\n",
				qdev->name, active_req, ret);
		qdev->cur_level = active_req;
	}
	pthread_mutex_unlock(&qdev->mtx);

	return 0;
}

static int qmi_get_cur_request(struct devices_manager_dev *dev_mgr, int *lvl)
{
	struct qmi_dev_info *qdev = (struct qmi_dev_info *)dev_mgr->data;

	pthread_mutex_lock(&qdev->mtx);
	*lvl = qdev->cur_level;
	pthread_mutex_unlock(&qdev->mtx);

	return 0;
}

static void qmi_device_release(struct devices_manager_dev *dev_mgr)
{
	if (dev_mgr) {
		if (dev_mgr->lvl_info)
			free(dev_mgr->lvl_info);
		dev_mgr->lvl_info = NULL;
		free(dev_mgr);
	}
}

static int qmi_add_cooling_device(struct qmi_dev_info *qdev)
{
	int ret_val = 0;
	uint32_t num_lvls = 0;
	struct devices_manager_dev *dev = NULL;
	int arr_idx = 0;

	if (qdev == NULL || qdev->max_level <= 0)
		return -(EINVAL);

	dev = (struct devices_manager_dev *)malloc(sizeof(*dev));
	if (dev == NULL) {
		msg("%s: calloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}

	/* Max_state is index, so number of levels = max_state + 1 */
	num_lvls = (uint32_t)(qdev->max_level + 1);

	dev->lvl_info = (struct device_lvl_info *)calloc(num_lvls,
			sizeof(struct device_lvl_info));
	if (dev->lvl_info == NULL) {
		msg("%s: calloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	snprintf(dev->dev_info.name, DEVICES_MAX_NAME_LEN, "%s", qdev->name);

	for (arr_idx = 0; (uint32_t)arr_idx < num_lvls; arr_idx++) {
	/*	dbgmsg("%s: %s lvl_info[%d]=%d", __func__,
			dev->dev_info.name, arr_idx, arr_idx);*/
		/*
		 * Cooling device levels start from 0 to # with increasing
		 * order of mitigation.
		 * For thermal-engine provide the levels as perf levels which
		 * be in the decrementing order. That way algorithms can use
		 * these levels with device_perf_floor configs.
		 */
		dev->lvl_info[arr_idx].lvl.value = arr_idx;
		dev->lvl_info[arr_idx].perf_lvl = num_lvls - arr_idx - 1;
	}

	dev->dev_info.num_of_levels = num_lvls;
	dev->dev_info.dev_type = DEVICE_CDEV_TYPE;
	/* Needed to act as generic device */
	dev->dev_info.min_lvl = 0;

	/* Needed to act as device_op_type device */
	dev->dev_info.max_dev_op_value_valid = 1;
	dev->dev_info.max_dev_op_value = dev->lvl_info[0].lvl.value;
	dev->active_req.value = dev->dev_info.max_dev_op_value;
	dev->dev_info.min_dev_op_value_valid = 1;
	dev->dev_info.min_dev_op_value =
		dev->lvl_info[num_lvls - 1].lvl.value;
	dev->action = qmi_set_cur_request;
	dev->release = qmi_device_release;
	dev->current_op = qmi_get_cur_request;
	pthread_mutex_init(&(qdev->mtx), NULL);

	strlcpy(dev->dev_info.device_units_name,
		UNIT_NAME_LVL, MAX_UNIT_NAME_SIZE);

	dev->data = (void *)qdev;
	qdev->data = dev;

	ret_val = devices_manager_add_dev(dev);
	if (ret_val && ret_val != -(EEXIST))
		msg("%s: Can not add device\n", __func__);

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev) {
			if (dev->lvl_info)
				free(dev->lvl_info);
			free(dev);
		}
	}

	return ret_val;
}

static int verify_tmd_device(qmi_client_type clnt,
			     struct qmi_client_info *clnt_info)
{
	int rc = -1;
	unsigned int i,j;
	tmd_get_mitigation_device_list_resp_msg_v01 data_resp;

	if (clnt == NULL) {
		return -1;
	}

	memset(&data_resp, 0, sizeof(data_resp));
	rc = qmi_client_send_msg_sync(clnt,
				      QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01,
				      NULL, 0,
				      &data_resp, sizeof(data_resp), 0);
	if (rc == QMI_NO_ERR) {
		clnt_info->dev_info_cnt = data_resp.mitigation_device_list_len;
		if (data_resp.mitigation_device_list_ext01_valid)
			 clnt_info->dev_info_cnt +=
				data_resp.mitigation_device_list_ext01_len;
		clnt_info->dev_info = calloc(clnt_info->dev_info_cnt,
						sizeof(*clnt_info->dev_info));
		if (clnt_info->dev_info == NULL) {
			msg("%s: calloc failed for client dev info\n", __func__);
			return -1;
		}

		for (i = 0; i < data_resp.mitigation_device_list_len; i++) {
			tmd_mitigation_dev_list_type_v01 *device =
				(tmd_mitigation_dev_list_type_v01 *)
				&data_resp.mitigation_device_list[i];
			clnt_info->dev_info[i].supported = 1;
			clnt_info->dev_info[i].max_level =
				device->max_mitigation_level;
			clnt_info->dev_info[i].cur_level = 0;
			clnt_info->dev_info[i].parent = clnt_info;
			strlcpy(clnt_info->dev_info[i].name,
				device->mitigation_dev_id.mitigation_dev_id,
				DEVICES_MAX_NAME_LEN);
			request_common_qmi(clnt, clnt_info->dev_info[i].name, 0);
			dbgmsg("%s: Device:%s available. max:%d\n", __func__,
					clnt_info->dev_info[i].name,
					clnt_info->dev_info[i].max_level);
			qmi_add_cooling_device(&clnt_info->dev_info[i]);
		}

		/* Look for extended device list */
		for (i = data_resp.mitigation_device_list_len, j = 0;
			data_resp.mitigation_device_list_ext01_valid &&
			i < clnt_info->dev_info_cnt &&
			j < data_resp.mitigation_device_list_ext01_len;
			i++, j++) {
			tmd_mitigation_dev_list_type_v01 *device =
				(tmd_mitigation_dev_list_type_v01 *)
				&data_resp.mitigation_device_list_ext01[j];
			clnt_info->dev_info[i].supported = 1;
			clnt_info->dev_info[i].max_level =
				device->max_mitigation_level;
			clnt_info->dev_info[i].cur_level = 0;
			clnt_info->dev_info[i].parent = clnt_info;
			strlcpy(clnt_info->dev_info[i].name,
				device->mitigation_dev_id.mitigation_dev_id,
				DEVICES_MAX_NAME_LEN);
			request_common_qmi(clnt, clnt_info->dev_info[i].name, 0);
			dbgmsg("%s: Device:%s available. max:%d\n", __func__,
					clnt_info->dev_info[i].name,
					clnt_info->dev_info[i].max_level);
			qmi_add_cooling_device(&clnt_info->dev_info[i]);
		}
	} else {
		msg("%s: QMI send_msg_sync to %s failed with error %d",
		     __func__, clnt_info->name, rc);
	}

	return (rc == QMI_NO_ERR) ? 0 : -1;
}

static void *qmi_register(void *data)
{
	int rc = -1;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_client_type notifier = NULL;
	void *clnt_local = NULL;
	struct qmi_client_info *clnt_info = data;

	dbgmsg("%s: qmi register:%s id:%d\n", __func__, clnt_info->name,
			clnt_info->instance_id);
	if (clnt_info == NULL) {
		msg("%s: Invalid argument.", __func__);
		return NULL;
	}

	pthread_mutex_lock(&clnt_info->mtx);
	/* release any old handles for clnt */
	if (clnt_info->handle) {
		qmi_client_release(clnt_info->handle);
		clnt_info->handle = NULL;
	}
	pthread_mutex_unlock(&clnt_info->mtx);

	rc = qmi_client_notifier_init(tmd_service_object, &os_params,
				      &notifier);
	if (rc != QMI_NO_ERR) {
		msg("qmi: qmi_client_notifier_init failed.\n");
		goto handle_return;
	}

	while (1) {
		QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
		rc = qmi_client_get_service_instance(tmd_service_object,
						     clnt_info->instance_id,
						     &info);
		if(rc == QMI_NO_ERR)
			break;
		/* wait for server to come up */
		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	};

	dbgmsg("%s: qmi server up:%s id:%d\n", __func__, clnt_info->name,
			clnt_info->instance_id);
	rc = qmi_client_init(&info, tmd_service_object, NULL, NULL, NULL,
			     (qmi_client_type *) (&clnt_local));
	if (rc != QMI_NO_ERR) {
		msg("%s thermal mitigation not available.", clnt_info->name);
		goto handle_return;
	}
	/* Verify mitigation device present on service */
	rc = verify_tmd_device(clnt_local, clnt_info);
	if (rc != 0) {
		qmi_client_release(clnt_local);
		clnt_local = NULL;
		goto handle_return;
	}

	/* best effort register for error */
	qmi_client_register_error_cb(clnt_local, qmi_clnt_error_cb, data);
	clnt_info->handle = clnt_local;
	info("%s thermal mitigation available.", clnt_info->name);

handle_return:
	if (notifier != NULL) {
		qmi_client_release(notifier);
	}

	return NULL;
}

static void qmi_clnt_error_cb(qmi_client_type clnt,
			      qmi_client_error_type error,
			      void *error_cb_data)
{
	struct qmi_client_info *clnt_info = error_cb_data;
	uint8_t idx;

	if (clnt_info == NULL) {
		msg("%s: Invalid argument.", __func__);
		return;
	}

	info("%s: with error %d called for clnt %s\n", __func__, error,
	    clnt_info->name);

	if (clnt == NULL)
		return;

	pthread_join(clnt_info->thread, NULL);
	/* Clear supported device flag */
	for (idx = 0; idx < clnt_info->dev_info_cnt; idx++)
		clnt_info->dev_info[idx].supported = 0;
	pthread_create(&(clnt_info->thread), NULL, qmi_register,
		       (void *)clnt_info);
}

static int generic_qmi_request(int level, int max_level,
			       struct qmi_client_info *clnt_info,
			       const char *dev_name, const char *debug_name)
{
	int ret = -1;

	if (level < 0)
		level = 0;

	if (level > max_level)
		level = max_level;

	pthread_mutex_lock(&clnt_info->mtx);

	if (!clnt_info->handle) {
		info("%s[%s]:%d is recorded and waiting for "
		     "completing QMI registration",
		     debug_name, clnt_info->name, level);
		ret = 0;
		goto handle_clnt_done;
	}

	ret = request_common_qmi(clnt_info->handle, dev_name, level);
	if (ret)
		msg("%s[%s]:%d mitigation failed with %d",
		    debug_name, clnt_info->name, level, ret);
	else
		thermalmsg(LOG_LVL_INFO, (LOG_LOGCAT | LOG_TRACE),
			   "%s:%s[%s]:%d\n",
			   MITIGATION, debug_name, clnt_info->name, level);

handle_clnt_done:
	/* Save previous request to handle subsytem restart, and
	   requests prior to QMI service being available. */
	set_req_lvl(clnt_info, dev_name, level);
	pthread_mutex_unlock(&clnt_info->mtx);

	return ret;
}

int qmi_communication_init(void)
{
	unsigned int idx = 0;
	/* Get the service object for the tmd API */
	tmd_service_object = tmd_get_service_object_v01();
	if (!tmd_service_object) {
		msg("qmi: tmd_get_service_object failed.\n");
		return -1;
	}

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++)
	{
		/* start thread to connect to remote QMI services */
		pthread_create(&(register_clnts[idx]->thread), NULL, qmi_register,
			       (void*)register_clnts[idx]);
	}

	return 0;
}

int qmi_communication_release(void)
{
	int rc = 0;
	int ret_val = 0;
	unsigned int idx = 0;

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
		pthread_join(register_clnts[idx]->thread, NULL);
	}

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
		if (register_clnts[idx]->handle) {
			rc = qmi_client_release(register_clnts[idx]->handle);
			if (rc) {
				msg("qmi: qmi_client_release %s clnt failed.\n",
				    register_clnts[idx]->name);
				ret_val = -1;
			}
			register_clnts[idx]->handle = NULL;
			if (register_clnts[idx]->dev_info) {
				free(register_clnts[idx]->dev_info);
				register_clnts[idx]->dev_info = NULL;
			}
		}
	}

	return ret_val;
}
