/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service utils implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "diag_hidl_util.h"
#include "diag.h"
#include "diag_dci.h"
#include <hwbinder/IPCThreadState.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <hidl/LegacySupport.h>
#include <stdio.h>
#include <pthread.h>
#include "diag_hidl_wrapper.h"
#include "masks.h"
#include <errno.h>
int data_ready = 0;
using vendor::qti::diaghal::V1_0::Idiagcallback;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;
using ::android::hardware::HidlMemory;
using ::android::hardware::hidl_handle;
using android::sp;
sp<IAllocator> ashmemAllocator;
hidl_memory mem_s, mem_s_1;
sp<IMemory> memory;
std::map<pid_t, sp<Idiagcallback>> client_info;
std::map<pid_t, sp<Idiagcallback>>::iterator it;
struct list_head apps_system_cmds = LIST_INIT(apps_system_cmds);
pthread_t diag_hidl_thread_hdl;

static pthread_mutex_t read_cmd_mutex;
static pthread_mutex_t packet_copy_mutex;
static pthread_mutex_t packet_ready_mutex;
static pthread_mutex_t copy_done_mutex;
static pthread_mutex_t dci_packet_copy_mutex;
static pthread_mutex_t dci_packet_ready_mutex;
static pthread_mutex_t dci_copy_done_mutex;

static pthread_mutex_t hidl_copy_client_mutex;
static pthread_mutex_t dci_hidl_copy_client_mutex;

pthread_cond_t read_cmd_cond;
pthread_cond_t packet_copy_cond;
pthread_cond_t packet_ready_cond;
pthread_cond_t copy_done_cond;
pthread_cond_t dci_packet_copy_cond;
pthread_cond_t dci_packet_ready_cond;
pthread_cond_t dci_copy_done_cond;

int packet_ready = 0;
int packet_copy = 0;
int copy_done = 0;
int dci_packet_ready = 0;
int dci_packet_copy = 0;
int dci_copy_done = 0;

int pkt_data_ready = 0;
int dci_pkt_data_ready = 0;
int msg_mask_data_ready = 0;
int log_mask_data_ready = 0;
int event_mask_data_ready = 0;
int timestamp_switch_data_ready = 0;
int dci_log_mask_data_ready = 0;
int dci_event_mask_data_ready = 0;
int userspace_data_ready = 0;
int dci_data_ready = 0;
int dci_buffering_mode_data_ready = 0;

int cmd_len = 0;
unsigned char apps_buf[DIAG_MAX_REQ_SIZE];
pid_t cmd_pid;

pid_t hidl_pid;
pid_t hidl_dci_pid;

unsigned char hidl_buf[READ_BUF_SIZE];
int hidl_buf_packet_len;

unsigned char hidl_dci_buf[READ_BUF_SIZE];
int hidl_dci_buf_packet_len;

hidl_memory diag_get_hidl_buffer()
{
	return mem_s_1;
}
void add_client_info_to_list(const sp<Idiagcallback>& callback, pid_t pid)
{
	pthread_mutex_lock(&read_cmd_mutex);
	client_info[pid] = callback;
	pthread_mutex_unlock(&read_cmd_mutex);
}

void remove_client_info_from_list(pid_t pid)
{
	pthread_mutex_lock(&read_cmd_mutex);
	it = client_info.find(pid);
	if (it != client_info.end())
		client_info.erase(it);
	pthread_mutex_unlock(&read_cmd_mutex);
}

sp<Idiagcallback> get_client_info_from_pid(pid_t pid)
{
	sp<Idiagcallback> callback;

	pthread_mutex_lock(&read_cmd_mutex);
	it = client_info.find(pid);
	if (it != client_info.end()) {
		callback = it->second;
		pthread_mutex_unlock(&read_cmd_mutex);
		return callback;
	}
	else {
		pthread_mutex_unlock(&read_cmd_mutex);
		return NULL;
	}
}

void remove_client_registrations_from_apps(pid_t pid)
{
	struct diag_cmd *dc;
	struct list_head *item;
	struct list_head *next;

	list_for_each_safe(item, next, &apps_system_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc->fd == pid) {
			list_del(&dc->node);
			free(dc);
		}
        }
	return;

}

int add_client_registrations_to_apps(void *data, int len, pid_t pid)
{
	struct diag_cmd_reg_tbl *pkt = (struct diag_cmd_reg_tbl*)data;
	struct diag_cmd *dc;
	unsigned int subsys;
	unsigned int cmd;
	unsigned int first;
	unsigned int last;
	int i;
	(void)len;

	if (!pkt || pkt->count > (len - sizeof(uint32_t)) / sizeof(diag_cmd_reg_entry))
		return -EINVAL;

	for (i = 0; i < pkt->count; i++) {
		cmd = pkt->entries[i].cmd;
		subsys = pkt->entries[i].subsys;
		if (cmd == DIAG_CMD_NO_SUBSYS_DISPATCH && subsys != DIAG_CMD_NO_SUBSYS_DISPATCH)
			cmd = DIAG_CMD_SUBSYS_DISPATCH;
		first = cmd << 24 | subsys << 16 | pkt->entries[i].first;
		last = cmd << 24 | subsys << 16 | pkt->entries[i].last;

		dc = (struct diag_cmd *)malloc(sizeof(*dc));
		if (!dc) {
			return -ENOMEM;
		}
		memset(dc, 0, sizeof(*dc));

		dc->first = first;
		dc->last = last;
		dc->fd = pid;
		list_add(&apps_system_cmds, &dc->node);
	}

	return 0;
}

int check_for_diag_system_client_commands(unsigned int key, void *ptr, int len)
{
	int pkt_type = PKT_TYPE;
	struct diag_cmd *dc;
	struct list_head *item;

	list_for_each(item, &apps_system_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last)
			continue;
		memcpy(apps_buf,&pkt_type,sizeof(pkt_type));
		memcpy(apps_buf + sizeof(pkt_type), ptr, len);
		cmd_pid = dc->fd;
		cmd_len = len + 4;
		diag_update_hidl_client(PKT_TYPE);
		return 1;
	}
	return 0;
}

int check_for_diag_dci_system_client_commands(unsigned int key, void *ptr, int len, int tag)
{
        int pkt_type = DCI_PKT_TYPE;
        struct diag_cmd *dc;
        int write_len = 0;
        struct list_head *item;
	struct diag_dci_pkt_header_t header;

        list_for_each(item, &apps_system_cmds) {
                dc = container_of(item, struct diag_cmd, node);
                if (key < dc->first || key > dc->last)
                        continue;
			memcpy(apps_buf, &pkt_type, sizeof(pkt_type));
			write_len = write_len + sizeof(pkt_type);
			header.start = 0x7e;
			header.version = 1;
			header.len = len + sizeof(int) + sizeof(uint8_t);
			header.pkt_code = DCI_PKT_RSP_CODE;
			header.tag = tag;
			memcpy(apps_buf+write_len, &header, sizeof(header));
			write_len += sizeof(header);
			memcpy(apps_buf + write_len, ptr, len);
			write_len += len;
			*(uint8_t *)(apps_buf + write_len) = 0x7e;
			write_len += sizeof(uint8_t);
			cmd_pid = dc->fd;
			cmd_len = write_len;
			diag_update_hidl_client(DCI_PKT_TYPE);
			return 1;
	}
	return 0;
}

void diag_clear_system_data_ready(void)
{
	pthread_mutex_lock(&read_cmd_mutex);
	data_ready = 0;
	pkt_data_ready = 0;
	msg_mask_data_ready = 0;
	log_mask_data_ready = 0;
	event_mask_data_ready = 0;
	timestamp_switch_data_ready = 0;
	pthread_mutex_unlock(&read_cmd_mutex);
}

void diag_clear_system_dci_data_ready(void)
{
	pthread_mutex_lock(&read_cmd_mutex);
	dci_data_ready = 0;
	dci_pkt_data_ready = 0;
	dci_log_mask_data_ready = 0;
	dci_event_mask_data_ready = 0;;
	pthread_mutex_unlock(&read_cmd_mutex);
}

void diag_update_hidl_client(int type)
{
	pthread_mutex_lock(&read_cmd_mutex);
	data_ready = data_ready | type;

	if (type & PKT_TYPE)
		pkt_data_ready++;
	if (type & DCI_PKT_TYPE)
		dci_pkt_data_ready++;
	if (type & MSG_MASKS_TYPE)
		msg_mask_data_ready++;
	if (type & LOG_MASKS_TYPE)
		log_mask_data_ready++;
	if (type & EVENT_MASKS_TYPE)
		event_mask_data_ready++;
	if (type & TIMESTAMP_SWITCH_TYPE)
		timestamp_switch_data_ready++;
	if (type & DCI_LOG_MASKS_TYPE)
		dci_log_mask_data_ready++;
	if (type & DCI_EVENT_MASKS_TYPE)
		dci_event_mask_data_ready++;
	if (type & USER_SPACE_DATA_TYPE)
		userspace_data_ready++;
	if (type & DATA_TYPE_DCI)
		dci_data_ready++;
	if (type & DCI_BUFFERING_MODE_STATUS_TYPE)
		dci_buffering_mode_data_ready++;
	pthread_cond_signal(&read_cmd_cond);
	pthread_mutex_unlock(&read_cmd_mutex);
}

void diag_signal_copy_complete(void)
{
	pthread_mutex_lock(&packet_copy_mutex);
	pthread_cond_signal(&packet_copy_cond);
	packet_copy = 1;
	pthread_mutex_unlock(&packet_copy_mutex);
}

void diag_signal_ready(void)
{
	pthread_mutex_lock(&packet_ready_mutex);
	pthread_cond_signal(&packet_ready_cond);
	packet_ready = 1;
	pthread_mutex_unlock(&packet_ready_mutex);
}

void diag_signal_return(void)
{
	pthread_mutex_lock(&copy_done_mutex);
	pthread_cond_signal(&copy_done_cond);
	copy_done = 1;
	pthread_mutex_unlock(&copy_done_mutex);
}

void diag_copy_for_hidl_client(unsigned char *buf, int len, int pid)
{
	pthread_mutex_lock(&hidl_copy_client_mutex);
	pthread_mutex_lock(&packet_ready_mutex);
	if (!packet_ready)
		pthread_cond_wait(&packet_ready_cond, &packet_ready_mutex);
	memcpy(hidl_buf, buf, len);
	hidl_buf_packet_len = len;
	hidl_pid = pid;
	packet_ready = 0;
	pthread_mutex_unlock(&packet_ready_mutex);

	diag_signal_copy_complete();

	pthread_mutex_lock(&copy_done_mutex);
	if (!copy_done)
		pthread_cond_wait(&copy_done_cond, &copy_done_mutex);
	copy_done = 0;
	pthread_mutex_unlock(&copy_done_mutex);
	pthread_mutex_unlock(&hidl_copy_client_mutex);
}
void diag_signal_dci_copy_complete(void)
{
	pthread_mutex_lock(&dci_packet_copy_mutex);
	pthread_cond_signal(&dci_packet_copy_cond);
	dci_packet_copy = 1;
	pthread_mutex_unlock(&dci_packet_copy_mutex);
}

void diag_signal_dci_ready(void)
{
	pthread_mutex_lock(&dci_packet_ready_mutex);
	pthread_cond_signal(&dci_packet_ready_cond);
	dci_packet_ready = 1;
	pthread_mutex_unlock(&dci_packet_ready_mutex);
}

void diag_signal_dci_return(void)
{
	pthread_mutex_lock(&dci_copy_done_mutex);
	pthread_cond_signal(&dci_copy_done_cond);
	dci_copy_done = 1;
	pthread_mutex_unlock(&dci_copy_done_mutex);
}

void diag_fwd_dci_data(pid_t pid, void *buf, int len)
{
	diag_update_hidl_client(DATA_TYPE_DCI);
	pthread_mutex_lock(&dci_hidl_copy_client_mutex);
	pthread_mutex_lock(&dci_packet_ready_mutex);
	if (!dci_packet_ready)
		pthread_cond_wait(&dci_packet_ready_cond, &dci_packet_ready_mutex);
	memcpy(hidl_dci_buf, buf, len);
	hidl_dci_buf_packet_len = len;
	hidl_dci_pid = pid;
	dci_packet_ready = 0;
	pthread_mutex_unlock(&dci_packet_ready_mutex);

	diag_signal_dci_copy_complete();

	pthread_mutex_lock(&dci_copy_done_mutex);
	if (!dci_copy_done)
		pthread_cond_wait(&dci_copy_done_cond, &dci_copy_done_mutex);
	dci_copy_done = 0;
	pthread_mutex_unlock(&dci_copy_done_mutex);
	pthread_mutex_unlock(&dci_hidl_copy_client_mutex);
}

static void * diag_hidl_thread(void * param)
{
	ashmemAllocator = IAllocator::getService("ashmem");
	int len, status = 0, i;
	unsigned char *data = NULL;
	(void)param;

	if (ashmemAllocator == nullptr)
                return NULL;
	ashmemAllocator->allocate(90000, [&](bool success, const hidl_memory& mem) {
                if (!success) {
                        printf("ashmem allocate failed!!");
                        status = -EINVAL;
                        return;
                }
                mem_s = mem;
                memory = mapMemory(mem);
                if (memory == nullptr) {
                        printf("%s: Could not map HIDL memory to IMemory", __func__);
                        status = -EINVAL;
                        return;
                }
                data = static_cast<unsigned char*>(static_cast<void*>(memory->getPointer()));
                if (data == nullptr) {
                        printf("%s: Could not get pointer to memory", __func__);
                        status = - EINVAL;
                        return;
                }
	}).isOk();
	if (status < 0)
		return NULL;
	 ashmemAllocator->allocate(90000, [&](bool success, const hidl_memory& mem) {
                if (!success) {
                        printf("ashmem allocate failed!!");
                        status = -EINVAL;
                        return;
                }
                mem_s_1 = mem;
        }).isOk();
        if (status < 0)
                return NULL;

	do {
		pthread_mutex_lock(&read_cmd_mutex);
		if (!data_ready)
			pthread_cond_wait(&read_cmd_cond, &read_cmd_mutex);
		if (!data) {
			pthread_mutex_unlock(&read_cmd_mutex);
			continue;
		}

		if (data_ready & PKT_TYPE) {
			memory->update();
			memcpy(data, apps_buf, cmd_len);
			memory->commit();
			it = client_info.find(cmd_pid);
			if (it != client_info.end()) {
				auto ret = it->second->send_data(mem_s, cmd_len);
				if (!ret.isOk())
					ALOGE("diag: unable to send command to client\n");
				else
					ALOGM(DIAG_DBG_MASK_CMD, "diag: %s: cmd pkt of len: %d to system client of pid: %d\n",
						__func__, (cmd_len - 4), cmd_pid);
			}
			pkt_data_ready--;
			if (!pkt_data_ready)
				data_ready = data_ready ^ PKT_TYPE;
		}
		if (data_ready & DCI_PKT_TYPE) {
			memory->update();
			memcpy(data, apps_buf, cmd_len);
			memory->commit();
			it = client_info.find(cmd_pid);
			if (it != client_info.end()) {
				auto ret = it->second->send_data(mem_s, cmd_len);
				if (!ret.isOk())
					ALOGE("diag: unable to send command to dci client\n");
			}
			dci_pkt_data_ready--;
			if (!dci_pkt_data_ready)
				data_ready = data_ready ^ DCI_PKT_TYPE;
		}
		if (data_ready & MSG_MASKS_TYPE) {
			memory->update();
			len = diag_msg_mask_update_hidl_client(data);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send msg mask to client\n");
			}

			msg_mask_data_ready--;
			if (!msg_mask_data_ready)
				data_ready = data_ready ^ MSG_MASKS_TYPE;
		}
		if (data_ready & LOG_MASKS_TYPE) {
			memory->update();
			len = diag_log_mask_update_hidl_client(data);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send log mask to client\n");
			}

			log_mask_data_ready--;
			if (!log_mask_data_ready)
				data_ready = data_ready ^ LOG_MASKS_TYPE;
		}
		if (data_ready & EVENT_MASKS_TYPE) {
			memory->update();
			len = diag_event_mask_update_hidl_client(data);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send event mask to client\n");
			}

			event_mask_data_ready--;
			if (!event_mask_data_ready)
				data_ready = data_ready ^ EVENT_MASKS_TYPE;
		}
		if (data_ready & TIMESTAMP_SWITCH_TYPE) {
			memory->update();
			len = diag_timestamp_switch_update_hidl_client(data);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send timestamp switch to client\n");
			}

			timestamp_switch_data_ready--;
			if (!timestamp_switch_data_ready)
				data_ready = data_ready ^ TIMESTAMP_SWITCH_TYPE;
		}
		if (data_ready & DCI_LOG_MASKS_TYPE)
		{
			memory->update();
			len = diag_dci_mask_update_hidl_client(data, DCI_LOG_MASKS_TYPE);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send dci log mask to client\n");
			}

			dci_log_mask_data_ready--;
			if (!dci_log_mask_data_ready)
				data_ready = data_ready ^ DCI_LOG_MASKS_TYPE;
		}
		if (data_ready & DCI_EVENT_MASKS_TYPE)
                {
			memory->update();
			len = diag_dci_mask_update_hidl_client(data, DCI_EVENT_MASKS_TYPE);
			memory->commit();

			for (it = client_info.begin(); it!= client_info.end(); it++) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send dci event mask to client\n");
			}

			dci_event_mask_data_ready--;
			if (!dci_event_mask_data_ready)
				data_ready = data_ready ^ DCI_EVENT_MASKS_TYPE;
		}
		if (data_ready & DCI_BUFFERING_MODE_STATUS_TYPE)
		{
			for (i = 1; i < NUM_PERIPHERALS; i++) {
				memory->update();
				len = diag_dci_buffering_mode_update_hidl_client(i, data, DCI_BUFFERING_MODE_STATUS_TYPE);
				memory->commit();

				for (it = client_info.begin(); it!= client_info.end(); it++) {
					auto ret = it->second->send_data(mem_s, len);
					if (!ret.isOk())
					ALOGE("diag: unable to send dci buffering mode status to client\n");
				}
			}

			dci_buffering_mode_data_ready--;
			if (!dci_buffering_mode_data_ready)
				data_ready = data_ready ^ DCI_BUFFERING_MODE_STATUS_TYPE;
		}
		if (data_ready & USER_SPACE_DATA_TYPE) {
			diag_signal_ready();
			pthread_mutex_lock(&packet_copy_mutex);
			if (!packet_copy)
				pthread_cond_wait(&packet_copy_cond, &packet_copy_mutex);
			packet_copy = 0;
			len = hidl_buf_packet_len;
			memory->update();
			memcpy(data, hidl_buf, len);
			memory->commit();
			pthread_mutex_unlock(&packet_copy_mutex);

			it = client_info.find(hidl_pid);
			if (it != client_info.end()) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send packet to client\n");
			}
			userspace_data_ready--;
			if (!userspace_data_ready)
				data_ready = data_ready ^ USER_SPACE_DATA_TYPE;
			diag_signal_return();
		} if (data_ready & DATA_TYPE_DCI) {
			diag_signal_dci_ready();
			pthread_mutex_lock(&dci_packet_copy_mutex);
			if (!dci_packet_copy)
				pthread_cond_wait(&dci_packet_copy_cond, &dci_packet_copy_mutex);
			dci_packet_copy = 0;
			len = hidl_dci_buf_packet_len;
			memory->update();
			memcpy(data, hidl_dci_buf, len);
			memory->commit();
			pthread_mutex_unlock(&dci_packet_copy_mutex);

			it = client_info.find(hidl_dci_pid);
			if (it != client_info.end()) {
				auto ret = it->second->send_data(mem_s, len);
				if (!ret.isOk())
					ALOGE("diag: unable to send packet to dci client\n");
			}
			dci_data_ready--;
			if (!dci_data_ready)
				data_ready = data_ready ^ DATA_TYPE_DCI;
			diag_signal_dci_return();
		}
		pthread_mutex_unlock(&read_cmd_mutex);
	}
	while (1);
}

int create_diag_hidl_thread()
{
	pthread_mutex_init(&read_cmd_mutex, NULL);
	pthread_cond_init(&read_cmd_cond, NULL);
	pthread_mutex_init(&packet_copy_mutex, NULL);
	pthread_mutex_init(&packet_ready_mutex, NULL);
	pthread_mutex_init(&copy_done_mutex, NULL);
	pthread_cond_init(&packet_copy_cond, NULL);
	pthread_cond_init(&packet_ready_cond, NULL);
	pthread_cond_init(&copy_done_cond, NULL);
	pthread_mutex_init(&dci_packet_copy_mutex, NULL);
	pthread_mutex_init(&dci_packet_ready_mutex, NULL);
	pthread_mutex_init(&dci_copy_done_mutex, NULL);
	pthread_mutex_init(&hidl_copy_client_mutex, NULL);
	pthread_cond_init(&dci_packet_copy_cond, NULL);
	pthread_cond_init(&dci_packet_ready_cond, NULL);
	pthread_cond_init(&dci_copy_done_cond, NULL);

	pthread_create(&diag_hidl_thread_hdl, NULL, diag_hidl_thread, NULL);
	return 0;
}
