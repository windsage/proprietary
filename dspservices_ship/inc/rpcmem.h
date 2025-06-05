/**
 * Copyright (c) 2012-2016, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */


#ifndef RPCMEM_H
#define RPCMEM_H

#include "AEEStdDef.h"

/**
 * RPCMEM_DEFAULT_HEAP
 * Dynamicaly select the heap to use.  This should be ok for most usecases.
 */
#define RPCMEM_DEFAULT_HEAP -1

/**
 * RPCMEM_DEFAULT_FLAGS should allocate memory with the same properties
 * as the ION_FLAG_CACHED flag
 */
#ifdef ION_FLAG_CACHED
#define RPCMEM_DEFAULT_FLAGS ION_FLAG_CACHED
#else
#define RPCMEM_DEFAULT_FLAGS 1
#endif

/**
 * ION_FLAG_CACHED should be defined as 1
 */
#define RPCMEM_FLAG_UNCACHED 0
#define RPCMEM_FLAG_CACHED RPCMEM_DEFAULT_FLAGS

/**
 * RPCMEM HEAP IDs
 * RPCMEM_HEAP_ID_SYSTEM:
 *   - non-contiguous physical memory
 *   - for sub-systems with SMMU
 *   - recommended for most usecases
 *
 * RPCMEM_HEAP_ID_CONTIG:
 *   - Contiguous physical memory
 *   - very limited memory available (< 8 MB)
 *   - use only for sub-systems without SMMU
 *
 * RPCMEM_HEAP_ID_SECURE:
 *   - Secure memory
 *   - use for secure usecases only
 *
 * If you are not using any of the RPCMEM defined heap IDs,
 * then it is your responsibility to ensure that you are passing
 * a valid ION heap ID.
 */
enum rpc_heap_ids {
	RPCMEM_HEAP_ID_SECURE	= 9,
	RPCMEM_HEAP_ID_CONTIG	= 22,
	RPCMEM_HEAP_ID_SYSTEM	= 25,
};

/**
 * examples:
 *
 * contiguous heap, uncached, 1kb
 *    rpcmem_alloc(RPCMEM_HEAP_ID_CONTIG, RPCMEM_FLAG_UNCACHED, 1024);
 *
 * system heap, cached, 1kb
 *    rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_FLAG_CACHED, 1024);
 *
 * just give me the defaults, 2kb
 *    rpcmem_alloc(RPCMEM_DEFAULT_HEAP, RPCMEM_DEFAULT_FLAGS, 2048);
 *
 * give me the default flags, but from heap 18, 4kb
 *    rpcmem_alloc(18, RPCMEM_DEFAULT_FLAGS, 4096);
 *
 */

// Deprecated flag. Please use macros from libion
#define ION_SECURE_FLAGS    ((1 << 31) | (1 << 19))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * call once to initialize the library
 * Note: should not call this if rpcmem is linked from libadsprpc.so
 * /libcdsprpc.so/libmdsprpc.so/libsdsprpc.so
 */
void rpcmem_init(void);

/**
 * call once for cleanup
 * Note: should not call this if rpcmem is linked from libadsprpc.so
 * /libcdsprpc.so/libmdsprpc.so/libsdsprpc.so
 */
void rpcmem_deinit(void);

/**
 * Allocate a zero-copy buffer for size upto 2 GB with the FastRPC framework.
 * Recommendation is to always use rpcmem_alloc2 if the api is available for
 * anysize <= 2 power(8*sizeof(size_t))
 * @param[in] heapid  Heap ID to use for memory allocation.
 * @param[in] flags   ION flags to use for memory allocation.
 * @param[in] size    Buffer size to allocate.
 * @return            Pointer to the buffer on success; NULL on failure.
 *
 * Examples:
 *
 * * Default memory attributes, 2 KB
 * @code
 *    rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 2048);
 * @endcode
 * Or
 * @code
 *    rpcmem_alloc_def(2048);
 * @endcode
 *
 * * Heap 22, uncached, 1 KB
 * @code
 *    rpcmem_alloc(22, 0, 1024);
 * @endcode
 * Or
 * @code
 *    rpcmem_alloc(22, RPCMEM_FLAG_UNCACHED, 1024);
 * @endcode
 *
 * * Heap 21, cached, 2 KB
 * @code
 *    rpcmem_alloc(21, RPCMEM_FLAG_CACHED, 2048);
 * @endcode
 * Or
 * @code
 *    #include <ion.h>
 *    rpcmem_alloc(21, ION_FLAG_CACHED, 2048);
 * @endcode
 *
 * * Default memory attributes but from heap 18, 4 KB
 * @code
 *    rpcmem_alloc(18, RPCMEM_DEFAULT_FLAGS, 4096);
 * @endcode
 */
void* rpcmem_alloc(int heapid, uint32 flags, int size);

/**
 * Allocate a zero-copy buffer with the FastRPC framework.
 * Some versions of libcdsprpc.so lack this function,
 * so users should set this symbol as weak.
 * @param[in] heapid  Heap ID to use for memory allocation.
 * @param[in] flags   ION flags to use for memory allocation.
 * @param[in] size    Buffer size to allocate.
 * @return            Pointer to the buffer on success; NULL on failure.
 *
 * Examples:
 *
 * * The usage examples are same as rpcmem_alloc.
 */
void* rpcmem_alloc2(int heapid, uint32 flags, size_t size);
/**
 * allocate with default settings
 */
 #if !defined(WINNT) && !defined (_WIN32_WINNT)
__attribute__((unused))
#endif
static __inline void* rpcmem_alloc_def(int size) {
   return rpcmem_alloc(RPCMEM_DEFAULT_HEAP, RPCMEM_DEFAULT_FLAGS, size);
}

/**
 * free buffer, ignores invalid buffers
 */
void rpcmem_free(void* po);

/**
 * returns associated fd
 */
int rpcmem_to_fd(void* po);

#ifdef __cplusplus
}
#endif

#define RPCMEM_HEAP_DEFAULT     0x80000000
#define RPCMEM_HEAP_NOREG       0x40000000
#define RPCMEM_HEAP_UNCACHED    0x20000000
#define RPCMEM_HEAP_NOVA        0x10000000
#define RPCMEM_HEAP_NONCOHERENT 0x08000000
#define RPCMEM_FORCE_NOFLUSH    0x01000000
#define RPCMEM_FORCE_NOINVALIDATE    0x02000000

/**
 * Allocate and map buffer to remote DSP process and unmap on rpcmem free. Buffers allocated
 * with this flag will be mapped to all new and current fastrpc sessions. It will be unmapped
 * during either buffer free or session close.
 * Buffers allocated with this flag will be pre-mapped to DSP and reduces the FastRPC latency
 * when the buffer passed as a parameter to RPC calls. It is recommended to use this flag only for
 * allocating buffers which are used with latency sensitive RPC methods.
 */
#define RPCMEM_TRY_MAP_STATIC   0x04000000

#endif //RPCMEM_H
