/**=============================================================================

@file
   secure_memory.c

@brief
   Secure memory management implementation.

Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

//==============================================================================
// Include Files
//==============================================================================
#include "secure_memory.h"
#include <utils/Log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SecureMemory"

#ifndef ION_SECURE_CARVEOUT_HEAP_ID
#define ION_SECURE_CARVEOUT_HEAP_ID 0
#endif

#include <stdlib.h>
#include <string.h>
struct fastrpc_heaps {
	struct fastrpc_heap system_heap;
	struct fastrpc_heap secure_carveout_heap;
	struct fastrpc_heap secure_cma_heap;
	struct fastrpc_heap qsecom_heap;
	struct fastrpc_heap display_heap;
};

struct fastrpc_heaps heaps;

static int setup_vmmem_obj(struct fastrpc_heap *heap, const char* vm_names[], int num_vm_handles) {
	int nErr = 0;
	fastrpc_heap_vmmem *vmmem = NULL;
	VmMem *vmmem_obj = NULL;
	VmHandle *vm_handles = NULL;

	if (heap == NULL) {
		nErr = SECUREMEMORY_INVALIDHEAP;
		ALOGE("failed to create VmMem object, error %d", nErr);
		goto bail;
	}
	vmmem = &heap->vmmem;
	int vm_perm_arr_size = (sizeof(uint32_t) * num_vm_handles);
	uint32_t vm_perms[] = {VMMEM_READ | VMMEM_WRITE | VMMEM_EXEC};

	vmmem_obj = CreateVmMem();
	if (vmmem_obj == NULL) {
		nErr = SECUREMEMORY_NORPCMEMORY;
		ALOGE("failed to create VmMem object");
		goto bail;
	}

	if (vmmem == NULL) {
		nErr = SECUREMEMORY_NORPCMEMORY;
		ALOGE("Error: vmmem is NULL");
		goto bail;
	}
	vmmem->vmmem_obj = vmmem_obj;

	vm_handles = (VmHandle *)malloc(sizeof(VmHandle) * num_vm_handles);
	if (vm_handles == NULL) {
		nErr = SECUREMEMORY_NORPCMEMORY;
		ALOGE("failed to allocate VM handle array for allocator %p", heap->allocator);
		goto bail;
	}
	vmmem->vm_handle_arr = vm_handles;

	vmmem->vm_perm_arr = (uint32_t *)malloc(vm_perm_arr_size);
	if (vmmem->vm_perm_arr == NULL) {
		nErr = SECUREMEMORY_NORPCMEMORY;
		ALOGE("failed to allocate VM perm array for allocator %p", heap->allocator);
		goto bail;
	}
	memcpy(vmmem->vm_perm_arr, vm_perms, vm_perm_arr_size);

	for (int i = 0; i < num_vm_handles; i++) {
		vm_handles[i] = FindVmByName(vmmem_obj, vm_names[i]);
		if (vm_handles[i] <= 0) {
			nErr = SECUREMEMORY_NORPCMEMORY;
			ALOGE("failed to find VM[%d] %s for allocator %p, ret %d, err %d",
				i, vm_names[i], heap->allocator, vm_handles[i], nErr);
			goto bail;
		} else {
			ALOGD("Found VM[%d] %s for allocator %p, ret %d",
				i, vm_names[i], heap->allocator, vm_handles[i]);
		}
	}
	vmmem->num_vm_handles = num_vm_handles;
	ALOGD("set up VMmem object for allocator %p (num handles %d)",
			heap->allocator, num_vm_handles);
bail:
	if (nErr) {
		if (vmmem && vmmem->vm_perm_arr) {
			free(vmmem->vm_perm_arr);
			vmmem->vm_perm_arr = NULL;
		}
		if (vm_handles) {
			free(vm_handles);
			vmmem->vm_handle_arr = NULL;
		}
		if (vmmem_obj) {
			FreeVmMem(vmmem_obj);
			vmmem->vmmem_obj = NULL;
		}
	}
	return nErr;
}

static int setup_dmabuf_heap_allocators(struct fastrpc_heap *heap,
	const char* heap_name, const char* ion_heap_name, unsigned int legacy_ion_heap_mask,
	unsigned int ion_heap_flags, unsigned legacy_ion_heap_flags) {
	int nErr = 0;
	BufferAllocator* heap_allocator = NULL;

	heap_allocator = CreateDmabufHeapBufferAllocator();
	if (!heap_allocator) {
		nErr = SECUREMEMORY_DMABUF_FAIL;
		ALOGE("Error: %s: failed to create DMA buf heap allocator for %s (%s), mask 0x%x, flags 0x%x, legacy flags 0x%x\n",
			__func__, heap_name, ion_heap_name, legacy_ion_heap_mask, ion_heap_flags, legacy_ion_heap_flags);
		goto bail;
	}
	nErr = MapDmabufHeapNameToIonHeap(heap_allocator,
		heap_name, ion_heap_name, ion_heap_flags, legacy_ion_heap_mask, legacy_ion_heap_flags);
	if (nErr) {
		nErr = SECUREMEMORY_DMABUF_FAIL;
		ALOGE("Error %d: %s: failed to map DMA buf heap to ION heap for %s (%s), mask 0x%x, flags 0x%x, legacy flags 0x%x\n",
			nErr, __func__, heap_name, ion_heap_name, legacy_ion_heap_mask, ion_heap_flags, legacy_ion_heap_flags);
		goto bail;
	}
	heap->allocator = heap_allocator;
	snprintf(heap->name, MAX_HEAP_NAME_LEN, "%s", heap_name);
bail:
	return nErr;
}

int secure_mem_init() {
	int nErr = 0;
	unsigned int secure_flags = ION_FLAG_CP_CDSP | ION_FLAG_SECURE;
	const char *vm_names[] = {"qcom,cp_cdsp"};

	nErr = setup_dmabuf_heap_allocators(&heaps.system_heap,
			"qcom,system", "system", ION_HEAP(ION_SYSTEM_HEAP_ID),
			ION_FLAG_CACHED, ION_FLAG_CACHED);
	if (nErr) {
		goto bail;
	}
	nErr = setup_dmabuf_heap_allocators(&heaps.secure_carveout_heap,
			"qcom,secure-cdsp", "secure_carveout", ION_HEAP(ION_SECURE_CARVEOUT_HEAP_ID),
			secure_flags, secure_flags);
	if (nErr) {
		goto bail;
	}
	nErr = setup_dmabuf_heap_allocators(&heaps.secure_cma_heap,
			"qcom,cma-secure-cdsp", "", 0, 0, 0);
	if (!nErr) {
		nErr = setup_vmmem_obj(&heaps.secure_cma_heap, vm_names, 1);
		if (nErr) {
			goto bail;
		}
	} else {
		ALOGD("Warning: %s: set cma-secure-cdsp allocator failed", __func__);
		nErr = 0;
	}
	nErr = setup_dmabuf_heap_allocators(&heaps.qsecom_heap,
			"qcom,qseecom", "qsecom", ION_HEAP(ION_QSECOM_HEAP_ID),
			ION_FLAG_CACHED, ION_FLAG_CACHED);
	if (nErr) {
		goto bail;
	}

	nErr = setup_dmabuf_heap_allocators(&heaps.display_heap,
			"qcom,display", "display", ION_HEAP(ION_DISPLAY_HEAP_ID),
			ION_FLAG_CACHED, ION_FLAG_CACHED);
	if (nErr) {
		goto bail;
	}
bail:
	return nErr;
}

void secure_mem_deinit() {
	if (heaps.system_heap.allocator) {
		FreeDmabufHeapBufferAllocator(heaps.system_heap.allocator);
	}
	if (heaps.qsecom_heap.allocator) {
		FreeDmabufHeapBufferAllocator(heaps.qsecom_heap.allocator);
	}
	if (heaps.secure_carveout_heap.allocator) {
		FreeDmabufHeapBufferAllocator(heaps.secure_carveout_heap.allocator);
	}
	if (heaps.display_heap.allocator) {
		FreeDmabufHeapBufferAllocator(heaps.display_heap.allocator);
	}
	if (heaps.secure_cma_heap.allocator) {
		fastrpc_heap_vmmem *vmmem = &heaps.secure_cma_heap.vmmem;
		FreeDmabufHeapBufferAllocator(heaps.secure_cma_heap.allocator);
		if (vmmem->vm_perm_arr) {
			free(vmmem->vm_perm_arr);
			vmmem->vm_perm_arr = NULL;
		}
		if (vmmem->vm_handle_arr) {
			free(vmmem->vm_handle_arr);
			vmmem->vm_handle_arr = NULL;
		}
		if (vmmem->vmmem_obj) {
			FreeVmMem(vmmem->vmmem_obj);
			vmmem->vmmem_obj = NULL;
		}
	}
}

int secure_mem_free(mem_handle *pIon)
{
   int32_t ret = 0;

   if (pIon == NULL)
   {
      return SECUREMEMORY_INVALIDHANDLE;
   }

   if (pIon->addr != 0)
   {
      ret = munmap((void*)(uint64_t)pIon->addr, pIon->size);
      if (ret)
      {
         ALOGD("Error: Unmapping ION Buffer failed with ret = %d", ret);
      }
      pIon->addr = 0;
   }

   if (pIon->fd)
   {
      close(pIon->fd);
   }
   pIon->fd = -1;
   pIon->size = 0;

   return 0;
}

int secure_mem_alloc(mem_handle *pIon, uint32_t nBytes, uint32_t nByteAlignment, enum mem_alloc_heaps heap_type)
{
   int map_fd = 0;
   struct fastrpc_heap* heap = NULL;
   int ret = 0;

   if (pIon == NULL)
   {
      return SECUREMEMORY_INVALIDHANDLE;
   }
   if (nBytes == 0)
   {
      return SECUREMEMORY_INVALIDSIZE;
   }

   pIon->fd = -1;
   pIon->size = 0;
   pIon->addr = 0;

   if (heap_type == SECURE_HEAP || nByteAlignment > 0)
   {
      uint32_t alignMin1 = nByteAlignment - 1;
      pIon->size = (nBytes + alignMin1) & (~alignMin1);
   }
   else
   {
      pIon->size = nBytes;
   }

   switch (heap_type) {
   case NON_SECURE_HEAP:
      ALOGD("Using non-secure memory from qsecom heap, cached");
      //This is a heap that is contiguous and non-dma.(For TA mapping purpose)
      heap = &heaps.qsecom_heap;
      break;
   case SECURE_HEAP:
      ALOGD("Using secure memory from carveout heap");
      heap = &heaps.secure_carveout_heap;
      break;
   case SYSTEM_HEAP:
      ALOGD("Using non-secure memory from system heap");
      heap = &heaps.system_heap;
      break;
   case DISPLAY_HEAP:
      ALOGD("Using non-secure memory from display heap");
      heap = &heaps.display_heap;
      break;
   default:
      ALOGD("Using default non-secure memory from system heap");
      heap = &heaps.system_heap;
   }

   map_fd = DmabufHeapAlloc(heap->allocator, heap->name, pIon->size, 0, 0);
   if (map_fd < 0)
   {
      ALOGD("Error: %s: DmabufHeapAlloc returned failure %d", __func__, map_fd);
      if (heap_type == SECURE_HEAP) {
           ALOGD("Warning: %s: DmabufHeapAlloc returned failure %d from carveout, retry allocation from cma", __func__, map_fd);
           heap = &heaps.secure_cma_heap;
           map_fd = DmabufHeapAlloc(heap->allocator, heap->name, pIon->size, 0, 0);
           if (map_fd < 0) {
                ALOGD("Error: %s: DmabufHeapAlloc returned failure %d", __func__, map_fd);
                goto alloc_fail;
           }
           fastrpc_heap_vmmem *vmmem = &heap->vmmem;
           if (vmmem && vmmem->vmmem_obj) {
               ret = LendDmabuf(vmmem->vmmem_obj, map_fd, vmmem->vm_handle_arr,
                       vmmem->vm_perm_arr, vmmem->num_vm_handles);
               if (ret) {
                   ALOGE("Error %d: %s: LendDmabuf failed for map_fd %d\n", ret, __func__, map_fd);
                   goto alloc_fail;
               }
           }
      } else {
           goto alloc_fail;
      }
   }

   if (map_fd)
   {
      pIon->fd = map_fd;
   }
   else
   {
      ALOGD("Error: map_fd is NULL");
      goto alloc_fail;
   }

   if (heap_type == SECURE_HEAP) {
      pIon->addr = 0;
   } else {
      pIon->addr = (uint64_t)mmap(NULL, pIon->size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
      if (pIon->addr == 0 || pIon->addr == (uint32_t)(-1))
      {
         ALOGD("Error: mmap return NULL");
         goto ioctl_fail;
      }
   }

   return 0;

ioctl_fail:
   if (map_fd >= 0)
   {
      close(map_fd);
   }

alloc_fail:
   return SECUREMEMORY_ALLOCFAILED;
}

