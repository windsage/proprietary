/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "ITMemoryService.hpp"
#include "CTMemoryService.hpp"
#include "ITEnv.hpp"
#include "object.h"

#include "CLoadAlgoTA.hpp"
#include "CSecureDSPService.hpp"
#include "ISecureDSPService.hpp"
#include "LoadAlgoTA.hpp"

#define LOG_TAG "loadalgo_TA: "
#define LOG_QTVM(MSG, ...)              \
do {                                    \
   printf(LOG_TAG MSG, ##__VA_ARGS__);  \
   fflush(stdout);                      \
} while(0)

#define MINK_QRTR_LE_SERVICE 5008
#define DESTROYIF(pv, destroy_func, default_value) \
do {                                \
   if(pv) {                         \
      destroy_func(pv);             \
      pv = default_value;           \
  }                                 \
} while(0)

#define DELETEIF(pv) DESTROYIF(pv, delete, nullptr)
#define FREEIF(pv) DESTROYIF(pv, free, nullptr)

extern Object gTVMEnv;
extern "C" {
extern int32_t tProcessOpen(uint32_t uid, Object cred, Object *objOut);
extern void tProcessShutdown(void);
}

static sem_t service_shutdown_sem;

struct memory_pool_service {
   ProxyBase * user_pool_factory;
   ITMemPool * user_pool;
   uint32_t pool_mem_size;
   ITMemPoolFactory *my_pool;
};

struct user_memory {
   uint32_t size;
   ProxyBase *proxy_buffer;
   void *va;
};

static int32_t setup_mem_service_pool(memory_pool_service *my_pool_service, ITEnv *my_opener);
static void release_mem_service_pool(memory_pool_service *my_pool_service);

enum errors_TA{
   SUCCESS = 0,
   EMEMPTR = 10,
   EBADSIZE,
   ENOMEMORY,
   ENOSUCH,
   EBADOBJ
};

enum module_op{
   GAUSSIAN = 0
};

/* ======================================================================== */
/*  Reference C version of gaussian7x7u8()                                  */
/* ======================================================================== */
static const int32_t GAUSS_7x7[7*7] = {
   1,   6,  15,  20,  15,   6,  1,
   6,  36,  90, 120,  90,  36,  6,
  15,  90, 225, 300, 225,  90, 15,
  20, 120, 300, 400, 300, 120, 20,
  15,  90, 225, 300, 225,  90, 15,
   6,  36,  90, 120,  90,  36,  6,
   1,   6,  15,  20,  15,   6,  1
};

/*
 * Open a DSP module on secure PD using the DSP service handle.
 *
 * Args
 *   @dsp_service_session_handle  : DSP service handle
 *   @module                      : Loadalgo shared object encapsulated on a ProxyBase object
 *   @module_size                 : Shared object size
 *   @module_name                 : Shared object filename
 *   @module_name_length          : Shared object filename's length
 *
 * Return : SecureDspModule handle on sucess, nullptr on failure.
 */
SecureDspModule *load_module_in_dsp_session(SecureDspSession *dsp_service_session_handle, const ProxyBase *module, uint32_t module_size, const uint8_t *module_name, size_t module_name_length) {
   int32_t nErr = 0;
   SecureDspModule *dsp_module_session_handle = nullptr;
   char *bad_interface_name = "BAD INTERFACE NAME";

   if (dsp_service_session_handle == nullptr || module_name == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }
   if (module_size == 0 || module_name_length == 0) {
      nErr = EBADSIZE;
      goto bail;
   }

   dsp_module_session_handle = new SecureDspModule();
   if (dsp_module_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }

   if (Object_OK != (nErr = dsp_service_session_handle->openModule(*module, module_size, module_name, module_name_length, *dsp_module_session_handle)))
      goto bail;

   LOG_QTVM("%s: opened DSP module handle %p (%u bytes) for interface %s on securePD\n", __func__, dsp_module_session_handle, module_size, (char *)module_name);

bail:
   if (nErr) {
      module_name = (module_name) ? module_name: (const uint8_t *)bad_interface_name;
      LOG_QTVM("Error 0x%x(%d): %s failed to load module module interface %s (%u bytes) on securePD\n", nErr, nErr, __func__, (char *)module_name, module_size);
      DELETEIF(dsp_module_session_handle);
   }

   return dsp_module_session_handle;
}

/*
 * Close a DSP module on secure PD using the DSP service handle.
 *
 * Args
 *   @dsp_service_session_handle       : DSP service handle
 *   @dsp_module_session_handle        : DSP module handle
 *
 * Return : 0 on on success.
 */
static int32_t close_module_in_dsp_session(SecureDspSession *dsp_service_session_handle, SecureDspModule *dsp_module_session_handle) {
   int32_t nErr = SUCCESS;

   if (dsp_service_session_handle == nullptr || dsp_module_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }

   if (Object_OK != (nErr = dsp_service_session_handle->closeModule(*dsp_module_session_handle))) {
      LOG_QTVM("Error 0x%x(%d): %s: failed to close DSP module handle %p using DSP service session handle %p\n", nErr, nErr, __func__, dsp_module_session_handle, dsp_service_session_handle);
      goto bail;
   }
   LOG_QTVM("%s: closed DSP module handle %p using DSP service session handle %p\n", __func__, dsp_module_session_handle, dsp_service_session_handle);
bail:
   if (nErr)
      LOG_QTVM("Error 0x%x(%d): %s: failed for DSP module handle %p\n", nErr, nErr, __func__, dsp_module_session_handle);

   return nErr;
}

/*
 * Release user's buffers.
 *
 * Args
 *   @buffer                   : List of user's buffers
 *   @buffers_size             : Length of users buffer's list
 */
static void release_user_buffer(user_memory *buffer, uint32_t buffers_size) {
   uint32_t ii = 0;

   for (ii = 0; ii < buffers_size; ++ii) {
      if (buffer[ii].proxy_buffer) {
         DELETEIF(buffer[ii].proxy_buffer);
         munmap(buffer[ii].va, buffer[ii].size);
         LOG_QTVM("%s: released buffer [%d]: va %p, size %u\n", __func__, ii, buffer[ii].va, buffer[ii].size);
      }
   }
}

/*
 * Allocate user's buffers from the memory pool.
 *
 * Args
 *   @user_pool            : Memory pool
 *   @buffer               : List of user's buffers
 *   @buffers_size         : Length of users buffer's list
 *
 * Return : 0 on on success.
 */
static int32_t allocate_user_buffer(ITMemPool *user_pool, user_memory *buffer, uint32_t buffers_size) {
   int32_t nErr = SUCCESS;
   uint32_t ii = 0;
   int fd = -1;

   for (ii = 0; ii < buffers_size; ++ii) {
      if (buffer[ii].size == 0)
         continue;

      buffer[ii].proxy_buffer = new ProxyBase();
      if (buffer[ii].proxy_buffer == nullptr) {
         nErr = EMEMPTR;
         LOG_QTVM("Error 0x%x: %s: failed to allocate ProxyBase for buffer %u\n", nErr, __func__, ii);
         goto bail;
      }
      nErr = user_pool->allocateBuffer(buffer[ii].size, *buffer[ii].proxy_buffer);
      if (Object_OK != nErr) {
         LOG_QTVM("Error 0x%x: %s: failed to allocate buffer %u with size %u\n", nErr, __func__, ii, buffer[ii].size);
         goto bail;
      }
      if (Object_OK != (nErr = Object_unwrapFd(buffer[ii].proxy_buffer->get(), &fd))) {
         LOG_QTVM("Error %d: %s: failed to obtain buffer for fd %d\n", nErr, __func__, fd);
         goto bail;
      }

      if (MAP_FAILED == (buffer[ii].va = mmap(nullptr, buffer[ii].size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0))) {
         nErr = -1;
         LOG_QTVM("Error %d: %s: failed to mmap fd %d, size %u\n", nErr, __func__, fd, buffer[ii].size);
         goto bail;
      }
      LOG_QTVM("%s: allocated buffer[%d]: va %p, size %u\n", __func__, ii, buffer[ii].va, buffer[ii].size);
   }

bail:
   if (nErr) {
      LOG_QTVM("Error 0x%x(%d): %s failed for buffer %d\n", nErr, nErr, __func__, ii);
      release_user_buffer(buffer, buffers_size);
   }
   return nErr;
}

/*
 * Setup and create memory pool.
 *
 * Args
 *   @my_pool_service              : Structure holding reference to memory pool resources
 *   @my_opener                    : Handle used to open sessions on QTVM's services
 *
 * Return : 0 on on success.
 */
static int32_t setup_mem_service_pool(memory_pool_service *my_pool_service, ITEnv *my_opener) {
   ProxyBase *memPoolFactory = nullptr;
   ITMemPool *memPool = nullptr;
   ITMemPoolFactory *my_pool = nullptr;
   uint32_t nErr = Object_OK;
   ITAccessPermissions_rules defaultRules = {0};
   defaultRules.specialRules = ITAccessPermissions_keepSelfAccess;

   memPoolFactory = new ProxyBase();
   if (memPoolFactory == nullptr) {
      nErr = EMEMPTR;
      LOG_QTVM("Error 0x%x: %s: failed to allocate ProxyBase for memory pool\n", nErr, __func__);
      goto bail;
   }
   memPool = new ITMemPool();
   if (memPool == nullptr) {
      nErr = EMEMPTR;
      LOG_QTVM("Error 0x%x: %s: failed to allocate ITMemPool structure\n", nErr, __func__);
      goto bail;
   }

   nErr = my_opener->open(CTMemPoolFactory_UID, *memPoolFactory);
   if (Object_OK != nErr) {
      LOG_QTVM("Error %d: %s: failed to open session to memory pool factory service (uid %u)\n", nErr, __func__, CTMemPoolFactory_UID);
      goto bail;
   }

   my_pool = new ITMemPoolFactory(memPoolFactory->get());
   if (my_pool == nullptr) {
      nErr = EMEMPTR;
      LOG_QTVM("Error 0x%x: %s: failed to allocate ITMemPoolFactory structure\n", nErr, __func__);
      goto bail;
   }

   /* Create memory pool from user's requirement. This will fetch memory from LA and make it available to TA */
   nErr = my_pool->createPool(defaultRules, my_pool_service->pool_mem_size, *memPool);
   if (Object_OK != nErr) {
      LOG_QTVM("Error %d: %s: failed to create memory pool of size %u with permissions (keepSelfAccess %lu)\n", nErr, __func__, my_pool_service->pool_mem_size, defaultRules.specialRules);
      goto bail;
   }
   my_pool_service->user_pool_factory = memPoolFactory;
   my_pool_service->my_pool = my_pool;
   my_pool_service->user_pool = memPool;
   LOG_QTVM("%s: created memory pool of %u bytes with permissions (keepSelfAccess %lu)\n", __func__, my_pool_service->pool_mem_size, defaultRules.specialRules);

bail:
   if (nErr) {
      DELETEIF(memPoolFactory);
      DELETEIF(my_pool);
      DELETEIF(memPool);
   }
   return nErr;
}

/*
 * Releasing and destroying memory pool.
 *
 * Args
 *   @my_pool_service    : Structure holding reference to memory pool resources
 */
static void release_mem_service_pool(memory_pool_service *my_pool_service) {
   DELETEIF(my_pool_service->user_pool);
   DELETEIF(my_pool_service->my_pool);
   DELETEIF(my_pool_service->user_pool_factory);
}

/**
   @brief
   Compute reference output for a given reference pattern.
   This reference output is used later for comparison with
   output generated within secure process on CDSP.
*/
static void Gaussian7x7u8_ref(
    uint8_t       *src,
    int32_t        width,
    int32_t        height,
    int32_t        stride,
    uint8_t        *dst,
    int32_t        dstStride
    )
{
    int32_t x, y, s, t;
    int32_t sum, out;

    for (y = 3; y < height - 3; y++)
    {
        for (x = 3; x < width - 3; x++)
        {
            sum = 0;

            for (t = -3; t <= 3; t++)
            {
                for (s = -3; s <= 3; s++)
                {
                    sum += src[(y+t)*stride + x+s] * GAUSS_7x7[((t+3)*7)+(s+3)];
                }
            }

            out  = sum >> 12;

            out = out < 0 ? 0 : out > 255 ? 255 : out;
            dst[y*dstStride + x] = (uint8_t)out;
        }
    }
}

static int32_t init_buffers(user_memory *in_buffers, uint32_t srcHeight, uint32_t srcStride, uint32_t srcWidth, user_memory *out_buffers, uint32_t dstStride, uint8_t **ref) {
   int32_t nErr = 0;
   uint8_t *src = nullptr, *dst_ref = nullptr;
   uint32_t srcSize = 0;
   uint32_t ii, jj;

   src = (uint8_t *)in_buffers[0].va;
   srcSize = in_buffers[0].size;
   for (jj = 0; jj < srcHeight; jj++)
   {
      uint8_t *ptr = &src[jj * srcStride];
      for (ii = 0; ii < srcWidth; ii++)
      {
         *ptr++ = ii + jj;        // some incrementing pattern fill
      }
   }

   memcpy(in_buffers[1].va, (void *)&srcWidth, sizeof(srcWidth));
   memcpy(in_buffers[2].va, (void *)&srcHeight, sizeof(srcHeight));
   memcpy(in_buffers[3].va, (void *)&srcStride, sizeof(srcStride));
   memcpy(out_buffers[1].va, (void *)&dstStride, sizeof(dstStride));

   dst_ref = (uint8_t *)calloc(1, srcSize);
   if (dst_ref == nullptr) {
      nErr = ENOMEMORY;
      LOG_QTVM("Error 0x%x: %s: Unable to allocate ref buffer with size %u", nErr, __func__, srcSize);
      goto bail;
   }
   Gaussian7x7u8_ref(src, srcWidth, srcHeight, srcStride, dst_ref, srcStride);
   *ref = dst_ref;
bail:
   if (nErr)
      FREEIF(dst_ref);
   return nErr;
}

static int32_t match_result(uint8_t *addr, uint32_t dstHeight, uint32_t dstWidth, uint32_t dstStride, uint8_t *ref) {
   uint8_t *dst = nullptr;
   uint32_t ii, jj;
   uint32_t bitexactErrors = 0;

   dst = (uint8_t *)addr;
   for (jj = 3; jj < dstHeight-3; jj++)
   {
       for (ii = 3; ii < dstWidth-3; ii++)
       {
           if (ref[jj * dstStride + ii] != dst[jj * dstStride + ii])
           {
               bitexactErrors++;
           }
       }
   }

   LOG_QTVM("%s: Number of bit-exact errors: %d\n", __func__, bitexactErrors);
   return bitexactErrors;
}
/*
 * This function will setup a memory pool and allocate TA's input and output buffers.
 * These buffers will then be send to the DSP module using DSP module interface run() along
 * with the module operation.
 *
 * Args
 *   @dsp_module_session_handle    : DSP module handle
 *   @my_opener                    : Handle used to open sessions on QTVM's services
 *   @operation                    : Client operation to run on DSP module
 *
 * Return : 0 on on success.
 */
static int32_t run_cmd_on_DSP_service(SecureDspModule *dsp_module_session_handle, ITEnv *my_opener, module_op operation) {
   int32_t nErr = Object_OK;
   uint32_t ii = 0, total_buff_size = 0;
   memory_pool_service my_pool_service = {};
   user_memory input_buffer[MAX_INPUT_BUFFERS] = {{}};
   user_memory output_buffer[MAX_OUTPUT_BUFFERS] = {{}};
   uint32_t srcWidth = CLoadalgo_TA::WIDTH;
   uint32_t srcHeight = CLoadalgo_TA::HEIGHT;
   uint32_t srcStride = srcWidth;    // keep aligned to 128 bytes!
   uint32_t dstWidth = CLoadalgo_TA::WIDTH;
   uint32_t dstHeight = CLoadalgo_TA::HEIGHT;
   uint32_t dstStride = srcStride;
   int srcSize = srcStride * srcHeight;
   int dstSize = dstStride * dstHeight;
   uint8_t *ref = nullptr;

   memset(&my_pool_service, 0, sizeof(my_pool_service));
   memset(&input_buffer, 0, sizeof(input_buffer));
   memset(&output_buffer, 0, sizeof(input_buffer));

   input_buffer[0].size = srcSize;
   input_buffer[1].size = sizeof(uint32_t);
   input_buffer[2].size = sizeof(uint32_t);
   input_buffer[3].size = sizeof(uint32_t);
   for (ii = 0; ii < MAX_INPUT_BUFFERS; ++ii)
      total_buff_size += input_buffer[ii].size;

   output_buffer[0].size = dstSize;
   output_buffer[1].size = sizeof(uint32_t);

/*
 * Dummy output buffers. Are not needed for test case but DSP service is expecting
 * all 8 buffers allocated.
 */
   output_buffer[2].size = sizeof(uint32_t);
   output_buffer[3].size = sizeof(uint32_t);

   for (ii = 0; ii < MAX_OUTPUT_BUFFERS; ++ii)
      total_buff_size += output_buffer[ii].size;

   my_pool_service.pool_mem_size = total_buff_size;
   if (Object_OK != (nErr = setup_mem_service_pool(&my_pool_service, my_opener)))
      goto bail;

   if (SUCCESS != (nErr = allocate_user_buffer(my_pool_service.user_pool, input_buffer, MAX_INPUT_BUFFERS)))
      goto bail;

   LOG_QTVM("%s: allocated all %u input user buffers\n", __func__, MAX_INPUT_BUFFERS);

   if (SUCCESS != (nErr = allocate_user_buffer(my_pool_service.user_pool, output_buffer, MAX_OUTPUT_BUFFERS)))
      goto bail;

   LOG_QTVM("%s: allocated all %u output user buffers\n", __func__, MAX_OUTPUT_BUFFERS);

   if (SUCCESS != (nErr = init_buffers(input_buffer, srcHeight, srcStride, srcWidth, output_buffer, dstStride, &ref)))
      goto bail;

   LOG_QTVM("%s: initialized all input/output buffers and reference output buffer\n", __func__);

   if (Object_OK != (nErr = dsp_module_session_handle->run((uint32_t)operation, *input_buffer[0].proxy_buffer, input_buffer[0].size, *input_buffer[1].proxy_buffer, input_buffer[1].size,
                                                      *input_buffer[2].proxy_buffer, input_buffer[2].size, *input_buffer[3].proxy_buffer, input_buffer[3].size,
                                                      *output_buffer[0].proxy_buffer, output_buffer[0].size, *output_buffer[1].proxy_buffer, output_buffer[1].size,
                                                      *output_buffer[2].proxy_buffer, output_buffer[2].size, *output_buffer[3].proxy_buffer, output_buffer[3].size))) {
      LOG_QTVM("Error 0x%x(%d): %s: run operation %u failed on DSP service for module handle %p\n", nErr, nErr, __func__, operation, dsp_module_session_handle);
      goto bail;
   }
   LOG_QTVM("%s: operation %u executed successfully on DSP service for module handle %p\n", __func__, operation, dsp_module_session_handle);


   nErr = match_result((uint8_t *)output_buffer[0].va, dstHeight, dstWidth, dstStride, ref);
   if (nErr == 0) {
      nErr = SUCCESS;
      LOG_QTVM("%s: ********Result match********\n", __func__);
   } else {
      nErr = -1;
      LOG_QTVM("%s: Result mismatch: error %d\n", __func__, nErr);
      goto bail;
   }

bail:
   release_user_buffer(input_buffer, MAX_INPUT_BUFFERS);
   release_user_buffer(output_buffer, MAX_OUTPUT_BUFFERS);
   release_mem_service_pool(&my_pool_service);
   FREEIF(ref);
   if (nErr)
      LOG_QTVM("Error 0x%x(%d): %s failed operation %u for module handle %p\n", nErr, nErr, __func__, operation, dsp_module_session_handle);
   return nErr;
}

static int32_t enable_runtime_farf_logs(SecureDspSession *dsp_service_session_handle, const IMemObject *farf_buffer) {
   int32_t nErr = 0;

   if (Object_OK != (nErr = dsp_service_session_handle->enableRuntimeFARF(*farf_buffer)))
      goto bail;

   LOG_QTVM("%s: Enabled runtime farf logs\n", __func__);
bail:
   if (nErr)
      LOG_QTVM("Warning: %s: failed to setup runtime farf, error 0x%x(%d)\n", __func__, nErr, nErr);

   return nErr;
}

/*
 * Allocate heap and pool memories on secure PD using DSP service.
 *
 * Args
 *   @dsp_service_session_handle   : DSP service handle
 *   @heapSize                     : Heap size requested on secure PD
 *   @codeSize                     : Pool size requested on secure PD
 *
 * Return : 0 on on success.
 */
static int32_t alloc_memory_in_dsp_session(SecureDspSession *dsp_service_session_handle, uint32_t heapSize, uint32_t codeSize) {
   int32_t nErr = 0;

   if (dsp_service_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }
   if (Object_OK != (nErr = dsp_service_session_handle->allocMemory(heapSize, codeSize)))
      goto bail;

   LOG_QTVM("%s: allocated heap (%u bytes) and pool (%u bytes) in securePD\n", __func__, heapSize, codeSize);
bail:
   if (nErr) {
      LOG_QTVM("Error 0x%x(%d): %s: failed for heap %u, pool %u\n", nErr, nErr, __func__, heapSize, codeSize);
   }

   return nErr;
}

/*
 * Create a DSP service session by creating a DSP service handle and then opening a
 * secure PD session through DSP service.
 *
 * Args
 *   @session_proxy   : Reference object when creating DSP service session
 *   @my_opener       : Handle used to open sessions on QTVM's services
 *
 * Return : SecureDspSession handle on sucess, nullptr on failure.
 */
static SecureDspSession* create_dsp_service_session(ProxyBase **session_proxy, ITEnv *my_opener, IMemObject *farf_buffer) {
   int32_t nErr = 0;
   SecureDspSession *dsp_service_session_handle = nullptr;
   ProxyBase *session_ref = nullptr;
   int32_t domain = DEFAULT_DOMAIN_ID;

   session_ref = new ProxyBase();
   if (session_ref == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }

   nErr = my_opener->open(CSecureDSPService_UID, *session_ref);
   if (Object_OK != nErr) {
      LOG_QTVM("Error 0x%x(%d): %s: failed to open session to DSP service (uid %d)\n", nErr, nErr, __func__, CSecureDSPService_UID);
      goto bail;
   }

   dsp_service_session_handle = new SecureDspSession(session_ref->get());
   if (dsp_service_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }
   LOG_QTVM("%s: DSP service (uid %d) connection established with session handle %p\n", __func__, CSecureDSPService_UID, dsp_service_session_handle);

   if (!farf_buffer->isNull())
      enable_runtime_farf_logs(dsp_service_session_handle, farf_buffer);

   nErr = dsp_service_session_handle->openSession(domain);
   if (Object_OK != nErr) {
      LOG_QTVM("Error 0x%x(%d): %s: failed to open securePD session to domain %d\n", nErr, nErr, __func__, domain);
      goto bail;
   }
   LOG_QTVM("%s: securePD session created for domain %d\n", __func__, domain);
   *session_proxy = session_ref;
bail:
   if (nErr) {
      LOG_QTVM("Error 0x%x(%d): %s: failed to create DSP service session on domain %d\n", nErr, nErr, __func__, domain);
      DELETEIF(dsp_service_session_handle);
      DELETEIF(session_ref);
   }

   return dsp_service_session_handle;
}


/*
 * Destroy and release DSP service session.
 *
 * Args
 *   @dsp_service_session_handle  : DSP service session handle.
 *   @session_proxy               : Reference object when creating DSP service session.
 */
static void destroy_dsp_service_session(SecureDspSession *dsp_service_session_handle, ProxyBase *session_proxy) {
   LOG_QTVM("%s: destroying DSP service session %p\n", __func__, dsp_service_session_handle);
   DELETEIF(session_proxy);
   DELETEIF(dsp_service_session_handle);
   return;
}
/*
 * Loadalgo_TA constructor
 */
CLoadalgo_TA::CLoadalgo_TA() {
   LOG_QTVM("%s: Creating TA service session %p\n", __func__, this);
}

/*
 * Loadalgo_TA destructor
 */
CLoadalgo_TA::~CLoadalgo_TA() {
   LOG_QTVM("%s: Destroying TA service session %p\n", __func__, this);
}

/*
 * Main interface for client to communicate with Loadalgo_TA service.
 * The client will send algo shared object and this function will start a session with DSP service.
 * Once session is initialized, it will create heap and pool memories in secure PD through DSP service.
 * Then it will proceed to run the command from the client and finally it will close the session
 * with the DSP service.
 *
 * Args
 *   @soFile_ref     : Loadalgo shared object encapsulated on a ProxyBase object.
 *   @soFileSize_val : Size of shared object.
 *   @moduleName_ptr : Filename of shared object.
 *   @moduleName_len : Length of shared object's filename.
 *   @heapSize_val   : Heap size for secure PD.
 *   @poolSize_val   : Pool size for secure PD.
 *
 * Return : 0 on on success.
 */
int32_t CLoadalgo_TA::run_algo(const ProxyBase &soFile_ref, uint32_t soFileSize_val, const uint8_t* moduleName_ptr, size_t moduleName_len, uint32_t heapSize_val, uint32_t poolSize_val) {
   int32_t nErr = SUCCESS, ret = SUCCESS;
   ProxyBase *session_proxy = nullptr;
   ITEnv *my_opener = nullptr;
   SecureDspSession* dsp_service_session_handle = nullptr;
   SecureDspModule* dsp_module_session_handle = nullptr;
   module_op operation = GAUSSIAN;

   if (!heapSize_val || !poolSize_val) {
      nErr = EBADSIZE;
      LOG_QTVM("Error %d: %s: heap size %u or pool size %u are invalid\n", nErr, __func__, heapSize_val, poolSize_val);
      goto bail;
   }
   my_opener = new ITEnv(gTVMEnv);
   if (my_opener == nullptr) {
      nErr = EMEMPTR;
      LOG_QTVM("Error 0x%x: %s: failed to open QTVM gTVMEnv\n", nErr, __func__);
      goto bail;
   }

   dsp_service_session_handle = create_dsp_service_session(&session_proxy, my_opener, &this->farf_buffer);
   if (dsp_service_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }

   nErr = alloc_memory_in_dsp_session(dsp_service_session_handle, heapSize_val, poolSize_val);
   if (nErr) {
      goto bail;
   }

   dsp_module_session_handle = load_module_in_dsp_session(dsp_service_session_handle, &soFile_ref, soFileSize_val, moduleName_ptr, moduleName_len);
   if (dsp_module_session_handle == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }

   nErr = run_cmd_on_DSP_service(dsp_module_session_handle, my_opener, operation);
   if (nErr)
      goto bail;

bail:
   ret = close_module_in_dsp_session(dsp_service_session_handle, dsp_module_session_handle);
   if (!nErr)
      nErr = ret;
   destroy_dsp_service_session(dsp_service_session_handle, session_proxy);
   DELETEIF(dsp_module_session_handle);
   DELETEIF(my_opener);

   if (nErr) {
      LOG_QTVM("Error 0x%x(%d): %s: failed load/run operation %u for module interface %s\n", nErr, nErr, __func__, operation, moduleName_ptr);
   }
   return nErr;
}

int32_t CLoadalgo_TA::enable_runtime_farf(const IMemObject &farfMask_ref) {
   int32_t nErr = SUCCESS;

   if (Object_isNull(farfMask_ref.get())) {
      nErr = EBADOBJ;
      goto bail;
   }
   this->farf_buffer = farfMask_ref;
bail:
   if (nErr)
      LOG_QTVM("Error %d: %s: failed to obtain runtime farf mask Oject\n");

   return nErr;
}

/*
 * Called by minkplatform whenever clients want to open a new session with
 * Loadalgo_TA service. This function creates an object from class CLoadalgo_TA and it encapsulates
 * object into a mink Object and returns it to Mink.
 *
 * Args
 *   @uid    : The unique ID of the requested service.
 *   @cred   : The ICredentials object of the caller.
 *   @objOut : The service object
 *
 * Return    : 0 on success
 */
int32_t tProcessOpen(uint32_t uid, Object cred, Object *objOut) {
   CLoadalgo_TA *TA_session_obj = nullptr;
   int32_t nErr = SUCCESS;

   // Verify we are receiving correct request for this TA
   if (CLoadAlgoTA_UID != uid) {
      nErr = ENOSUCH;
      goto bail;
   }
   if (objOut == nullptr) {
      nErr = EMEMPTR;
      goto bail;
   }
   TA_session_obj = new CLoadalgo_TA();
   if (TA_session_obj == nullptr) {
      nErr = ENOMEMORY;
      goto bail;
   }
   *objOut = (Object){ImplBase::invoke, TA_session_obj};
   LOG_QTVM("%s: TA service session created. TA service session handle %p, service uid %d\n", __func__, TA_session_obj, uid);
bail:
   if (nErr)
      LOG_QTVM("Error 0x%x: %s failed to create session for uid %u, output object %p, TA session object %p\n", nErr, __func__, uid, objOut, TA_session_obj);
   return nErr;
}

/*
 * Called by mink platform when last session is detroyed on this service.
 * It will release session semaphore which will wakeup main thread and exit the TA.
 */
void tProcessShutdown(void) {
   // Increment (unlock) semaphore. Allow main thread to complete.
   sem_post(&service_shutdown_sem);
   LOG_QTVM("%s: posted on service semaphore. Service will exit\n", __func__);
}

/*
 * Loadalgo_TA main function when service is launched. It will initialize
 * service semaphore and then it will wait on it. The semaphore will be
 * released by tProcessShutdown() when the last session on this TA is destroyed.
 *
 * Args
 *  @argc : Number of arguments
 *  @argv : Argument's list
 *
 * Return : 0 on success
 */
int32_t main(int32_t argc, char *argv[]) {
   int32_t nErr = 0;

   LOG_QTVM("%s: starting TA service (%u) with pid %u\n", __func__, CLoadAlgoTA_UID, getpid());
   if (sem_init(&service_shutdown_sem, 0, 0) != 0) {
      LOG_QTVM("%s: failed to initialize service semaphore\n", __func__);
      nErr = -1;
      goto bail;
   }

   // Decrement (lock) the semaphore. Put to sleep indefinitely.
   LOG_QTVM("%s: service initialized. Waiting for requests\n", __func__);
   if (sem_wait(&service_shutdown_sem) != 0) {
      LOG_QTVM("%s: failed to wait on service semaphore\n", __func__);
      nErr = -1;
      goto bail;
   }
bail:
   LOG_QTVM("%s: service exited\n", __func__);
   return nErr;
}
