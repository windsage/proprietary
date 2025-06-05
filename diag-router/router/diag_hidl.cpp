/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service implementation for open, ioctl, write calls.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "diag_hidl.h"
#include "diag_hidl_util.h"
#include "dm.h"
#include "diag.h"
#include "diag_cntl.h"
#include "masks.h"
#include "diag_dci.h"
#include "diag_mux.h"
#include <string.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#define DATA_TYPE_HIDL_SOCKET	46
#define MAX_PKT_SIZE 100000

extern int hidl_sock_fd;
using android::hardware::IPCThreadState;
using android::hardware::Void;
using android::hardware::Return;
using android::hardware::hidl_string;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;
using vendor::qti::diaghal::V1_0::implementation::diagService;

struct diaghalDeathRecipient : virtual public hidl_death_recipient {
	virtual void serviceDied(uint64_t caller_pid, const android::wp<IBase>&) override {
		auto diag = diagService::getInstance();

		ALOGE("diaghal just died for pid %lu\n", caller_pid);
		remove_client_info_from_list(caller_pid);
		pthread_mutex_lock(&diagmem->dci_mutex);
		diag_cleanup_dci_client_entry(caller_pid);
		pthread_mutex_unlock(&diagmem->dci_mutex);
		diag_close_logging_process(NULL, caller_pid);
		remove_client_registrations_from_apps(caller_pid);
		diag->dmRemove(caller_pid);
      }
};
static android::sp<diaghalDeathRecipient> halDeathRecipient = nullptr;
namespace vendor {
namespace qti {
namespace diaghal {
namespace V1_0 {
namespace implementation {

diagService *diagService::instance = nullptr;

Return<int32_t> diagService::open(const sp<Idiagcallback>& callback)
{
	pid_t caller_pid = IPCThreadState::self()->getCallingPid();

	this->dmAdd(caller_pid);
	add_client_info_to_list(callback, caller_pid);
	if (halDeathRecipient == nullptr) {
		halDeathRecipient = new diaghalDeathRecipient();
	}
	Return<bool> death_linked = callback->linkToDeath(halDeathRecipient, caller_pid);
	if (!death_linked || !death_linked.isOk()) {
		ALOGE("linking diaghal to death failed: %s", death_linked.description().c_str());
	} else {
		ALOGE("diaghal linked to death!! for pid %d\n", caller_pid);
	}
	return 0;
}
Return<int32_t> diagService::ioctl(uint32_t cmd, const hidl_memory& buf, uint32_t len)
{
	void *data;
	pid_t caller_pid;
	sp<IMemory> memory;
	hidl_memory mem_s;
	int ret = 0;
	int socket_data_type = DATA_TYPE_HIDL_SOCKET;
	uint64_t mem_size = 0;

	switch(cmd) {
	case DIAG_IOCTL_COMMAND_REG:
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}
		data = static_cast<void*>(memory->getPointer());
		caller_pid = IPCThreadState::self()->getCallingPid();

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				__func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}
		add_client_registrations_to_apps(data, len, caller_pid);
		break;
	case DIAG_IOCTL_COMMAND_DEREG:
		caller_pid = IPCThreadState::self()->getCallingPid();
		remove_client_registrations_from_apps(caller_pid);
		break;
	case DIAG_IOCTL_GET_DELAYED_RSP_ID:
	{
		uint16_t delayed_rsp_id;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(delayed_rsp_id)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		caller_pid = android::hardware::IPCThreadState::self()->getCallingPid();
		memory->update();
		delayed_rsp_id = diag_get_next_delayed_rsp_id();
		memcpy(data, &delayed_rsp_id, sizeof(uint16_t));
		memory->commit();
	}
		break;
	case DIAG_IOCTL_REMOTE_DEV:
	{
		int remote_mask;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(remote_mask)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		caller_pid = android::hardware::IPCThreadState::self()->getCallingPid();
		memory->update();
		remote_mask = diag_get_remote_mask_info();
		memcpy(data, &remote_mask, sizeof(uint16_t));
		memory->commit();
	}
		break;
	case DIAG_IOCTL_QUERY_CON_ALL:
	{
		struct diag_con_all_param_t con_params;

		memset(&con_params, 0, sizeof(con_params));
		con_params.diag_con_all = DIAG_CON_ALL;
		con_params.num_peripherals = NUM_PERIPHERALS;
		con_params.upd_map_supported = 1;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(con_params)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		caller_pid = android::hardware::IPCThreadState::self()->getCallingPid();

		memory->update();
		memcpy(data, &con_params, sizeof(con_params));
		memory->commit();
	}
		break;
	case DIAG_IOCTL_QUERY_MD_PID:
	{
		struct diag_query_pid_t *pid_params;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(*pid_params)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		caller_pid = android::hardware::IPCThreadState::self()->getCallingPid();

		memory->update();
		pid_params = (struct diag_query_pid_t *)data;
		diag_query_diag_session_pid(pid_params);
		memcpy(data, pid_params, sizeof(*pid_params));
		memory->commit();
	}
		break;
	case DIAG_IOCTL_SWITCH_LOGGING:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		diag_clear_system_data_ready();
		ret = diag_switch_logging(NULL, (struct diag_logging_mode_param_t *)data, len);
		break;
	}
	case DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		ret = diag_set_peripheral_buffering_mode((struct diag_buffering_mode_t *)data, len);
	}
		break;
	case DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		ret = diag_peripheral_data_drain_immediate((struct diag_perif_drain *)data, len);
	}
		break;
	case DIAG_IOCTL_LSM_DEINIT:
	{
		int data_type = 0x00000010;
		sp<Idiagcallback> cb;

		caller_pid = IPCThreadState::self()->getCallingPid();
		ALOGE("diag: In %s Received LSM DEINIT request for client: %d\n", __func__, caller_pid);
		cb = get_client_info_from_pid(caller_pid);
		if (!cb) {
			ret = -EINVAL;
			break;
		}
		mem_s = diag_get_hidl_buffer();
		memory = mapMemory(mem_s);
		if (memory == nullptr) {
			ALOGE("%s: Could not map HIDL memory to IMemory", __func__);
			ret = -ENOMEM;
			break;
		}
		unsigned char *data = static_cast<unsigned char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(data_type)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		memory->update();
		memcpy(data, &data_type, sizeof(data_type));
		memory->commit();
		auto ret = cb->send_data(mem_s, sizeof(data_type));
		if (!ret.isOk())
			ALOGE("diag: unable to update deinit\n");
		ALOGE("diag: In %s Completed sending deinit pkt to client: %d\n", __func__, caller_pid);
		remove_client_info_from_list(caller_pid);
		diag_close_logging_process(NULL, caller_pid);
		remove_client_registrations_from_apps(caller_pid);
		cb->unlinkToDeath(halDeathRecipient);
		this->dmRemove(caller_pid);
		ALOGE("diag: In %s Completed LSM DEINIT request for client: %d\n", __func__, caller_pid);
	}
		break;
	case DIAG_IOCTL_QUERY_MASK:
	{
		int len;
		pid_t caller_pid = IPCThreadState::self()->getCallingPid();
		sp<Idiagcallback> cb;
		cb = get_client_info_from_pid(caller_pid);
		if (!cb) {
			ret = -EINVAL;
			break;
		}
		mem_s = diag_get_hidl_buffer();
		memory = mapMemory(mem_s);
		if (memory == nullptr) {
			printf("%s: Could not map HIDL memory to IMemory", __func__);
			ret = -ENOMEM;
			break;
		}
		unsigned char*data = static_cast<unsigned char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		memory->update();
		len = diag_msg_mask_update_hidl_client(data);
		memory->commit();
		auto ret = cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update msg mask\n");
		memory->update();
		len = diag_log_mask_update_hidl_client(data);
		memory->commit();
		ret= cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update log mask\n");
		memory->update();
		len = diag_event_mask_update_hidl_client(data);
		memory->commit();
		ret = cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update event mask\n");
		memory->update();
		len = diag_dci_mask_update_hidl_client(data, DCI_LOG_MASKS_TYPE);
		memory->commit();
		ret = cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update dci log mask\n");
		memory->update();
		len = diag_dci_mask_update_hidl_client(data, DCI_EVENT_MASKS_TYPE);
		memory->commit();
		ret = cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update dci event mask\n");

		memory->update();
		len = diag_timestamp_switch_update_hidl_client(data);
		memory->commit();
		ret= cb->send_data(mem_s, len);
		if (!ret.isOk())
			ALOGE("diag: unable to update timestamp switch value\n");
	}
		break;
	case DIAG_IOCTL_DCI_REG:
	{
		pid_t caller_pid = IPCThreadState::self()->getCallingPid();

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		ALOGE("diag: received dci reg ioctl\n");

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		ret = diag_dci_register_client((struct diag_dci_reg_tbl_t *)data, NULL, caller_pid);
		break;
	}
	case DIAG_IOCTL_DCI_SUPPORT:
	{
		struct diag_dci_peripherals_t dci_support;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(dci_support)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		memcpy(&dci_support, data, sizeof(dci_support));
		ALOGE("diag: DCI support query for periph: %d proc: %d\n", dci_support.list, dci_support.proc);
		ret = diag_dci_get_support_list(&dci_support);
		memory->update();
		memcpy(data, &dci_support, sizeof(dci_support));
		memory->commit();
		break;
	}
	case DIAG_IOCTL_DCI_HEALTH_STATS:
	{
		struct  diag_dci_health_stats_proc health_stats;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}
		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(health_stats)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		memcpy(&health_stats, data, sizeof(health_stats));
		ret = diag_dci_copy_health_stats(&health_stats);
		memory->update();
		memcpy(data, &health_stats, sizeof(health_stats));
		memory->commit();
		break;
	}
	case DIAG_IOCTL_DCI_LOG_STATUS:
	{
		struct diag_log_event_stats *log_status;
		struct diag_dci_client_tbl *dci_client = NULL;
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		log_status = (struct diag_log_event_stats *)data;
		dci_client = diag_dci_get_client_entry(log_status->client_id);
		memory->update();
		log_status->is_set = diag_dci_query_log_mask(dci_client, log_status->code);
		memory->commit();
		ret = DIAG_DCI_NO_ERROR;
		break;
	}
	case DIAG_IOCTL_DCI_EVENT_STATUS:
	{
		struct diag_log_event_stats *event_status;
		struct diag_dci_client_tbl *dci_client = NULL;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		event_status = (struct diag_log_event_stats*)data;
		dci_client = diag_dci_get_client_entry(event_status->client_id);
		memory->update();
		event_status->is_set = diag_dci_query_event_mask(dci_client, event_status->code);
		memory->commit();
		ret = DIAG_DCI_NO_ERROR;
		break;
	}
	case DIAG_IOCTL_DCI_CLEAR_LOGS:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}
		data = static_cast<int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		ret = diag_dci_clear_log_mask(*(int *)data);
		break;
	}
	case DIAG_IOCTL_DCI_CLEAR_EVENTS:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		ret = diag_dci_clear_event_mask(*(int *)data);
		break;
	}
	case DIAG_IOCTL_DCI_DEINIT:
	{
		struct diag_dci_client_tbl *dci_client;
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}
		data = static_cast<unsigned int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		pthread_mutex_lock(&diagmem->dci_mutex);
		dci_client = diag_dci_get_client_entry(*(int *)data);
		if (!dci_client) {
			pthread_mutex_unlock(&diagmem->dci_mutex);
			ret = DIAG_DCI_NOT_SUPPORTED;
			break;
		}
		ret = diag_dci_deinit_client(dci_client);
		pthread_mutex_unlock(&diagmem->dci_mutex);
		break;
	}
	case DIAG_IOCTL_VOTE_REAL_TIME:
	{
		 memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<unsigned int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}
		ret = diag_process_vote_realtime(( struct real_time_vote_t *) data, sizeof(struct  real_time_vote_t ));
		break;
	}
	case DIAG_IOCTL_GET_REAL_TIME:
	{
		struct real_time_query_t rt_query;

		if (len > sizeof(rt_query))
			return -EINVAL;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<unsigned int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < sizeof(rt_query)) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		memcpy(&rt_query, data, len);
		ret = diag_get_real_time(&rt_query, sizeof(rt_query));
		memory->update();
		memcpy(data, &rt_query, sizeof(rt_query));
		memory->commit();
		break;
	}
	case DIAG_IOCTL_QUERY_PD_LOGGING:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			return -EINVAL;
		}
		data = static_cast<unsigned int*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			return -ENOMEM;
		}
		return diag_query_pd_logging((struct diag_logging_mode_param_t *)data);
	}
	case DIAG_IOCTL_SET_OVERRIDE_PID:
	{
		struct diag_client *dm;
		pid_t pid;

		caller_pid = IPCThreadState::self()->getCallingPid();
		memory = mapMemory(buf);
		if (memory == nullptr)
			return -EINVAL;
		data = static_cast<unsigned int *>(static_cast<void *>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("diag: %s: Could not get pointer to memory\n", __func__);
			return -ENOMEM;
		}
		dm = dmGet(caller_pid);
		if (!dm) {
			ALOGE("diag: %s: No dm found for pid %d\n", __func__, caller_pid);
			return -ENODEV;
		}
		pid = *(pid_t *)data;
		if (pid < 0 || pid >= PID_MAX) {
			ALOGE("diag: %s: pid: %d invalid override_pid: %d\n", __func__, caller_pid, pid);
			return -EINVAL;
		}
		ALOGE("diag: %s: pid: %d set override_pid: %d\n", __func__, caller_pid, pid);
		dm_set_override_pid(dm, pid);
		break;
	}
	case DIAG_IOCTL_REGISTER_CALLBACK:
	{
		struct diag_callback_reg_t reg;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			return -EINVAL;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			return -ENOMEM;
		}
		memcpy(&reg, data, sizeof(struct diag_callback_reg_t));
		if (reg.proc < 0 || reg.proc >= NUM_DIAG_MD_DEV)
			return -EINVAL;
		if (diagmem->md_session_mode[reg.proc] == DIAG_MD_PERIPHERAL)
			return -EIO;

		return 0;
	}
	case DIAG_IOCTL_HDLC_TOGGLE:
	{
		pid_t caller_pid = IPCThreadState::self()->getCallingPid();
		uint8_t hdlc_support;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			return -EINVAL;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			return -ENOMEM;
		}
		memcpy(&hdlc_support, data, sizeof(uint8_t));
		return diag_hdlc_toggle(caller_pid, hdlc_support);
	}
	case DIAG_IOCTL_MDM_HDLC_TOGGLE:
	{
		pid_t caller_pid = IPCThreadState::self()->getCallingPid();
		uint8_t hdlc_support;

		memory = mapMemory(buf);
		if (memory == nullptr) {
			return -EINVAL;
		}
		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("%s: Could not get pointer to memory", __func__);
			return -ENOMEM;
		}
		memcpy(&hdlc_support, data, sizeof(uint8_t));
		return diag_mdm_hdlc_toggle(caller_pid, hdlc_support);
	}
	case DIAG_IOCTL_VOTE_DCI_BUFFERING_MODE:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("diag: %s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		ret = diag_dci_vote_peripheral_buffering_mode((struct diag_dci_buffering_mode_t *)data, len);
	}
		break;
	case DIAG_IOCTL_DCI_DRAIN_IMMEDIATE:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			ALOGE("diag: %s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		mem_size = memory->getSize();
		if (mem_size < len) {
			ALOGE("diag: %s: insufficient buf size: %lld for cmd pkt: %d of size %d\n",
				 __func__, mem_size, cmd, len);
			ret = -ENOMEM;
			break;
		}

		ret = diag_dci_peripheral_data_drain_immediate((struct diag_dci_perif_drain *)data, len);
	}
		break;
	case DATA_TYPE_UPDATE_QDSS_ETR1_SUPPORT:
	{
		memory = mapMemory(buf);
		if (memory == nullptr) {
			ret = -EINVAL;
			break;
		}

		data = static_cast<char*>(static_cast<void*>(memory->getPointer()));
		if (data == nullptr) {
			printf("%s: Could not get pointer to memory", __func__);
			ret = -ENOMEM;
			break;
		}

		memcpy(&etr1_support, data, sizeof(etr1_support));
	}
		break;
	default:
		break;
	}

	/*
	 * For HIDL, the processing of IOCTL events are not part of the watch thread but directly
	 * from this one. Once the processing is done, the watch thread does not run until the next
	 * event on a FD happens. Therefore, manually trigger an event on hidl_sock_fd to resume
	 * the watch thread immediately.
	 */
	::write(hidl_sock_fd, &socket_data_type, sizeof(socket_data_type));

	return ret;
}

Return<int32_t> diagService::write(const hidl_memory& buf, uint32_t len)
{
	sp<IMemory> memory;
	unsigned char *data;
	unsigned char *base;
	hidl_memory mem_s;
	int data_type = 0;
	int socket_data_type = DATA_TYPE_HIDL_SOCKET;
	memory = mapMemory(buf);
	if (memory == nullptr) {
		return 0;
	}
	data = static_cast<unsigned char*>(static_cast<void*>(memory->getPointer()));
	base = data;
	pid_t caller_pid = android::hardware::IPCThreadState::self()->getCallingPid();
	memcpy(&data_type, data, sizeof(data_type));
	data = data + sizeof(data_type);
	switch (data_type) {
	case DATA_TYPE_RSP:
	case DATA_TYPE_DELAYED_RSP:
		if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
			ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd pkt written to client of fd: %d of len: %d\n",
				__func__, this->fds[1], len - 4);
			print_hex_dump(__func__, (void *)base, MIN((len -4), PRINT_HEX_DUMP_LEN));
		}
	case DATA_TYPE_EVENT:
	case DATA_TYPE_LOG:
	case DATA_TYPE_F3:
		pthread_mutex_lock(&this->lock);
		::write(this->fds[1], base, len);
		pthread_mutex_unlock(&this->lock);
		break;
	case USER_SPACE_DATA_TYPE:
	{
		struct diag_client *dm = dmGet(caller_pid);

		if (dm)
			caller_pid = dm->override_pid ? dm->override_pid : dm->pid;

		diag_process_userspace_data(NULL, data, len - 4, caller_pid);
		if (hidl_sock_fd >= 0)
			::write(hidl_sock_fd, &socket_data_type, sizeof(socket_data_type));
		break;
	}
	case USER_SPACE_RAW_DATA_TYPE:
	{
		struct diag_client *dm = dmGet(caller_pid);

		if (dm)
			caller_pid = dm->override_pid ? dm->override_pid : dm->pid;

		diag_process_userspace_raw_data(NULL, data, len - 4, caller_pid);
		if (hidl_sock_fd >= 0)
			::write(hidl_sock_fd, &socket_data_type, sizeof(socket_data_type));
		break;
	}
	case DATA_TYPE_DCI:
	{
		int ret;

		ret =  diag_process_dci_transaction(data, len - 4);
		if (hidl_sock_fd >= 0)
			::write(hidl_sock_fd, &socket_data_type, sizeof(socket_data_type));
		return ret;
	}
	case DATA_TYPE_DCI_LOG:
	case DATA_TYPE_DCI_EVENT:
	case DATA_TYPE_DCI_PKT:
		diag_process_apps_dci_read_data((void*)data, len - 4, this->flow);
		 if (hidl_sock_fd >= 0)
			::write(hidl_sock_fd, &socket_data_type, sizeof(socket_data_type));
		break;
	default:
		break;
	}
	return 0;
}

Return<int32_t> diagService::close()
{
	return 0;
}

diagService *diagService::getInstance(void)
{
	if (!instance)
		instance = new diagService;
	return instance;
}

struct diag_client *diagService::dmGet(pid_t pid)
{
	struct diag_client *dm;

	list_for_each_entry(dm, &this->dms, node) {
		if (dm->pid == pid)
			return dm;
	}
	return NULL;
}

struct diag_client *diagService::dmAdd(pid_t pid)
{
	struct diag_client *dm = (struct diag_client *)calloc(1, sizeof(*dm));

	if (!dm)
		err(1, "diag: %s: failed to allocate DM context\n", __func__);

	dm_set_pid(dm, pid);
	list_add(&this->dms, &dm->node);
	return dm;
}

void diagService::dmRemove(pid_t pid)
{
	struct diag_client *dm = this->dmGet(pid);

	if (!dm)
		return;

	list_del(&dm->node);
	free(dm);
}

int diagService::serviceInit(void)
{
	int ret;

	if (fds[0] >= 0)
		return 0;

	this->buf = malloc(MAX_PKT_SIZE);
	if (!this->buf) {
		ALOGE("diag: %s: Failed to allocate receive buffer\n", __func__);
		return -1;
	}
	this->flow = watch_flow_new();

	pthread_mutex_init(&this->lock, NULL);

	ret = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, this->fds);
	if (ret < 0) {
		ALOGE("diag: %s: Failed to create socket pair %d\n", __func__, errno);
		return -1;
	}
	fcntl(this->fds[0], F_SETFL, O_NONBLOCK);
	fcntl(this->fds[1], F_SETFL, O_NONBLOCK);
	watch_add_readfd(this->fds[0], this->readFromWatch, (void *)this, this->flow);

	return 0;
}

int diagService::readFromWatch(int fd, void *data)
{
	auto diag = static_cast<diagService *>(data);
	struct hidl_pkt {
		int type;
		unsigned char data[];
	};
	struct hidl_pkt *pkt;
	int data_len;
	int n;
	unsigned char non_hdlc_buf[DIAG_MAX_RSP_SIZE+5];

	pthread_mutex_lock(&diag->lock);
	while (1) {
		n = read(fd, diag->buf, MAX_PKT_SIZE);
		if (n <= 0)
			break;

		pkt = (struct hidl_pkt *)diag->buf;
		data_len = n - offsetof(struct hidl_pkt, data);
		switch (pkt->type) {
		case DATA_TYPE_EVENT:
		case DATA_TYPE_LOG:
		case DATA_TYPE_F3:
			diag_process_apps_data(pkt->data, data_len, non_hdlc_buf, sizeof(non_hdlc_buf), 0);
			break;
		case DATA_TYPE_RSP:
		case DATA_TYPE_DELAYED_RSP:
			if (diag_debug_mask & DIAG_DBG_MASK_CMD) {
				ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd rsp from client of fd: %d of len: %d\n",
					__func__, fd, data_len);
				print_hex_dump(__func__, (void *)(non_hdlc_buf), MIN(data_len, PRINT_HEX_DUMP_LEN));
			}
			diag_process_apps_data(pkt->data, data_len, non_hdlc_buf, sizeof(non_hdlc_buf), 1);
			break;
		}
	}
	pthread_mutex_unlock(&diag->lock);

	return 0;
}

diagService::diagService(void) : fds{-1, -1} {
	list_init(&dms);
}

} // namespace implementation

} // namespace v1_0
} // namespace diaghal
} // namespace qti

} // namespace vendor
