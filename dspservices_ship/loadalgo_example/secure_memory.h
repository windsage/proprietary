/**=============================================================================

@file
   secure_memory.h

@brief
   Header file for secure memory management.

Copyright (c) 2019, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

#ifndef SECURE_MEMORY_H
#define SECURE_MEMORY_H

typedef int ion_user_handle_t;

#include "AEEStdDef.h"
#include <stdio.h>
#include <linux/dma-buf.h>
#include <linux/ion.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vmmem_wrapper.h>

#ifdef LOADALGO_QMAA_ENABLED
/*
 * In QMAA mode, kernel dependencies are not allowed. So,
 * redefine ION macros from locally to maintain compilation.
 * These hardcoded values are from the 5.4 kernel, so this
 * module will not be functional on QMAA builds compiled
 * with an older kernel.
 */
#define ION_HEAP(bit)                   bit
#define ION_BIT(nr)                     (1U << (nr))
#define ION_SYSTEM_HEAP_ID              ION_BIT(25)
#define ION_SECURE_CARVEOUT_HEAP_ID     ION_BIT(15)
#define ION_FLAG_SECURE                 ION_BIT(31)
#define ION_QSECOM_HEAP_ID              ION_BIT(7)
#define ION_FLAG_CP_CDSP                ION_BIT(29)
#define ION_DISPLAY_HEAP_ID             ION_BIT(3)

#else
#include <linux/msm_ion.h>
#endif //LOADALGO_QMAA_ENABLED

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mem_handle.h"

/* @brief: Error codes retruned by secure memory module */
enum securemem_error_codes {
    SECUREMEMORY_INVALIDHANDLE = 600,
    SECUREMEMORY_INVALIDSIZE,
    SECUREMEMORY_OPENFAILED,
    SECUREMEMORY_ALLOCFAILED,
    SECUREMEMORY_DMABUF_FAIL,
    SECUREMEMORY_NORPCMEMORY,
    SECUREMEMORY_INVALIDHEAP
};

enum mem_alloc_heaps {
    NON_SECURE_HEAP = 0,
    SECURE_HEAP,
    SYSTEM_HEAP,
    DISPLAY_HEAP
};

#define MAX_HEAP_NAME_LEN 50

typedef struct fastrpc_heap_vmmem {
	VmMem *vmmem_obj;
	VmHandle *vm_handle_arr;
	uint32_t *vm_perm_arr;
	int num_vm_handles;
} fastrpc_heap_vmmem;

struct fastrpc_heap {
	BufferAllocator* allocator;
	const char name[MAX_HEAP_NAME_LEN];
	fastrpc_heap_vmmem vmmem;
};

/* @brief: Free the allocated ion memory.
 * @param pIon: Memory handle that stores address, fd, size etc.
 * @retval : 0 in case of success.
 */
int secure_mem_free(mem_handle *pIon);

/* @brief: Allocate ion memory.
 * @param pIon: Memory handle that stores address, fd, size etc.
 * @param nBytes: Size to allocate.
 * @param nByteAlignment: Alignment.
 * @param heap_type: Allocate from secure, non-secure heap or system heap
 *                   Use one of the pre-defined heap types from mem_alloc_heaps.
 * @retval : 0 in case of success.
 */
int secure_mem_alloc(mem_handle *pIon, uint32_t nBytes, uint32_t nByteAlignment, enum mem_alloc_heaps heap_type);

/* @brief: Set up the memory allocators
 * @retval : 0 in case of success.
 */
int secure_mem_init();

/* @brief: Free the memory allocators
 * @retval : None
 */
void secure_mem_deinit();

#endif
