/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag HIDL Implementation

GENERAL DESCRIPTION

Diag HIDL service implementation for open, ioctl, write calls.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#pragma once

#include <vendor/qti/diaghal/1.0/Idiag.h>
#include <vendor/qti/diaghal/1.0/Idiagcallback.h>
#include <vendor/qti/diaghal/1.0/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <hwbinder/IPCThreadState.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include "diag_hidl_util.h"
#include "watch.h"
#include "dm.h"
#include "list.h"
#include "util.h"

namespace vendor {
namespace qti {
namespace diaghal {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
struct diagService : public Idiag {
    Return<int32_t> open(const sp<Idiagcallback>& callback) override;
    Return<int32_t> ioctl(uint32_t cmd, const hidl_memory& buf, uint32_t len) override;
    Return<int32_t> write(const hidl_memory& buf, uint32_t len) override;
    Return<int32_t> close() override;

    /* The following are member functions not inherited from Idiag
     * These are to be used in diag-router and are only exposed
     * locally to the service implementation.
     */

    /* Returns the single existing Instance of diagService */
    static diagService* getInstance(void);

    /* Finds and returns the process DM */
    struct diag_client *dmGet(pid_t pid);

    /* Allocates a DM for a process and adds it to diagService list of DMs */
    struct diag_client *dmAdd(pid_t pid);

    /* Removes the process DM from diagService list of DMs and frees it */
    void dmRemove(pid_t pid);

    /* Initialize fds, mutexes and buffer variables
     * This should only be called once during init
     */
    int serviceInit(void);

    /* Reads and processes data from synchronization watch */
    static int readFromWatch(int fd, void *data);

private:
	static diagService *instance;

	/* Make default constructor private */
	diagService(void);

	pthread_mutex_t lock;
	void *buf;
	int fds[2];
	struct watch_flow *flow;
	struct list_head dms;
};

}
}
}
}
}  // namespace vendor::qti::diaghal::V1_0::implementation
