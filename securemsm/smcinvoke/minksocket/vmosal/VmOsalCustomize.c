/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "VmOsal.h"
#include "VmOsalMem.h"

/*This file needs to be updated by OEMs in order to run this library on a different OS*/

int vm_osal_mutex_init(void* mutex, void* attr)
{
  return 0;
}

int vm_osal_mutex_deinit(void* mutex)
{
  return 0;
}

int vm_osal_mutex_lock(void* mutex)
{
  return 0;
}

int vm_osal_mutex_unlock(void* mutex)
{
  return 0;
}

int vm_osal_cond_init(void* signal_object, void* attr)
{
  return 0;
}

/** No deinit for wait queue */
int vm_osal_cond_deinit(void* signal_object)
{
  return 0;
}

int vm_osal_cond_wait(void* cond_wait, void* mutex, int *condition)
{
  return 0;
}

int vm_osal_cond_set(void* sig)
{
  return 0;
}

int vm_osal_cond_broadcast(void* sig)
{
  return 0;
}

struct vm_osal_thread_ctxt {
  void* (*fn)(void *);
  void *data;
};

int vm_osal_thead_fn(void *data)
{
  return 0;
}

int vm_osal_thread_create(vm_osal_thread *tid, void* (*fn)(void *data), void* data, char *thread_name)
{
  return 0;
}

int vm_osal_thread_detach(vm_osal_thread tid)
{
  return 0;
}

int vm_osal_thread_self(void)
{
  return 0;
}

/** No equivalent of pthread_join in kernel - use STOP on the thread to make sure it is exited */
int vm_osal_thread_join(vm_osal_thread tid, void **retVal)
{
  return 0;
}

/**
 * Thread specific key store/restore is not needed for UDP/across VMs
 * This implementation should [TID][KEY] = DATA, retrieve the same way
 * TID is thread ID which has to be obtained from caller context
 */
int vm_osal_create_TLS_key(VM_KEY_TYPE *key, void (*destructor)(void*))
{
  return 0;
}

int vm_osal_store_TLS_key(VM_KEY_TYPE key, const void *value)
{
  return 0;
}

void *vm_osal_retrieve_TLS_key(VM_KEY_TYPE key)
{
  return NULL;
}

int vm_osal_socket_close(int sockFd)
{
  return 0;
}

int vm_osal_fd_close(int ufd)
{
  return 0;
}

int vm_osal_mem_close(int memFd)
{
  return 0;
}

int vm_osal_atomic_add(int *ptr, int number)
{
  return 0;
}

int vm_osal_fd_dup(int oldfd)
{
  return 0;
}

int vm_osal_poll(void* pollfds, int nfds, int timeout)
{
  return 0;
}

vm_osal_vmMem *vm_osal_createVmMem(void)
{
  return NULL;
}

/**
 * instance: unused
 */
void vm_osal_freeVmMem(vm_osal_vmMem *instance)
{
}

/**
 * instance: unused
 * cstr: unused
 */
vm_osal_vmHandle vm_osal_findVmByName(vm_osal_vmMem *instance, char *cstr)
{
  return 0;
}

/**
 * instance: unused
 * dmaBufFd: membuf fd
 * handles: unused
 * perms: unused
 * nr: unused
 * memparcelHandle: memparcel handle
 */
int vm_osal_lendDmabufHandle(vm_osal_vmMem *instance, int dmaBufFd,
                             vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                             int64_t *memparcelHandle)
{
  return 0;
}

/**
 * instance: unused
 * dmaBufFd: membuf fd
 * handles: unused
 * perms: unused
 * nr: unused
 * memparcelHandle: memparcel handle
 */
int vm_osal_shareDmabufHandle(vm_osal_vmMem *instance, int dmaBufFd,
                             vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                             int64_t *memparcelHandle)
{
  return 0;
}

/**
 * instance: unused
 * owner: unused
 * handles: unused
 * perms: unused
 * nr: unused
 * memparcelHandle: memparcel handle
 */
int vm_osal_retrieveDmabuf(vm_osal_vmMem *instance, vm_osal_vmHandle owner,
                           vm_osal_vmHandle *handles, uint32_t *perms, int nr,
                           int64_t memparcelHandle)
{
  return 0;
}

/**
 * instance: unused
 * dmaBufFd: membuf fd
 * memparcelHandle: memparcel handle
 */
int vm_osal_reclaimDmabuf(vm_osal_vmMem *instance, int dmaBufFd,
                          int64_t memparcelHandle)
{
  return 0;
}

int vm_osal_pipe(int pipefd[2])
{
  return 0;
}

void vm_osal_sleep(unsigned int time_in_sec)
{
}

int vm_osal_getsockopt(int fd, int level, int optname,
                       void *optval, socklen_t *optlen)
{
  return 0;
}

vm_osal_pid vm_osal_getPid(void)
{
  return 0;
}

vm_osal_pid vm_osal_getTid(void)
{
  return 0;
}
