/********************************************************************
Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Not a Contribution
*********************************************************************/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "BufferAllocatorWrapper.h"
#include "MinkTransportUtils.h"
#include "vmmem_wrapper.h"

#define MEMSTRLEN 20
static pthread_mutex_t gMemLock = PTHREAD_MUTEX_INITIALIZER;

// Counter to initialise memory object identifier for shm
static int32_t gMemStr = 0;

// Max number of shared memory objects
enum { listMapCapacity = 1000 };

// key value pair to store fd and memory object identifier mapping
struct key_value {
    int32_t key;
    int32_t value;
};

struct listMap {
    struct key_value kvPairs[listMapCapacity];
    size_t count;
};

static pthread_mutex_t gMyMapLock = PTHREAD_MUTEX_INITIALIZER;
struct listMap *gMyMap = NULL;

// Create new list
static void listMapCreate(struct listMap **mp)
{
    *mp = (struct listMap *)malloc(sizeof(struct listMap));
    if ((*mp) == NULL) {
        return;
    }

    (*mp)->count = 0;
}

// Insert new element into the list. Initialise the list for first entry.
static int32_t listMapInsert(struct listMap **mp, int32_t key, int32_t value)
{
    // Initialize new list
    if (*mp == NULL) {
        listMapCreate(mp);
        if ((*mp) == NULL) {
            return -1;
        }
    }

    if (((*mp)->count == listMapCapacity) || (key <= 0) || (value <= 0)) {
        return -1;
    }

    (*mp)->kvPairs[(*mp)->count].key = key;
    (*mp)->kvPairs[(*mp)->count].value = value;
    (*mp)->count++;
    return 0;
}

// Retrieve memory object identifier for a particular fd
static int32_t listMapValueForKey(struct listMap *mp, int32_t key)
{
    int32_t ret = -1;
    if ((mp == NULL) || (mp->count == 0)) {
        return ret;
    }

    if (key <= 0) {
        return ret;
    }

    for (size_t i = 0; i < mp->count; ++i) {
        if (mp->kvPairs[i].key == key) {
            ret = mp->kvPairs[i].value;
            break;
        }
    }

    return ret;
}

// Free complete mapping list
static void listMapDestroy(struct listMap *mp)
{
    if (mp == NULL) {
        return;
    }

    for (size_t i = 0; i < mp->count; ++i) {
        mp->kvPairs[i].key = 0;
        mp->kvPairs[i].value = 0;
    }

    free(mp);
}

// Remove a particular entry from the list
static int32_t listMapDeleteKVPair(struct listMap *mp, int32_t key)
{
    int32_t ret = -1;
    if ((mp == NULL) || (mp->count == 0)) {
        return ret;
    }

    if (key <= 0) {
        return ret;
    }

    for (size_t i = 0; i < mp->count; ++i) {
        if (mp->kvPairs[i].key == key) {
            mp->kvPairs[i].key = 0;
            mp->kvPairs[i].value = 0;
        }
    }

    return 0;
}

// Opens a new shared memory object based on the static counter
// and inserts the new entry into the mapping.
int32_t DmabufHeapAlloc(BufferAllocator *buffer_allocator, const char *heap_name, size_t len,
                        unsigned int heap_flags, size_t legacy_align)
{
    (void)buffer_allocator;
    (void)heap_name;
    (void)len;
    (void)heap_flags;
    (void)legacy_align;

    pthread_mutex_lock(&gMemLock);
    char shmpath[MEMSTRLEN];
    int32_t shm_fd;
    int32_t ret = -1;
    snprintf(shmpath, MEMSTRLEN, "tvmd_%d", gMemStr + getpid());
    shm_fd = shm_open(shmpath, O_CREAT | O_RDWR, 0777);
    if (shm_fd == -1) {
        pthread_mutex_unlock(&gMemLock);
        return -1;
    }

    ret = ftruncate(shm_fd, len);
    if (ret) {
        pthread_mutex_unlock(&gMemLock);
        return ret;
    }

    pthread_mutex_lock(&gMyMapLock);
    ret = listMapInsert(&gMyMap, shm_fd, gMemStr + getpid());
    pthread_mutex_unlock(&gMyMapLock);
    if (ret) {
        pthread_mutex_unlock(&gMemLock);
        return ret;
    }

    gMemStr++;
    LOG_MSG("returning shm_fd = %d, shmpath = %s\n", shm_fd, shmpath);
    pthread_mutex_unlock(&gMemLock);

    return shm_fd;
}

// Completely stubbed
VmMem *CreateVmMem()
{
    return (VmMem *)&gMemStr;
}

// Completely stubbed
void FreeVmMem(VmMem *instance)
{
    (void)instance;
}

// Completely stubbed
VmHandle FindVmByName(VmMem *instance, char *cstr)
{
    (void)instance;
    (void)cstr;

    return 1;
}

// Share the memory object using memparcel_hdl
int32_t ShareDmabufHandle(VmMem *instance, int32_t dma_buf_fd, VmHandle *handles, uint32_t *perms,
                          int32_t nr, int64_t *memparcel_hdl)
{
    (void)instance;
    (void)handles;
    (void)perms;
    (void)nr;

    pthread_mutex_lock(&gMyMapLock);
    if (dma_buf_fd == -1) {
        pthread_mutex_unlock(&gMyMapLock);
        return -1;
    } else {
        *memparcel_hdl = listMapValueForKey(gMyMap, dma_buf_fd);
        pthread_mutex_unlock(&gMyMapLock);
    }

    return 0;
}

// Lend the memory object using memparcel_hdl
int32_t LendDmabufHandle(VmMem *instance, int32_t dma_buf_fd, VmHandle *handles, uint32_t *perms,
                         int32_t nr, int64_t *memparcel_hdl)
{
    (void)instance;
    (void)handles;
    (void)perms;
    (void)nr;

    pthread_mutex_lock(&gMyMapLock);
    if (dma_buf_fd == -1) {
        pthread_mutex_unlock(&gMyMapLock);
        return -1;
    } else {
        *memparcel_hdl = listMapValueForKey(gMyMap, dma_buf_fd);
        pthread_mutex_unlock(&gMyMapLock);
    }

    return 0;
}

// Retrieve the shared memory object passed in memparcel_hdl.
int32_t RetrieveDmabuf(VmMem *instance, VmHandle owner, VmHandle *handles, uint32_t *perms,
                       int32_t nr, int64_t memparcel_hdl)
{
    (void)instance;
    (void)owner;
    (void)handles;
    (void)nr;

    char shmpath[MEMSTRLEN];
    int32_t bufKey = 0;
    (void)bufKey;
    snprintf(shmpath, MEMSTRLEN, "%ld", memparcel_hdl);
    int32_t flags;
    int32_t ret = -1;
    if ((perms[1] | VMMEM_READ) && (perms[1] | VMMEM_WRITE)) {
        flags = O_RDWR;
    } else {
        flags = O_RDONLY;
    }

    pthread_mutex_lock(&gMyMapLock);
    if (gMyMap != NULL) {
        for (size_t i = 0; i < gMyMap->count; i++) {
            if (gMyMap->kvPairs[i].value == memparcel_hdl) {
                bufKey = gMyMap->kvPairs[i].key;
                pthread_mutex_unlock(&gMyMapLock);
                return bufKey;
            }
        }
    }

    int32_t shm_fd = shm_open(shmpath, flags, 0);
    if (shm_fd == -1) {
        pthread_mutex_unlock(&gMyMapLock);
        return -1;
    }

    ret = listMapInsert(&gMyMap, shm_fd, memparcel_hdl);
    pthread_mutex_unlock(&gMyMapLock);
    if (ret) {
        return -1;
    }

    return shm_fd;
}

// Completely stubbed
int32_t ReclaimDmabuf(VmMem *instance, int32_t dma_buf_fd, int64_t memparcel_hdl)
{
    (void)instance;
    (void)dma_buf_fd;
    (void)memparcel_hdl;

    return 0;
}

// Close the fd and unlink the shared memory object.
// Alternate implementation used to replace linux based close(fd)
// to help unlink shm based memory initialised for offtarget testing.
int32_t close_offtarget(int32_t fd, int32_t *shmpath_id)
{
    char shmpath[MEMSTRLEN];
    size_t i;
    pthread_mutex_lock(&gMyMapLock);
    *shmpath_id = listMapValueForKey(gMyMap, fd);
    if (*shmpath_id < 0) {
        pthread_mutex_unlock(&gMyMapLock);
        return -1;
    }

    snprintf(shmpath, MEMSTRLEN, "tvmd_%d", *shmpath_id);
    listMapDeleteKVPair(gMyMap, fd);
    if (gMyMap != NULL) {
        for (i = 0; i < gMyMap->count; i++) {
            if (gMyMap->kvPairs[i].key != 0) {
                break;
            }
        }
        // Destroy the listMap if all key value pairs are 0.
        if (i == gMyMap->count) {
            listMapDestroy(gMyMap);
            gMyMap = NULL;
        }
    }

    close(fd);
    pthread_mutex_unlock(&gMyMapLock);

    return 0;
}

int32_t close_offtarget_unlink(int32_t fd)
{
    char shmpath[MEMSTRLEN];
    int32_t shmpath_id;
    int32_t ret = -1;

    ret = close_offtarget(fd, &shmpath_id);
    if (ret) {
        return -1;
    }

    snprintf(shmpath, MEMSTRLEN, "tvmd_%d", shmpath_id);

    ret = shm_unlink(shmpath);

    return ret;
}

int32_t RefreshMemFd(int32_t fd, mode_t mode)
{
    char shmpath[MEMSTRLEN];
    int32_t shm_fd;
    int32_t ret = -1;
    int32_t shmpath_id;

    pthread_mutex_lock(&gMemLock);

    ret = close_offtarget(fd, &shmpath_id);
    if (ret) {
        pthread_mutex_unlock(&gMemLock);
        return ret;
    }

    snprintf(shmpath, MEMSTRLEN, "tvmd_%d", shmpath_id);
    LOG_MSG("shmpath = %s\n", shmpath);
    shm_fd = shm_open(shmpath, mode, 0);
    if (shm_fd == -1) {
        pthread_mutex_unlock(&gMemLock);
        return ret;
    }

    pthread_mutex_lock(&gMyMapLock);
    ret = listMapInsert(&gMyMap, shm_fd, shmpath_id);
    pthread_mutex_unlock(&gMyMapLock);
    if (ret) {
        pthread_mutex_unlock(&gMemLock);
        return ret;
    }

    LOG_MSG("refreshing shm_fd = %d", shm_fd);

    pthread_mutex_unlock(&gMemLock);

    return shm_fd;
}

// Completely stubbed
BufferAllocator *CreateDmabufHeapBufferAllocator(void)
{
    return (BufferAllocator *)&gMemStr;
}

// Completely stubbed
void FreeDmabufHeapBufferAllocator(BufferAllocator *bufferAllocator)
{
    (void)bufferAllocator;
}

int32_t RemoteAllocDmabuf(VmMem *instance, uint64_t size, VmHandle *handles, uint32_t *perms,
                          int32_t nr, char *c_src_dma_heap_name, char *c_dst_dma_heap_name)
{
    (void)instance;
    (void)size;
    (void)handles;
    (void)perms;
    (void)nr;
    (void)c_src_dma_heap_name;
    (void)c_dst_dma_heap_name;
    return 1;  // poolFd
}
