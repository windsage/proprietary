/***********************************************************************
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/
#define _GNU_SOURCE

#include "ShareMemory.h"
#include "Confinement.h"
#include "IConfinement.h"
#include "MSMem.h"
#include "MinkTransportUtils.h"
#include "VmOsalMem.h"

#ifndef OFFTARGET
#include <linux/mem-buf.h>
#include <linux/qti-smmu-proxy.h>
#endif

#define OK_OR_EXIT(func)          \
  do {                            \
    if (Object_isERROR(func)) {   \
      ret = Object_ERROR;         \
      goto exit;                  \
    }                             \
  } while (0)

static inline int32_t _createVmMem(vm_osal_vmMem **vmMem)
{
  if (!vmMem) {
    LOG_ERR("Invalid vmMem.\n");
    return Object_ERROR;
  }

  *vmMem = vm_osal_createVmMem();
  if (NULL == *vmMem) {
    LOG_ERR("Error creating VmMem.\n");
    return Object_ERROR;
  }

  return Object_OK;
}

static inline int32_t _getVmHandle(vm_osal_vmMem *vmMem,
                                   vm_osal_vmHandle *vmHandle,
                                   char *vmName)
{
  if (!vmMem || !vmHandle || !vmName) {
    LOG_ERR("Invalid vmMem, vmHandle and vmName.\n");
    return Object_ERROR;
  }

  *vmHandle = vm_osal_findVmByName(vmMem, vmName);
  if (*vmHandle < 0) {
    LOG_ERR("Error find VM by \"%s\".\n", vmName);
    return Object_ERROR;
  }

  return Object_OK;
}

static inline void _cleanupVmMem(vm_osal_vmMem *vmMem)
{
  if (vmMem) {
    vm_osal_freeVmMem(vmMem);
  }
}

// TODO: extend following operations to support more than 1 borrowers

static int32_t ShareMemory_PrepareInfoForMemBuf(
  uint64_t specialRules, char *destVMName, vm_osal_vmMem *vmMem,
  vm_osal_vmHandle *handleArr, uint32_t *permArr, int32_t nr, bool flip,
  vm_osal_vmHandle *ownerHandle)
{
  int32_t ret = Object_OK;

  if ((!(ITAccessPermissions_removeSelfAccess & specialRules) && nr != 2) ||
      ((ITAccessPermissions_removeSelfAccess & specialRules) && nr != 1)) {
    LOG_ERR("INCONSISTENT entry for input information.\n");
    ret = Object_ERROR;
    goto exit;
  }

  if (NULL == destVMName) {
    LOG_ERR("Invalid destVM name.\n");
    ret = Object_ERROR;
    goto exit;
  }

  if (!(ITAccessPermissions_removeSelfAccess & specialRules)) {
    permArr[0] = VMMEM_READ | VMMEM_WRITE;
    permArr[1] = VMMEM_READ | VMMEM_WRITE;
  } else {
    permArr[0] = VMMEM_READ | VMMEM_WRITE;
  }

  if (ownerHandle == NULL) {
    OK_OR_EXIT(_getVmHandle(vmMem, &handleArr[0], destVMName));
  } else {
    // Ownership wont change even after MEM_SHARE or MEM_LEND
    OK_OR_EXIT(_getVmHandle(vmMem, ownerHandle, destVMName));
    OK_OR_EXIT(_getVmHandle(vmMem, &handleArr[0], "qcom,self"));
  }

  if (!(ITAccessPermissions_removeSelfAccess & specialRules)) {
    OK_OR_EXIT(_getVmHandle(vmMem, &handleArr[1], "qcom,self"));
  }

  if (nr > 1 && flip) {
    vm_osal_vmHandle tmp = handleArr[0];
    handleArr[0] = handleArr[1];
    handleArr[1] = tmp;
  }

exit:

  return ret;
}

#if !defined(OFFTARGET) && defined(MEM_BUF_IOC_GET_MEMPARCEL_HDL)
static int32_t ShareMemory_QueryExistingMemparcelHandle(int32_t dmaBufFd, int64_t *outMPHandle)
{
  int32_t ret = Object_ERROR;
  int32_t memBufFd = -1;
  struct mem_buf_get_memparcel_hdl_ioctl_arg getMemParcelHdlArg = {};

  // If the DMA-buffer has been already shared,
  // then just call into mem-buf driver to get memparcel handle
  getMemParcelHdlArg.dma_buf_fd = dmaBufFd;

  memBufFd = open("/dev/membuf", O_RDONLY | O_CLOEXEC);
  if (-1 == memBufFd) {
    LOG_ERR("Unable to open /dev/membuf, errno=%d!\n", errno);
    ret = Object_ERROR;
    goto exit;
  }

  ret = ioctl(memBufFd, MEM_BUF_IOC_GET_MEMPARCEL_HDL, &getMemParcelHdlArg);
  if (ret) {
    LOG_ERR("ioctl(MEM_BUF_IOC_GET_MEMPARCEL_HDL) failed, errno=%d!\n", errno);
    ret = Object_ERROR;
    goto exit;
  }

  *outMPHandle = getMemParcelHdlArg.memparcel_hdl;
  LOG_MSG("ioctl(MEM_BUF_IOC_GET_MEMPARCEL_HDL) passed.\n");
  ret = Object_OK;

exit:
  if (-1 != memBufFd) {
    vm_osal_mem_close(memBufFd);
  }

  return ret;
}
#else
static int32_t ShareMemory_QueryExistingMemparcelHandle(int32_t dmaBufFd, int64_t *outMPHandle)
{
  return Object_ERROR;
}
#endif

int32_t ShareMemory_GetMemParcelHandle(int32_t dmaBufFd, Object conf,
                                       char *destVMName, int64_t *outMPHandle)
{
  int32_t ret = Object_OK;
  int32_t nr = -1;
  uint32_t permArr[2] = {0};
  uint64_t specialRules = 0;
  int32_t retGetSpecialRules = IConfinement_getSpecialRules(conf, &specialRules);
  vm_osal_vmMem *vmMem = NULL;
  vm_osal_vmHandle handleArr[2] = {0};

  if (!Object_isOK(retGetSpecialRules)) {
    LOG_ERR("Got non-confinement object.\n");
    ret = Object_ERROR;
    goto exit;
  }

  if ((ITAccessPermissions_smmuProxyControlled & specialRules) ||
      (ITAccessPermissions_mixedControlled & specialRules)) {
    return ShareMemory_QueryExistingMemparcelHandle(dmaBufFd, outMPHandle);
  }

  nr = !(ITAccessPermissions_removeSelfAccess & specialRules) ? 2 : 1;

  OK_OR_EXIT(_createVmMem(&vmMem));

  if (!Object_isOK(ShareMemory_PrepareInfoForMemBuf(
        specialRules, destVMName, vmMem, handleArr, permArr, nr, true, NULL))) {
    LOG_ERR("Error preparing info for mem-buf.\n");
    ret = Object_ERROR;
    goto exit;
  }

  if (!(ITAccessPermissions_removeSelfAccess & specialRules)) {
    ret =
      vm_osal_shareDmabufHandle(vmMem, dmaBufFd, handleArr, permArr, nr,
                                outMPHandle);
  } else {
    ret =
      vm_osal_lendDmabufHandle(vmMem, dmaBufFd, handleArr, permArr, nr,
                               outMPHandle);
  }

  if (ret < 0) {
    LOG_ERR("%s failed with ret=%d.\n",
         !(ITAccessPermissions_removeSelfAccess & specialRules)
           ? "ShareDmabufHandle"
           : "LendDmabufHandle",
         ret);
    ret = Object_ERROR;
    goto exit;
  } else {
    LOG_MSG("%s passed with ret=%d.\n",
         !(ITAccessPermissions_removeSelfAccess & specialRules)
           ? "ShareDmabufHandle"
           : "LendDmabufHandle",
         ret);
    ret = Object_OK;
  }

exit:
  _cleanupVmMem(vmMem);

  return ret;
}

#if !defined(OFFTARGET) && defined(QTI_SMMU_PROXY_GET_DMA_BUF)
static int32_t ShareMemory_SMMUProxyGetDMA(int64_t memparcelHandle, int32_t *outFd)
{
  int32_t ret = Object_ERROR;
  int32_t smmuFD = -1;
  struct smmu_proxy_get_dma_buf_ctl getDMABufCtl;

  // If mem-buf retrieval is failed,
  // lets try to get DMA buffer via SMMU Proxy driver.
  getDMABufCtl.memparcel_hdl = memparcelHandle;

  smmuFD = open("/dev/qti-smmu-proxy", O_RDWR);
  if (-1 == smmuFD) {
    LOG_ERR("Unable to open /dev/qti-smmu-proxy, errno=%d!\n", errno);
    ret = Object_ERROR;
    goto exit;
  }

  ret = ioctl(smmuFD, QTI_SMMU_PROXY_GET_DMA_BUF, &getDMABufCtl);
  if (ret) {
    LOG_ERR("ioctl(QTI_SMMU_PROXY_GET_DMA_BUF) failed, errno=%d!\n", errno);
    ret = Object_ERROR;
    goto exit;
  }

  *outFd = getDMABufCtl.dma_buf_fd;
  LOG_MSG("ioctl(QTI_SMMU_PROXY_GET_DMA_BUF) passed.\n");
  ret = Object_OK;

exit:
  if (-1 != smmuFD) {
    vm_osal_mem_close(smmuFD);
  }

  return ret;
}
#else
static int32_t ShareMemory_SMMUProxyGetDMA(int64_t memparcelHandle, int32_t *outFd)
{
  return Object_ERROR;
}
#endif

/* In CSF 2.5.1, AVM is lending a DMA buffer to both QTVM and OEMVM.
 *  (1) When QTVM acts as borrower of the DMA buffer:
 *    i. MEM_ACCEPT is executed by SMMU Proxy.
 *   ii. MinkTransport will directly query the DMA buf FD and marshal it.
 *  (2) When OEMVM acts as borrower of the DMA buffer:
 *    i. MEM_ACCEPT is executed by MEM-BUF.
 */
static int32_t ShareMemory_MixedGetDMA(int64_t memparcelHandle, char *destVMName,
                                       int32_t *outFd)
{
  int32_t ret = Object_OK, idx = 0;
  uint32_t permArr[2] = {0};
  vm_osal_vmMem *vmMem = NULL;
  vm_osal_vmHandle ownerHandle = 0;
  vm_osal_vmHandle handleArr[2] = {0};
  char *vmCandidates[2] = {"qcom,trusted_vm", "qcom,oemvm"};

  OK_OR_EXIT(_createVmMem(&vmMem));

  for (idx = 0; idx < 2; idx++) {
    OK_OR_EXIT(_getVmHandle(vmMem, &handleArr[idx], vmCandidates[idx]));
    permArr[idx] = VMMEM_READ | VMMEM_WRITE;
  }

  // Ownership wont change even after MEM_SHARE or MEM_LEND
  OK_OR_EXIT(_getVmHandle(vmMem, &ownerHandle, destVMName));

  // In OEMVM, MEM_ACCEPT is executed by MEM-BUF.
  ret = vm_osal_retrieveDmabuf(vmMem, ownerHandle, handleArr, permArr, 2,
                               memparcelHandle);
  if (ret < 0) {
    LOG_ERR("RetrieveDmabuf failed with ret=%d.\n", ret);
    ret = Object_ERROR;
    goto exit;
  } else {
    *outFd = ret;
    ret = Object_OK;
  }

exit:
  _cleanupVmMem(vmMem);

  return ret;
}

static int32_t ShareMemory_GetRemoteFileDescriptor(int64_t memparcelHandle,
                                                   char *destVMName,
                                                   uint64_t specialRules,
                                                   int32_t *outFd)
{
  int32_t ret = Object_OK;
  uint32_t permArr[2] = {0};
  int32_t nr = !(ITAccessPermissions_removeSelfAccess & specialRules) ? 2 : 1;
  vm_osal_vmMem *vmMem = NULL;
  vm_osal_vmHandle handleArr[2] = {0};
  vm_osal_vmHandle *ownerHandle = NULL;

  if (ITAccessPermissions_mixedControlled & specialRules) {
    return ShareMemory_MixedGetDMA(memparcelHandle, destVMName, outFd);
  }

  if (ITAccessPermissions_smmuProxyControlled & specialRules) {
    return ShareMemory_SMMUProxyGetDMA(memparcelHandle, outFd);
  }

  OK_OR_EXIT(_createVmMem(&vmMem));

  if (ITAccessPermissions_removeSelfAccess & specialRules) {
    ownerHandle = handleArr + 1;
  }

  if (!Object_isOK(ShareMemory_PrepareInfoForMemBuf(specialRules, destVMName,
                                                    vmMem, handleArr, permArr,
                                                    nr, false, ownerHandle))) {
    LOG_ERR("Error preparing info for mem-buf.\n");
    ret = Object_ERROR;
    goto exit;
  }

  if (!(ITAccessPermissions_removeSelfAccess & specialRules)) {
    ownerHandle = handleArr;
  }

  if (NULL == ownerHandle) {
    LOG_ERR("Unexpected error: ownerHandle is NULL!\n");
    ret = Object_ERROR;
    goto exit;
  }

  ret = vm_osal_retrieveDmabuf(vmMem, *ownerHandle, handleArr, permArr, nr,
                               memparcelHandle);
  if (ret < 0) {
    LOG_ERR("RetrieveDmabuf failed with ret=%d.\n", ret);
    ret = Object_ERROR;
    goto exit;
  } else {
    *outFd = ret;
    ret = Object_OK;
  }

exit:
  _cleanupVmMem(vmMem);

  return ret;
}

int32_t ShareMemory_GetMSMem(int64_t memparcelHandle, char *destVMName,
                             ITAccessPermissions_rules *confRules,
                             Object *objOut)
{
  int32_t ret = Object_OK, outFd = -1;
  uint64_t specialRules = confRules->specialRules;

  ret = ShareMemory_GetRemoteFileDescriptor(memparcelHandle, destVMName,
                                            specialRules, &outFd);
  if (!Object_isOK(ret)) {
    LOG_ERR("ShareMemory_GetRemoteFileDescriptor failed!!\n");
    goto exit;
  }

  if (Object_isERROR(MSMem_new_remote(outFd, confRules, objOut))) {
    LOG_ERR("creating new MSMem of shared/lent memBuf failed!!\n");
    ret = Object_ERROR_MEM;
    goto exit;
  }

  LOG_MSG("creating new MSMem of shared/lent memBuf with dmabufFd=%d, constructed \
           msMem objOut=%p\n", outFd, objOut);

exit:

  return ret;
}

int32_t ShareMemory_ReclaimMemBuf(int32_t fd, int64_t memparcelHandle)
{
  int32_t ret = Object_OK;
  vm_osal_vmMem *vmMem = NULL;

  OK_OR_EXIT(_createVmMem(&vmMem));

  ret = vm_osal_reclaimDmabuf(vmMem, fd, memparcelHandle);

  if (ret < 0) {
    LOG_ERR("ReclaimDmabuf failed eventually!!!\n");
    ret = Object_ERROR;
  } else {
    ret = Object_OK;
  }

exit:
  _cleanupVmMem(vmMem);

  return ret;
}
