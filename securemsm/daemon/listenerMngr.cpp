/********************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/

#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <vector>
#include <BufferAllocator/BufferAllocator.h>
#include <linux/msm_ion.h>
#include <linux/dma-buf.h>
#include <sys/mman.h>
#include "listenerFunc.h"
#include "listenerMngr.h"
#include "IRegisterListenerCBO.h"
#include "IClientEnv.h"
#include "object.h"
#include "TZCom.h"
#include "CListenersmci.h"
#include "CRegisterListenerCBO.h"
#ifdef ANDROID
#include <utils/Log.h>
#include <common_log.h>
#include <android/log.h>
#include <cutils/properties.h>
#endif
#ifdef OE
#include <syslog.h>
#define LOGI(...) syslog(LOG_NOTICE, "INFO:" __VA_ARGS__)
#define LOGV(...) syslog(LOG_NOTICE,"VERB:" __VA_ARGS__)
#define LOGD(...) syslog(LOG_DEBUG,"DBG:" __VA_ARGS__)
#define LOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#define LOGW(...) syslog(LOG_WARNING,"WRN:" __VA_ARGS__)
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "ListenerMngr"
#endif

using namespace std;

std::vector<Object> oRegister;

static struct listenerServices listeners[] = {
#ifndef LISTENERS_VIRTUALIZED
#ifdef COMPILE_RPMB_LISTENER
        /* Change the value of RPMB_LISTENER_INDEX incase you need to change this order */
        {
                .serviceName = "RPMB system services",
                .id = RPMB_SERVICE,
                .isRegistered = false,

#ifdef OE
                .fileName = "librpmb.so.1",
#else
                .fileName = "librpmb.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = "rpmb_init_service",
                .listenerLen = RPMB_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#endif //COMPILE_RPMB_LISTENER
#ifndef OE
        /* Change the value of SSD_LISTENER_INDEX incase you need to change this order */
        {
                .serviceName = "SSD system services",
                .id = SSD_SERVICE,
                .isRegistered = false,

#ifdef OE
                .fileName = "libssd.so.1",
#else
                .fileName = "libssd.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = "ssd_init_service",
                .listenerLen = SSD_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#endif //!OE
#endif //!LISTENERS_VIRTUALIZED
#ifndef OE
#ifdef COMPILE_GPT_LISTENER
        /* Change the value of GPT_LISTENER_INDEX incase you need to change this order */
        {
                .serviceName = "GPT system services",
                .id = GPT_SERVICE,
                .isRegistered = false,
#ifdef OE
                .fileName = "libgpt.so.1",
#else
                .fileName = "libgpt.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = "gpt_init_service",
                .listenerLen = GPT_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#endif // COMPILE_GPT_LISTENER
#endif //!OE
        {
                .serviceName = "time services",
                .id = TIME_SERVICE,
                .isRegistered = false,

#ifdef OE
                .fileName = "libdrmtime.so.1",
#else
                .fileName = "libdrmtime.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = NULL,
                .listenerLen = TIME_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
        {
                .serviceName = "file system services",
                .id = FILE_SERVICE,
                .isRegistered = false,
#ifdef OE
                .fileName = "libdrmfs.so.1",
#else
                .fileName = "libdrmfs.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = NULL,
                .listenerLen = FILE_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr}
        },
        {
                .serviceName = "gpfile system services",
                .id = GPFILE_SERVICE,
                .isRegistered = false,
#ifdef OE
                .fileName = "libdrmfs.so.1",
#else
                .fileName = "libdrmfs.so",
#endif
                .smciDispatch = "smci_gpdispatch",
                .fileInit = "config_path_init",
                .listenerLen = GPFILE_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#ifndef OE
        {
                .serviceName = "gp request cancellation services",
                .id = GPREQCANCEL_SERVICE,
                .isRegistered = false,
                .fileName = "libGPreqcancel.so",
                .smciDispatch = "smci_dispatch",
                .fileInit = "smci_listener_init",
                .listenerLen = GPREQCANCEL_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#ifdef COMPILE_OPS_LISTENER
        {
                .serviceName = "output protection service listener",
                .id = OPS_SERVICE_ID,
                .isRegistered = false,
#ifdef OE
                .fileName = "libops.so.1",
#else
                .fileName = "libops.so",
#endif
                .smciDispatch = "smci_dispatch",
                .fileInit = "smci_listener_init",
                .listenerLen = OPS_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#endif
        {
                .serviceName = "secure processor services", // SP-to-TZ interrupt
                .id = SP_SERVICE,
                .isRegistered = false,
                .fileName  = "libspl.so",
                .smciDispatch = "smci_dispatch",
                .fileInit  = "smci_listener_init",
                .listenerLen = SP_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
        {
                .serviceName = "interrupt services",
                .id = QIS_SERVICE,
                .isRegistered = false,
                .fileName = "libqisl.so",
                .smciDispatch = "smci_dispatch",
                .fileInit = "smci_listener_init",
                .listenerLen = OPS_SERVICE_BUF_LEN,
                .libHandle = NULL,
                .dmaBufMgr = {-1, nullptr},
        },
#endif  //!OE
};

static void  releaseDmabuf(struct dmaBufManager *dmaBufMgr)
{
  close(dmaBufMgr->dmaFd);
  delete(dmaBufMgr->dmaBufAllocator);
  dmaBufMgr->dmaBufAllocator = nullptr;
}

int allocateDmaBuf(struct dmaBufManager *dmaBufMgr, size_t dmaBuffLen)
{
  int ret  = -1;
  int32_t dmafd = -1;
  dmaBuffLen = (dmaBuffLen + 4095) & (~4095);
  dmaBufMgr->dmaBufAllocator = new BufferAllocator();

  dmafd = dmaBufMgr->dmaBufAllocator->Alloc("qcom,qseecom", dmaBuffLen, 0, 0);
  if (dmafd < 0) {
    LOGE("Failed to allocate dma memory %d\n", dmafd);
    ret = -1;
    goto alloc_fail;
  }
  dmaBufMgr->dmaFd = dmafd;
  return 0;

alloc_fail:
  close(dmafd);
  delete(dmaBufMgr->dmaBufAllocator);
  dmaBufMgr->dmaBufAllocator = nullptr;
  return ret;
}

static void stop_listeners_smci(void)
{
  int numListeners = 0;
  int idx = 0;

  /* Stop the root listener services */
  numListeners = sizeof(listeners)/sizeof(struct listenerServices);
  LOGD("Total listener services to be stopped = %d", numListeners);
  for (vector<Object>::iterator it = oRegister.begin();
            it != oRegister.end(); ++it) {
            Object_ASSIGN_NULL(*it);
  }
  for (idx = 0; idx < numListeners; idx++) {
    /* resource cleanup for registered listeners */
    if(listeners[idx].isRegistered) {
      Object_RELEASE_IF(listeners[idx].oListener);
      releaseDmabuf(&listeners[idx].dmaBufMgr);
      listeners[idx].isRegistered = false;
    }
    /* close libhandle for all listeners */
    if(listeners[idx].libHandle != NULL) {
      dlclose(listeners[idx].libHandle);
      listeners[idx].libHandle = NULL;
    }
  }
}

#ifdef __cplusplus
extern "C" {
#endif

int init_listeners(void)
{
  serviceInitialize srvInitialize;
  int ret = 0;

  for (int index = 0; index < sizeof(listeners)/sizeof(struct listenerServices); index++) {
    if (listeners[index].fileInit) {
      listeners[index].libHandle  = dlopen(listeners[index].fileName,
        RTLD_NOW);
      if (listeners[index].libHandle == NULL) {
        LOGE("Init dlopen(%s, RLTD_NOW) is failed.... %s\n",
                      listeners[index].fileName, dlerror());
        return -1;
      }

      srvInitialize =  (serviceInitialize)dlsym(listeners[index].libHandle,
                            listeners[index].fileInit);
      if (srvInitialize == NULL) {
        LOGE("dlsym(%s) not found in lib %s, dlerror msg: %s\n",
                        listeners[index].fileInit,
                        listeners[index].fileName,
                        dlerror());
        return -1;
      }
      ret = (*srvInitialize)();
      if (ret < 0) {
        LOGE("Init for dlsym(%s) failed, ret = %d",
                    listeners[index].fileInit, ret);
        return -1;
      }
    }
  }

  return 0;
}

int start_listeners_smci(void)
{
  int ret = 0;
  int idx = 0;
  int numListeners = 0;

  Object oMem = Object_NULL;
  Object oClientEnv = Object_NULL;
  Object oRegisterTmp = Object_NULL;

  numListeners = sizeof(listeners)/sizeof(struct listenerServices);
  LOGD("Total listener services to start = %d", numListeners);
  for (idx = 0; idx < numListeners; idx++) {

    /* There are 4 threads for each callback. */
    ret = TZCom_getClientEnvObject(&oClientEnv);
    if (ret) {
      Object_ASSIGN_NULL(oClientEnv);
      LOGE("Error %d getting clientEnv\n", ret);
      goto exit_release;
    }

    ret = IClientEnv_open(oClientEnv, CRegisterListenerCBO_UID, &oRegisterTmp);
    Object_ASSIGN_NULL(oClientEnv);
    if (ret) {
      LOGE("Error %d getting obj for UID = %d\n", ret, CRegisterListenerCBO_UID);
      ret = -1;
      goto exit_release;
    }

    oRegister.push_back(Object_NULL);
    Object_ASSIGN(oRegister[idx], oRegisterTmp);
    Object_ASSIGN_NULL(oRegisterTmp);

    ret = allocateDmaBuf(&listeners[idx].dmaBufMgr, listeners[idx].listenerLen);
    if (ret) {
      ret = -1;
      LOGE("dma alloc returned error");
      goto exit_release;
    }

    ret = TZCom_getFdObject(dup(listeners[idx].dmaBufMgr.dmaFd), &oMem);
    if (Object_isERROR(ret)) {
      ret = -1;
      LOGE("tzcom_getfdobj failed returned error");
      goto exit_release_dma;
    }

    /* Create CBO listener and register it */
    ret = CListenerCBO_new(&listeners[idx].oListener, oMem, &listeners[idx]);
    if (Object_isERROR(ret)) {
      ret = -1;
      LOGE("CListenerCBO_new failed returned error");
      goto exit_release_dma;
    }
    ret = IRegisterListenerCBO_register(oRegister[idx], listeners[idx].id, listeners[idx].oListener, oMem);
    if (Object_isERROR(ret)) {
      LOGE("IRegisterListenerCBO_register(%d) failed, ret = %d",listeners[idx].id, ret);
      ret = -1;
      goto exit_release_oListener;
    }
    /* set isRegistered of the listener to true on successful register */
    listeners[idx].isRegistered = true;
    Object_ASSIGN_NULL(oMem);
  }

  return ret;

exit_release_oListener:
  /* release oListener for the current listener for which operation failed */
  Object_RELEASE_IF(listeners[idx].oListener);

exit_release_dma:
  /* release dma memory for the current listener for which operation failed */
  releaseDmabuf(&listeners[idx].dmaBufMgr);
exit_release:
  /* if current listener registation fails, stop previous listeners services */
  Object_RELEASE_IF(oMem);
  stop_listeners_smci();
  return ret;
}

#ifdef __cplusplus
}
#endif
