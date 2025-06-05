/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef _VM_OSAL_MEM_H_
#define _VM_OSAL_MEM_H_

#ifdef _CUSTOMIZED_PLATFORM
#else
#include "vmmem_wrapper.h"
#endif //_CUSTOMIZED_PLATFORM

#if defined (__cplusplus)
extern "C" {
#endif /* __cplusplus */

#ifdef _CUSTOMIZED_PLATFORM
typedef void* vm_osal_vmMem;
typedef void* vm_osal_vmHandle;
#else
typedef VmMem vm_osal_vmMem;
typedef VmHandle vm_osal_vmHandle;
#endif //_CUSTOMIZED_PLATFORM

vm_osal_vmMem *vm_osal_createVmMem(void);
void vm_osal_freeVmMem(vm_osal_vmMem *instance);
vm_osal_vmHandle vm_osal_findVmByName(vm_osal_vmMem *instance, char *cstr);
int vm_osal_lendDmabufHandle(vm_osal_vmMem *instance, int dmaBufFd,
                             vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                             int64_t *memparcelHandle);
int vm_osal_shareDmabufHandle(vm_osal_vmMem *instance, int dmaBufFd,
                             vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                             int64_t *memparcelHandle);
int vm_osal_retrieveDmabuf(vm_osal_vmMem *instance, vm_osal_vmHandle owner,
                           vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                           int64_t memparcelHandle);
int vm_osal_reclaimDmabuf(vm_osal_vmMem *instance, int dmaBufFd,
                          int64_t memparcelHandle);

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif //_VM_OSAL_MEM_H_

