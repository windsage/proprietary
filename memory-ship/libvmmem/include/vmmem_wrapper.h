/*
 * Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VMMEM_WRAPPER_H_
#define _VMMEM_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VmMem VmMem;
typedef int VmHandle;

#define VMMEM_READ (1U << 0)
#define VMMEM_WRITE (1U << 1)
#define VMMEM_EXEC (1U << 2)

VmMem *CreateVmMem(void);
void FreeVmMem(VmMem *instance);
int IsExclusiveOwnerDmabuf(int fd, bool *is_exclusive_owner);
VmHandle FindVmByName(VmMem *instance, char *cstr);
int LendDmabufHandle(VmMem *instance, int dma_buf_fd, VmHandle *handles,
		uint32_t *perms, int nr, int64_t *memparcel_hdl);
int LendDmabuf(VmMem *instance, int dma_buf_fd, VmHandle *handles,
		uint32_t *perms, int nr);
int ShareDmabufHandle(VmMem *instance, int dma_buf_fd, VmHandle *handles,
	uint32_t *perms, int nr, int64_t *memparcel_hdl);
int ShareDmabuf(VmMem *instance, int dma_buf_fd, VmHandle *handles,
	uint32_t *perms, int nr);
int RetrieveDmabuf(VmMem *instance, VmHandle owner, VmHandle *handles,
	uint32_t *perms, int nr, int64_t memparcel_hdl);
int ReclaimDmabuf(VmMem *instance, int dma_buf_fd, int64_t memparcel_hdl);
int RemoteAllocDmabuf(VmMem *instance, uint64_t size, VmHandle *handles,
	uint32_t *perms, int nr,
	char *c_src_dma_heap_name, char *c_dst_dma_heap_name);
int MemorySizeHint(VmMem *instance, int64_t size, const char *c_name);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* _VMMEM_WRAPPER_H_ */
