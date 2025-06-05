/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif

#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/Log.h>
#include <string.h>
#include "stdint.h"
#include "stdlib.h"
#include "AEEStdErr.h"
#include "verify.h"
#include "HAP_farf.h"
#include "object.h"
#include "fdwrapper.h"
#include "LinkCredentials.h"
#include "xtzdCredentials.h"
#include "CLoadAlgoTA.hpp"
#include "ILoadAlgoTA.hpp"
#include "minkipc.h"
#include "ITProcessLoader.hpp"
#include "CTProcessLoader.hpp"
#include "IModule.hpp"
#include "RemoteShareMemory.h"

extern "C" {
#include "secure_memory.h"
}

extern const char* __progname;


/*
 * For debugging/development only. This allows to load DSP service from PVM.
 * DSP service would need to be remove from EmbeddedProcessLoader.c on LE as well.
 */
#ifdef LOAD_DSPSERVICE
char *DSP_file = "/data/local/tmp/DSP_service";
#endif

/*
 * Service ID to connect to QTVM.
 * QTVM will connect to QRTR advertising itself as 5008
 */
#define MINK_QRTR_LE_SERVICE 5008

#define NAME_SIZE 100

#define SECURE_MODULE_INTERFACE "example_algo"
#define SECURE_MODULE_INTERFACE_LENGTH (sizeof(SECURE_MODULE_INTERFACE))

#define DESTROYIF(pv, destroy_func) \
do {                                \
   if(pv) {                         \
      destroy_func(pv);             \
      pv = nullptr;                 \
  }                                 \
} while(0)

#define MB 1024*1024
#define ALIGN_SIZE(size, alignment) \
    (((size) + ((alignment) - 1)) & ~((alignment) - 1))

#define DELETEIF(pv) DESTROYIF(pv, delete)
#define FREEIF(pv) DESTROYIF(pv, free)
#define FCLOSEIF(pv) DESTROYIF(pv, fclose)

struct file_info {
   IMemObject *proxy_file;
   Object object_file;
   mem_handle *file_buffer;
   char name[NAME_SIZE];
   uint32_t name_length;
};

struct minkIPC_connection {
   Object mLinkCredentials;
   Object mXtzdCredentials;
   Object qtvm_opener;
   MinkIPC *opener_conn;
   ProxyBase *TA_session_proxy;
   ITProcessController *TA_process_controller;
};


extern "C" {
extern int run_gaussian_algo_qtvm(char* TA_path, char* module_path, unsigned dynamic_heap_size, unsigned hlos_physpool_size);
extern int get_file_size(FILE* stream, size_t* memsz);
}

static int load_file_to_object(file_info *file_info, enum mem_alloc_heaps heap);

/*
 * Open session on TVM.
 *
 * Args
 *   @mink_connect : Structure containing information about TVM and TA session.
 *
 * Return : Loadalgo_TA handle on success. Nullptr on failure.
 */
static int32_t setup_minkIPC_connection(minkIPC_connection *mink_connect) {
   int32_t nErr = AEE_SUCCESS;
   CredInfo info = {};
   info.pid = getpid();

   mink_connect->opener_conn = MinkIPC_connect_QRTR(MINK_QRTR_LE_SERVICE, &mink_connect->qtvm_opener);
   if(nullptr == mink_connect->opener_conn) {
      nErr = AEE_ENOSUCH;
      ALOGE("Error 0x%x: %s: failed to connect to QRTR LE service %d\n", nErr, __func__, MINK_QRTR_LE_SERVICE);
      goto bail;
   }

   // Create LinkCredentials and XtzdCredentials
   // These should be done in ssgtzd in the future
   nErr = LinkCredentials_new(&info, ELOC_LOCAL, UNIX, &mink_connect->mLinkCredentials);
   if (Object_OK != nErr) {
      ALOGE("Error %d: %s: failed link credentials remote\n", nErr, __func__);
      goto bail;
   }

   nErr = XtzdCredentials_newFromCBO(mink_connect->mLinkCredentials, &mink_connect->mXtzdCredentials);
   if (Object_OK != nErr) {
      ALOGE("Error %d: %s: failed to wrap Xtzd credentials\n", nErr, __func__);
      goto bail;
   }
   printf("%s: Connection with Mink QRTR LE service %d established\n", __func__, MINK_QRTR_LE_SERVICE);

bail:
   if (nErr)
      printf("Error 0x%x: %s: Mink QRTR LE service %d failed to open or authenticate\n", nErr, __func__, MINK_QRTR_LE_SERVICE);

   return nErr;
}

/*
 * Release and destroy TVM session.
 *
 * Args
 *   @mink_connect : Structure containing information about TVM and TA session.
 */
static void release_minkIPC_connection(minkIPC_connection *mink_connect) {
   Object_RELEASE_IF(mink_connect->mXtzdCredentials);
   Object_RELEASE_IF(mink_connect->mLinkCredentials);
   Object_RELEASE_IF(mink_connect->qtvm_opener);
   DESTROYIF(mink_connect->opener_conn, MinkIPC_release);
}

/*
 * Open a session on TA by loading TA on TVM first and then connecting to it.
 *
 * Args
 *   @mink_connect    : Structure containing information about TVM and TA session.
 *   @TA_path         : Absolute path to TA.
 *   @TA_service_uid  : TA uid.
 *
 * Return : Loadalgo_TA handle on success. Nullptr on failure.
 */
static Loadalgo_TA* create_TA_IPC_session(minkIPC_connection *mink_connect, const char* TA_path, uint32_t TA_service_uid) {
   int32_t nErr = AEE_SUCCESS;
   Loadalgo_TA *TA_session_handle = nullptr;
   ProxyBase *fd_proxy = nullptr, *credentials_proxy = nullptr, *proc_loader_proxy = nullptr;
   ITProcessLoader *my_process_loader = nullptr;
   IModule *my_opener = nullptr;
   file_info file_info = {};

#ifdef LOAD_DSPSERVICE
   ITProcessController *DSP_process_controller = nullptr;
   ProxyBase *session_proxy = nullptr;
#endif

   my_opener = new IModule(mink_connect->qtvm_opener);
   if (my_opener == nullptr) {
      nErr = Object_ERROR_MEM;
      ALOGE("Error %d: %s: failed to create IModule for QTVM\n", nErr, __func__);
      goto bail;
   }

   credentials_proxy = new ProxyBase(mink_connect->mLinkCredentials);
   if (credentials_proxy == nullptr) {
      nErr = Object_ERROR_MEM;
      ALOGE("Error %d: %s: failed to create ProxyBase for credentials\n", nErr, __func__);
      goto bail;
   }

   proc_loader_proxy = new ProxyBase();
   if (proc_loader_proxy == nullptr) {
      nErr = Object_ERROR_MEM;
      ALOGE("Error %d: %s: failed to create ProxyBase for process loader\n", nErr, __func__);
      goto bail;
   }

   nErr = my_opener->open(CTProcessLoader_UID, *credentials_proxy, *proc_loader_proxy);
   if (Object_OK != nErr) {
      ALOGE("Error %d: %s: failed to open session to process loader service (uid %d)\n", nErr, __func__, CTProcessLoader_UID);
      goto bail;
   }

   my_process_loader = new ITProcessLoader(proc_loader_proxy->get());
   if (my_process_loader == nullptr) {
      nErr = Object_ERROR_MEM;
      ALOGE("Error %d: %s: failed to create ITProcessLoader handle\n", nErr, __func__);
      goto bail;
   }
   printf("%s: opened session to process loader service (uid %d)\n", __func__, CTProcessLoader_UID);

#ifdef LOAD_DSPSERVICE

   file_info.name_length = strlen((const char*)DSP_file);
   if (file_info.name_length + 1 > NAME_SIZE) {
      nErr = AEE_EBUFFERTOOSMALL;
      goto bail;
   }
   strlcpy(file_info.name, (const char*)DSP_file, sizeof(file_info.name));
   nErr = load_file_to_object(&file_info, DISPLAY_HEAP);
   if (nErr) {
      goto bail;
   }
   printf("%s: file %s (size %u bytes) loaded to object using heap %u\n", __func__, DSP_file, file_info.file_buffer->size, DISPLAY_HEAP);

   fd_proxy = new ProxyBase(file_info.object_file);
   if (fd_proxy == nullptr) {
      nErr = AEE_EMEMPTR;
      goto bail;
   }

   DSP_process_controller = new ITProcessController();
   if (DSP_process_controller == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create ITProcessController for DSP service\n", nErr, __func__);
      goto bail;
   }
   nErr = my_process_loader->loadFromBuffer(*fd_proxy, *DSP_process_controller);
   if (Object_OK != nErr) {
      printf("Error %d: %s: failed to load %s from buffer\n", nErr, __func__, DSP_file);
      goto bail;
   }
   printf("%s: service %s loaded on QTVM\n", __func__, DSP_file);
#endif

   memset(&file_info, 0, sizeof(file_info));
   file_info.name_length = strlen((const char*)TA_path);
   if (file_info.name_length + 1 > NAME_SIZE) {
      nErr = AEE_EBUFFERTOOSMALL;
      goto bail;
   }
   strlcpy(file_info.name, (const char*)TA_path, sizeof(file_info.name));
   nErr = load_file_to_object(&file_info, DISPLAY_HEAP);
   if (nErr) {
      goto bail;
   }
   printf("%s: file %s (size %zu bytes) loaded to fd object using heap %u\n", __func__, TA_path, file_info.file_buffer->size, DISPLAY_HEAP);

   fd_proxy = new ProxyBase(file_info.object_file);
   if (fd_proxy == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create ProxyBase for fd object %s\n", nErr, __func__, TA_path);
      goto bail;
   }

   mink_connect->TA_process_controller = new ITProcessController();
   if (mink_connect->TA_process_controller == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create ITProcessController for %s\n", nErr, __func__, TA_path);
      goto bail;
   }

   nErr = my_process_loader->loadFromBuffer(*fd_proxy, *mink_connect->TA_process_controller);
   if (Object_OK != nErr) {
      ALOGE("Error %d: %s: failed to load %s\n", nErr, __func__, TA_path);
      goto bail;
   }
   printf("%s: service %s loaded on QTVM\n", __func__, TA_path);

   mink_connect->TA_session_proxy = new ProxyBase();
   if (mink_connect->TA_session_proxy == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create ProxyBase for %s\n", nErr, __func__, TA_path);
      goto bail;
   }
   nErr = my_opener->open(TA_service_uid, *credentials_proxy, *mink_connect->TA_session_proxy);
   if (Object_OK != nErr) {
      ALOGE("Error %d: %s: failed to open session to TA service %s (uid %d)\n", nErr, __func__, TA_path, TA_service_uid);
      goto bail;
   }

   TA_session_handle = new Loadalgo_TA(mink_connect->TA_session_proxy->get());
   if (TA_session_handle == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create session handle to TA (uid %u)\n", nErr, __func__, TA_service_uid);
      goto bail;
   }

   printf("%s: TA service (uid %d) connection established with session handle %p\n", __func__, TA_service_uid, TA_session_handle);
bail:
   if (nErr)
      printf("Error 0x%x(%d): %s: failed to connect to TA %s (uid %d)\n", nErr, nErr, __func__, TA_path, TA_service_uid);

   DELETEIF(fd_proxy);
   secure_mem_free(file_info.file_buffer);
   FREEIF(file_info.file_buffer);
   DELETEIF(credentials_proxy);
   DELETEIF(my_opener);
   DELETEIF(my_process_loader);
   DELETEIF(proc_loader_proxy);
   return TA_session_handle;
}

/*
 * Destroy TA session.
 *
 * Args
 *   @mink_connect  : Structure containing information about TVM and TA sessions.
 */
static void destroy_TA_IPC_session(minkIPC_connection *mink_connect) {
   DELETEIF(mink_connect->TA_session_proxy);
   DELETEIF(mink_connect->TA_process_controller);
}

/*
 * Copy file to dma buffer.
 *
 * Args
 *   @stream  : File stream.
 *   @memsz   : Size of file.
 */
static mem_handle *copy_file_to_dma_buffer(FILE* stream, size_t memsz, enum mem_alloc_heaps heap) {
   int32_t nErr = AEE_SUCCESS;
   size_t numRead = 0;
   mem_handle *buffer = nullptr;

   buffer = (mem_handle *)calloc(sizeof(*buffer), 1);
   nErr = secure_mem_alloc(buffer, memsz, 2*MB, heap);
   if (nErr) {
      ALOGE("Error 0x%x: %s: failed to allocate dma memory of size %zu (aligned to %zu) from heap %u\n", nErr, __func__, memsz, ALIGN_SIZE(memsz, 2*MB), heap);
      goto bail;
   }

   nErr = fseek(stream, 0, SEEK_SET);
   if (nErr) {
      ALOGE("Error %d: %s: failed to seek to the start of the file with errno %d (%s)\n", nErr, __func__, errno, strerror(errno));
      goto bail;
   }
   numRead = fread((void *)buffer->addr, 1, memsz, stream);
   if (numRead != memsz) {
      nErr = AEE_EFREAD;
      ALOGE("Error 0x%x: %s: failed to read file. Expected size: %zu bytes, actual size: %zu bytes\n", nErr, __func__, memsz, numRead);
   }
bail:
   if (nErr) {
      secure_mem_free(buffer);
      FREEIF(buffer);
   }
   return buffer;
}

/*
 * Load file to Mink Object. File is loaded to dma memory and then is encapsulated to Mink Object.
 *
 * Args
 *   @file_info  : Structure containing information about the file.
 *
 * Return : 0 on on success.
 */
static int load_file_to_object(file_info *file_info, enum mem_alloc_heaps heap) {
   int32_t nErr = AEE_SUCCESS, fd = -1;
   FILE *stream = nullptr;
   mem_handle *module_buffer = nullptr;
   size_t size = 0;
   Object loaded_object = Object_NULL;
   ITAccessPermissions_rules confRules = {};

   stream = fopen(file_info->name, "r");
   if (stream == nullptr) {
      nErr = AEE_EFOPEN;
      ALOGE("Error 0x%x: %s: failed to open file %s\n", nErr, __func__, file_info->name);
      goto bail;
   }

   nErr = get_file_size(stream, &size);
   if (nErr) {
      goto bail;
   }

   module_buffer = copy_file_to_dma_buffer(stream, size, heap);
   if (module_buffer == nullptr) {
      nErr = AEE_EUNABLETOLOAD;
      goto bail;
   }

   fd = module_buffer->fd;
   loaded_object = FdWrapper_new(fd);
   if (Object_isNull(loaded_object)) {
      nErr = Object_ERROR_BADOBJ;
      ALOGE("Error %d: %s: failed to obtained wrapper object from fd %d for file %s\n", nErr, __func__, fd, file_info->name);
      goto bail;
   }

   module_buffer->fd = -1;
   munmap((void *)module_buffer->addr, module_buffer->size);
   module_buffer->addr = 0;
   confRules.specialRules = ITAccessPermissions_removeSelfAccess; // Borrower will be the sole owner in TVM
   nErr = RemoteShareMemory_attachConfinement(&confRules, &loaded_object); // Tell mink transport layer this buffer is MEM_LEND
   if (nErr) {
      ALOGE("Error %d: %s: failed to remove self access to buffer\n", nErr, __func__);
      goto bail;
   }

   file_info->file_buffer = module_buffer;
   file_info->object_file = loaded_object;
bail:
   if (nErr) {
      printf("Error 0x%x:(%d): %s: failed to load file %s\n", nErr, nErr, __func__, file_info->name);
      secure_mem_free(module_buffer);
      FREEIF(module_buffer);
   }
   FCLOSEIF(stream);
   return nErr;
}

/*
 * Prepare module by loading it to Mink Object and then encapsulate it to a Proxybase object.
 *
 * Args
 *   @module_info  : Structure containing information about the module.
 *
 * Return : 0 on on success.
 */
static int32_t prepare_module(file_info *module_info) {
   int32_t nErr = AEE_SUCCESS;

   module_info->proxy_file = new IMemObject();
   if (module_info->proxy_file == nullptr) {
      nErr = AEE_EMEMPTR;
      ALOGE("Error 0x%x: %s: failed to create ProxyBase for module %s\n", nErr, __func__, module_info->name);
      goto bail;
   }
   nErr = load_file_to_object(module_info, SYSTEM_HEAP);
   if (nErr) {
      goto bail;
   }

   module_info->proxy_file->consume(module_info->object_file);
   printf("%s: created ProxyBase object for module %s using heap %u\n", __func__, module_info->name, SYSTEM_HEAP);

bail:
   if (nErr) {
      printf("Error 0x%x(%d): %s: failed to setup session on TA for module %s\n", nErr, nErr, __func__, module_info->name);
      DELETEIF(module_info->proxy_file);
   }
   return nErr;
}

static int32_t load_farf_file_to_buffer(file_info *runtime_farf_file_info) {
   FILE *stream = nullptr;
   int nErr = 0, size = 0, namelen = 0;
   char farf_file_name[NAME_SIZE];

   VERIFYC(runtime_farf_file_info->proxy_file = new IMemObject(), AEE_EMEMPTR);
   namelen = snprintf(farf_file_name, sizeof(farf_file_name), "%s.farf", __progname);

   /* Ignore if farf file does not exists */
   stream = fopen(farf_file_name, "r");
   if (stream == nullptr) {
      nErr = AEE_ENOSUCHFILE;
      goto bail;
   }

   strlcpy(runtime_farf_file_info->name, (const char *)&farf_file_name, sizeof(runtime_farf_file_info->name));
   VERIFY(0 == (nErr = load_file_to_object(runtime_farf_file_info, SYSTEM_HEAP)));
   runtime_farf_file_info->proxy_file->consume(runtime_farf_file_info->object_file);
bail:
   FCLOSEIF(stream);
   return nErr;
}

int enable_runtime_farf(Loadalgo_TA *TA_session_handle, file_info *runtime_farf_file_info) {
   int32_t nErr = AEE_SUCCESS;

   nErr = load_farf_file_to_buffer(runtime_farf_file_info);
   if (nErr == AEE_ENOSUCHFILE) {
      nErr = 0;
      goto bail;
   }
   nErr = TA_session_handle->enable_runtime_farf(*runtime_farf_file_info->proxy_file);
   if (nErr)
      goto bail;

   printf("%s: setting runtime mask to secure PD\n", __func__);
bail:
   if (nErr)
      printf("Error 0x%x: %s: failed to enable runtime farf on secure PD\n", nErr, __func__);

   return nErr;
}

/*
 * Start algo by preparing the module first. Then loading it to a ProxyBase object and then send it to TA using
 * TA's session handle.
 *
 * Args
 *   @TA_session_handle  : Session handle for TA.
 *   @module_path        : Absolute path to module shared object.
 *   @dynamic_heap_size  : Secure PD heap size
 *   @hlos_physpool_size : Secure PD pool size
 *
 * Return : 0 on on success.
 */
static int32_t start_algo(Loadalgo_TA *TA_session_handle, char *module_path, unsigned dynamic_heap_size, unsigned hlos_physpool_size) {
   int32_t nErr = AEE_SUCCESS, str_len = 0;
   file_info module_info = {};
   char secure_module_interface[SECURE_MODULE_INTERFACE_LENGTH] = {};

   memset(&module_info, 0, sizeof(module_info));
   module_info.name_length = strlen((const char*)module_path);
   if (module_info.name_length + 1 > NAME_SIZE) {
      nErr = AEE_EBUFFERTOOSMALL;
      goto bail;
   }
   strlcpy(module_info.name, (const char*)module_path, sizeof(module_info.name));
   nErr = prepare_module(&module_info);
   if (nErr) {
      goto bail;
   }

   str_len = snprintf(secure_module_interface, SECURE_MODULE_INTERFACE_LENGTH, "%s", SECURE_MODULE_INTERFACE);

   nErr = TA_session_handle->run_algo(*module_info.proxy_file, (uint32_t)module_info.file_buffer->size, (const uint8_t*)secure_module_interface, str_len + 1 ,
                                      dynamic_heap_size, hlos_physpool_size);
   printf("%s: run_algo returned 0x%x(%d)\n", __func__, nErr, nErr);
bail:
   if (nErr)
      printf("Error 0x%x(%d): %s: failed for module %s\n", nErr, nErr, __func__, module_path);

   secure_mem_free(module_info.file_buffer);
   FREEIF(module_info.file_buffer);
   DELETEIF(module_info.proxy_file);
   return nErr;
}

/*
 * Running module on secure PD through TVM.
 * A connection to TVM will be established that will load the TA on TVM.
 * After TA is loaded and we obtain a session handle for TA, handle will be used
 * to send module to the TA.
 *
 * Args
 *   @TA_path            : Absolute path to TA.
 *   @module_path        : Absolute path to module shared object.
 *   @dynamic_heap_size  : Secure PD heap size
 *   @hlos_physpool_size : Secure PD pool size
 *
 * Return : 0 on on success.
 */
int run_gaussian_algo_qtvm(char* TA_path, char* module_path, unsigned dynamic_heap_size, unsigned hlos_physpool_size) {
   Loadalgo_TA *TA_session_handle = nullptr;
   int32_t nErr = 0;
   minkIPC_connection mink_connect;
   file_info runtime_farf_file_info = {};

   printf("%s: running through QTVM\n", __func__);
   memset(&mink_connect, 0, sizeof(mink_connect));
   if (module_path == nullptr) {
      nErr = AEE_EBADPARM;
      ALOGE("Error 0x%x: %s: incorrect module name\n", nErr, __func__);
      goto bail;
   }
   nErr = setup_minkIPC_connection(&mink_connect);
   if (nErr) {
      goto bail;
   }

   TA_session_handle = create_TA_IPC_session(&mink_connect, TA_path, CLoadAlgoTA_UID);
   if (TA_session_handle == nullptr) {
      nErr = AEE_EMEMPTR;
      goto bail;
   }

   nErr = enable_runtime_farf(TA_session_handle, &runtime_farf_file_info);
   if (nErr) {
      goto bail;
   }

   nErr = start_algo(TA_session_handle, module_path, dynamic_heap_size, hlos_physpool_size);
   if (nErr) {
      goto bail;
   }

bail:
   if (nErr) {
      printf("Error 0x%x(%d): secure PD TVM test failed\n", nErr, nErr);
   }
   if (TA_session_handle != nullptr)
      printf("%s: TA session handle %p destroyed\n", __func__, TA_session_handle);
   DELETEIF(TA_session_handle);
   destroy_TA_IPC_session(&mink_connect);
   release_minkIPC_connection(&mink_connect);
   secure_mem_free(runtime_farf_file_info.file_buffer);
   FREEIF(runtime_farf_file_info.file_buffer);
   DELETEIF(runtime_farf_file_info.proxy_file);
   printf("%s: Test returned 0x%x\n", __func__, nErr);
   return nErr;
}
