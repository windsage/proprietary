/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service registration implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <hidl/LegacySupport.h>
#include "diag_hidl.h"
#include "diag_hidl_service.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#define READ_BUF_SIZE 100000
using vendor::qti::diaghal::V1_0::Idiag;
using vendor::qti::diaghal::V1_0::implementation::diagService;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
pthread_t read_thread_hdl;
pthread_t sock_read_thread_hdl;
unsigned char read_buf[READ_BUF_SIZE];
int hidl_sock_fd = -1;

static void *diag_sock_read_thread(void * param)
{
	int bytes_read;
	(void)param;

	do {
		bytes_read = read(hidl_sock_fd, read_buf, READ_BUF_SIZE);
	} while(1);
}

static void *hidlservicethread(void * param)
{
	struct sockaddr_un addr;
	int ret;

	/* Keep Around until all commands are moved to socketpair fds*/
	hidl_sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (hidl_sock_fd < 0) {
		ALOGE(" diag:In %s Failed to create socket\n", __func__);
		return 0;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, "\0diag", sizeof(addr.sun_path)-1);
	ret = connect(hidl_sock_fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) {
		ALOGE("diag: In %s failed to connect to diag socket\n", __func__);
		close(hidl_sock_fd);
		return 0;
	}
	ALOGE("diag: In %s successfully connected to socket %d\n", __func__, hidl_sock_fd);
	pthread_create(&sock_read_thread_hdl, NULL, diag_sock_read_thread, NULL);

	auto diag = diagService::getInstance();
	if (diag == nullptr)
		return param;

	/* Move this to private function called connect*/
	ret = diag->serviceInit();
	if (ret)
		return param;

	/* initialize strong pointer to add reference to diagService object */
	android::sp<Idiag> service = diag;
	configureRpcThreadpool(1, true);

	ret = service->registerAsService();
	if (ret) {
		ALOGE("diag: %s: register as service failed\n", __func__);
		return param;
	}
	ALOGE("diag: %s: register as service successful\n", __func__);

	/* joinRpcThreadpool should never return */
	joinRpcThreadpool();
	ALOGE("diag: %s: join rpc thread pool\n", __func__);

        return 0;
}

int register_diag_hidl_service()
{
	pthread_create(&read_thread_hdl, NULL, hidlservicethread, NULL);

	return 0;
}
