/********************************************************************
 Copyright (c) 2016-2017, 2021-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/


#if defined(USE_GLIB) && !defined (__aarch64__)
/* FIXME SDX60:
   The Olympic bits/socket.h header is patched out to only include the required
   asm/socket.h if __USE_MISC is defined. Explicitly define this flag as a
   workaround for 32-bit LE targets until the root-issue can be addressed in
   the kernel. Otherwise SO_PEERCRED is not defined. */
#define __USE_MISC 1
#endif
#include <sys/socket.h>
#include <sys/un.h>
#include "bbuf.h"
#include "cdefs.h"
#include "check.h"
#include "ConnEventHandler.h"
#include "fdwrapper.h"
#include "MinkTransportUtils.h"
#include "LinkCredentials.h"
#include "LocalAdapter.h"
#include "lxcom_sock.h"
#include "memscpy.h"
#include "minksocket.h"
#include "msforwarder.h"
#include "object.h"
#include "ObjectTableMT.h"
#include "Primordial.h"
#include "RemoteAdapter.h"
#include "threadpool.h"
#include "Confinement.h"
#include "ITAccessPermissions.h"
#include "IConfinement.h"
#include "MSMem.h"
#include "RemoteShareMemory.h"
#include "ShareMemory.h"
#include "WrappedMemparcel.h"
#include "vmuuid.h"
#ifdef VNDR_SMCINVOKE_QRTR
#include "libqrtr.h"
#else
int qrtr_sendto(int sock, uint32_t node, uint32_t port, const void *data, unsigned int sz)
{
  return 0;
}
#endif
#include <linux/vm_sockets.h>

extern vm_osal_key gMinkPeerUIDTLSKey;
extern vm_osal_key gMinkPeerGIDTLSKey;

#define BEGIN_INVOKE_ID         1
#define MAX_OBJECT_COUNT        1024
#define MSG_PREALLOCED_SIZE     1024
#define MAX_BUFFER_ALLOCATION   4*1024*1024 // 4MB: Max page size in linux kernel

#ifndef SO_PEERCRED
#define SO_PEERCRED 17
#endif

#define INVALID_OBJECT_HANDLE UINT16_MAX

#define VM_SOCK_ADDR_POPULATE(data, port, cid) data.svm_family = AF_VSOCK; \
                                               data.svm_port = port;\
                                               data.svm_cid = cid;

const uint8_t vmuuidLE[VMUUID_MAX_SIZE] = {CLIENT_VMUID_TUI};

const uint8_t vmuuidOEM[VMUUID_MAX_SIZE] = {CLIENT_VMUID_OEM};

// Memparcel handles are 64 bits, arg handles are 16 bits, so we need 4
#define NUM_EXTRA_ARGS_PER_MEMPARCEL 4

// ITAccessPermissions_rules.specialRules are 64 bits
// We just need specialRules in minksocket for now
// TODO:
//   i. send whole confinement to another VM efficiently
//  ii. check if via BI is practicable
#define NUM_EXTRA_ARGS_SPECIALRULES 4

#define COUNT_NUM_EXTRA_ARGS_OUTBOUND(k, a, section)   ({          \
  size_t __numExtra = 0;                                           \
  FOR_ARGS(i, k, section) {                                        \
    int fd;                                                        \
    if ((isWrappedFd(a[i].o, &fd) ||                               \
         isMSMem(a[i].o, &fd)) &&                                  \
        is_remote_sock(me->sockType)) {                            \
      __numExtra += NUM_EXTRA_ARGS_PER_MEMPARCEL;                  \
      __numExtra += NUM_EXTRA_ARGS_SPECIALRULES;                   \
    }                                                              \
  }                                                                \
  __numExtra; })


#define COUNT_NUM_EXTRA_ARGS_INBOUND(k, a, section, start) ({      \
  size_t __numExtra = 0;                                           \
  size_t __ii = start;                                             \
  FOR_ARGS(i, k, section) {                                        \
    uint16_t flags = a[__ii].o.flags;                              \
    if (flags & LXCOM_MEMPARCEL_OBJECT) {                          \
      __numExtra += NUM_EXTRA_ARGS_PER_MEMPARCEL;                  \
      __ii += NUM_EXTRA_ARGS_PER_MEMPARCEL;                        \
      __numExtra += NUM_EXTRA_ARGS_SPECIALRULES;                   \
      __ii += NUM_EXTRA_ARGS_SPECIALRULES;                         \
    }                                                              \
    __ii++;                                                        \
  }                                                                \
  __numExtra; })


#define CHECK_MAX_ARGS(k, dir, numExtra)                           \
  do {                                                             \
    int numArgs = ObjectCounts_num##dir(k);                        \
    int numTotalArgs = numArgs + numExtraArgs;                     \
    if (numTotalArgs > LXCOM_MAX_ARGS) {                           \
      LOG_ERR("Too many args: %d (args : %d, extra args: %d)\n",   \
        (uint32_t)numTotalArgs, (uint32_t)numArgs,                 \
        (uint32_t)numExtraArgs);                                   \
      return Object_ERROR_MAXARGS;                                 \
    }                                                              \
  } while(0)

struct MinkSocket {
  int refs;
  int32_t dispatchErr;
  bool bDone;
  bool workForServer;
  ObjectTableMT table;
  int sock;
  SOCK_TYPE sockType;
  bool closeOnOTEmpty;
  int sockPair[2];
  uint32_t invoke_id;
  vm_osal_mutex mutex;
  vm_osal_cond cond;
  QList qlInvokes;
  QList qlMSForwarder;
  ThreadPool *pool;
  CredInfo credInfo;
  bool peer_available;
  int msForwarderCount;
  uint32_t node;
  uint32_t port;
  Object linkCredentials;
  Object forwarder;
  uint8_t *vmuuid;
  size_t vmuuidLen;
};

/******************************************************************
  pop out MSForwarder from qlMSForwarder of MinkSocket
  private interface, caller takes care of the mutex
*******************************************************************/
static inline MSForwarder *MinkSocket_popForwarder(MinkSocket *me)
{
  QNode *msFwd_node = QList_pop(&me->qlMSForwarder);
  if (NULL == msFwd_node)
    return NULL;

  return c_containerof(msFwd_node, MSForwarder, node);
}

/******************************************************************
  attach MSForwarder to qlMSForwarder of MinkSocket
  public interface, function itself takes care of the mutex
*******************************************************************/
int32_t MinkSocket_attachForwarder(MinkSocket *me, MSForwarder *msFwd)
{
  vm_osal_mutex_lock(&me->mutex);

  if (!me->bDone && -1 != me->sock) {
    vm_osal_atomic_add(&me->msForwarderCount, 1);
    MinkSocket_retain(msFwd->conn);
    QList_appendNode(&me->qlMSForwarder, &msFwd->node);
    vm_osal_mutex_unlock(&me->mutex);
    return Object_OK;
  }

  vm_osal_mutex_unlock(&me->mutex);
  return Object_ERROR;
}

/***********************************************************************
  detach MSForwarder from qlMSForwarder of MinkSocket
  it should be invoked after MinkSocket_close() and only can be invoked
  at the end of MinkSocket operation because the possible delete
  private interface, caller takes care of the mutex

  return Object_OK when MinkSocket instance just be released but not freed
  return Object_ERROR_UNAVAIL when MinkSocket instance be freed
************************************************************************/
static int32_t MinkSocket_detachForwarderLocked(MinkSocket *me, MSForwarder *msFwd)
{
  if (me && (me == msFwd->conn)) {
    QNode_dequeueIf(&msFwd->node);
    msFwd->conn = NULL;
    vm_osal_atomic_add(&me->msForwarderCount, -1);
    if (vm_osal_atomic_add(&me->refs, -1) == 0) {
      MinkSocket_delete(me);
      return Object_ERROR_UNAVAIL;
    }
  }

  return Object_OK;
}

/*@brief: detach specific MSForwarder from MinkSocket
 *        only can be invoked after after MinkSocket_close()
 *        public interface, function itself takes care of the mutex
 *
 *@return: Object_OK when MinkSocket instance just be released but not freed
 *         Object_ERROR_UNAVAIL when MinkSocket instance be freed
**/
int32_t MinkSocket_detachForwarder(MinkSocket *me, MSForwarder *msFwd)
{
  int32_t ret = Object_OK;

  vm_osal_mutex_lock(&me->mutex);

  ret = MinkSocket_detachForwarderLocked(me, msFwd);

  vm_osal_mutex_unlock(&me->mutex);

  return ret;
}

/*@brief: detach all MSForwarder from MinkSocket
 *        only can be invoked after after MinkSocket_close()
 *        public interface, function itself takes care of the mutex
 *
 *@return: Object_OK when MinkSocket instance just be released but not freed
 *         Object_ERROR_UNAVAIL when MinkSocket instance be freed
*/
int32_t MinkSocket_detachForwarderAll(MinkSocket *me)
{
  int32_t ret = Object_OK;
  MSForwarder *msFwd;

  vm_osal_mutex_lock(&me->mutex);

  //every msforwarder holds one refs of corrsponding minksocket
  //only the last msforwarder may lead the invocation return non-zero outcome
  //so there is not risk of overwritten on 'ret'
  while ((msFwd = MinkSocket_popForwarder(me))) {
    ret = MinkSocket_detachForwarderLocked(me, msFwd);
  }

  vm_osal_mutex_unlock(&me->mutex);

  if (ret) {
    LOG_TRACE("MinkSocket instance is freed in disorder\n");
  }
  return ret;
}

/******************************************************************
  terminate relation between MinkSockt and MSForwarder
  public interface, function itself takes care of the mutex
*******************************************************************/
void MinkSocket_preDeleteForwarder(MinkSocket *me, MSForwarder *msFwd)
{
  if (NULL == me) {
    return;
  }

  vm_osal_mutex_lock(&me->mutex);

  //promordial is not allown to be released from remote
  if ((msFwd->handle >= 0) && (PRIMORDIAL_HANDLE != msFwd->handle)) {
    MinkSocket_sendClose(me, msFwd->handle);
  }
  MSForwarder_notifyConnEvent(msFwd, EVENT_CONN_DELETE);
  MinkSocket_detachForwarderLocked(me, msFwd);

  vm_osal_mutex_unlock(&me->mutex);
}

/******************************************************************
  notify all MinkSocket affiliated handlers of the given event
  it must be invoked before MinkSocket detach operation, which
  will romove MSForwarder from MinkSocket
*******************************************************************/
void MinkSocket_notifyClose(MinkSocket *me, uint32_t event)
{
  QNode *pqn = NULL;
  QNode *pqn_next = NULL;
  MSForwarder *msFwd = NULL;

  vm_osal_mutex_lock(&me->mutex);
  QLIST_NEXTSAFE_FOR_ALL(&me->qlMSForwarder, pqn, pqn_next) {
    msFwd = c_containerof(pqn, MSForwarder, node);
    if (msFwd) {
      MSForwarder_notifyConnEvent(msFwd, event);
    }
  }

  vm_osal_mutex_unlock(&me->mutex);
}

int MinkSocket_initPrimordial(MinkSocket *me, Object pmd)
{
  return ObjectTableMT_AddPrimordial(&me->table, pmd);
}

int MinkSocket_deinitPrimordial(MinkSocket *me)
{
  return  ObjectTableMT_releasePrimordial(&me->table);
}

//return how much to add to the alignment
#define PADDED(x) ((__typeof__(x))((x) + (((uint64_t)(~(x))+1) & (LXCOM_MSG_ALIGNMENT-1))))

static bool is_remote_sock(int32_t sType) {
  return (QIPCRTR == sType || VSOCK == sType);
}

// ----------------------------------------------------------------------------
// Functions for checking VM name, needed for interacting with libvmmem
// ----------------------------------------------------------------------------

static char* nodeToVMName(uint32_t node) {
  // These names are filenames under /dev/mem_buf_vm/,
  // they are opened by libvmmem
  static char* tuiVMName = "qcom,trusted_vm";
  static char* oemVMName = "qcom,oemvm";
  static char* hlosVMName = "qcom,hlos";

  // These node values are IPC Router Processor IDs from MPROC team
  if (node == 1) {
    return hlosVMName;
  } else if (node == 20) {
    return tuiVMName;
  } else if (node == 21) {
    return oemVMName;
  }
  LOG_ERR("Node value not known: %d\n", node);
  return NULL;
}

static char* MinkSocket_getDestVMName(MinkSocket *mSock)
{

  if (mSock->sockType == QIPCRTR) {

    uint32_t node = mSock->node;

    if (is_remote_sock(mSock->sockType)) {
      return nodeToVMName(node);
    }

  } else { // VSOCK

  // For now we have to use hardcoded values.
  // This logic can be removed once VSOCK is populating node values

#ifdef ENABLE_TUI_OEM_VM
    static char* destName = "qcom,trusted_vm";
#else
    static char* destName = "qcom,oemvm";
#endif

    return destName;
  }

  return NULL;
}

// ----------------------------------------------------------------------------
// Implement WrappedMemparcel
// ----------------------------------------------------------------------------

typedef struct WrappedMemparcel {
  int refs;
  int64_t memparcelHandle;
  // It can be either FdWrapper or MSMem.
  Object wrappedFdObj;
} WrappedMemparcel;

static int32_t
WrappedMemparcel_delete(WrappedMemparcel* me)
{
  int32_t fd = -1;
  uint64_t specialRules = 0;
  Object confObj = Object_NULL;
  FdWrapper *fdw = NULL;
  MSMem *msmem = NULL;

  if (isWrappedFd(me->wrappedFdObj, &fd)) {
    fdw = FdWrapperFromObject(me->wrappedFdObj);
    if (!fdw) {
      return Object_ERROR;
    }
    confObj = fdw->confinement;
    if (Object_isNull(confObj) || (NULL == ConfinementFromObject(confObj))) {
      return Object_ERROR;
    }
    if (!Object_isOK(IConfinement_getSpecialRules(confObj, &specialRules))) {
      return Object_ERROR;
    }
  } else if (isMSMem(me->wrappedFdObj, &fd)) {
    msmem = MSMemFromObject(me->wrappedFdObj);
    if (!msmem) {
      return Object_ERROR;
    }
    specialRules = msmem->confRules.specialRules;
  } else {
    return Object_ERROR;
  }

  if (!((ITAccessPermissions_smmuProxyControlled & specialRules) ||
        (ITAccessPermissions_mixedControlled & specialRules))) {
#ifdef ANDROID
    LOG_TRACE("Reclaiming memparcelHdl = %lld and dmaBufFd = %d.\n", me->memparcelHandle, fd);
#else
    LOG_TRACE("Reclaiming memparcelHdl = %ld and dmaBufFd = %d.\n", me->memparcelHandle, fd);
#endif
    ShareMemory_ReclaimMemBuf(fd, me->memparcelHandle);
  }

  // Delete the WPM object:
  Object_ASSIGN_NULL(me->wrappedFdObj);
  heap_free(me);

  return Object_OK;
}

static int32_t
WrappedMemparcel_release(WrappedMemparcel* me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    return WrappedMemparcel_delete(me);
  }
  return Object_OK;
}

static int32_t
WrappedMemparcel_getConfinement(WrappedMemparcel* me, ITAccessPermissions_rules *outConfRules)
{
  MSMem *msmem = NULL;
  FdWrapper *fdw = NULL;
  int32_t fd = -1;
  Object confObj = Object_NULL;

  if (isWrappedFd(me->wrappedFdObj, &fd)) {
    fdw = FdWrapperFromObject(me->wrappedFdObj);
    if (!fdw) {
      return Object_ERROR;
    }
    confObj = fdw->confinement;
    if (Object_isNull(confObj) || (NULL == ConfinementFromObject(confObj))) {
      return Object_ERROR;
    }
    return IConfinement_getConfinementRules(confObj, outConfRules);
  } else if (isMSMem(me->wrappedFdObj, &fd)) {
    msmem = MSMemFromObject(me->wrappedFdObj);
    if (!msmem) {
      return Object_ERROR;
    }
    memscpy(outConfRules, sizeof(ITAccessPermissions_rules), &msmem->confRules, sizeof(ITAccessPermissions_rules));
    return Object_OK;
  }

  return Object_ERROR;
}

static int32_t
WrappedMemparcel_invoke(void *cxt, ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  WrappedMemparcel *me = (WrappedMemparcel*) cxt;
  ObjectOp method = ObjectOp_methodID(op);

  switch (method) {
  case Object_OP_retain:
    vm_osal_atomic_add(&me->refs, 1);
    return Object_OK;

  case Object_OP_release:
    return WrappedMemparcel_release(me);

  case WrappedMemparcel_OP_getConfinement:
    if (k != ObjectCounts_pack(0, 1, 0, 0)) {
      break;
    }
    ITAccessPermissions_rules *confRules_ptr = (ITAccessPermissions_rules *) args[0].b.ptr;
    if (args[0].b.size != sizeof(ITAccessPermissions_rules)) {
      break;
    }
    return WrappedMemparcel_getConfinement(me, confRules_ptr);
  }

  return Object_ERROR;
}

static Object WrappedMemparcel_asObject(WrappedMemparcel* me)
{
  return (Object) { WrappedMemparcel_invoke, me };
}

static WrappedMemparcel* WrappedMemparcel_fromObject(Object obj)
{
  return (obj.invoke == WrappedMemparcel_invoke ?
         (WrappedMemparcel*) obj.context : NULL);
}

// wrappedFdObj can be either FdWrapper or MSMem.
static Object WrappedMemparcel_getWrappedFdObj(WrappedMemparcel* me)
{
  int32_t fd = -1;
  if (isWrappedFd(me->wrappedFdObj, &fd) || isMSMem(me->wrappedFdObj, &fd)) {
    Object_retain(me->wrappedFdObj);
    return me->wrappedFdObj;
  }

  return Object_NULL;
}

// Valid wrappedFdObj should meet one and only one requirement below:
//    i) FdWrapper with confinement object attached.
//   ii) Remote MSMem.
static WrappedMemparcel* WrappedMemparcel_new(Object wrappedFdObj, MinkSocket* mSock)
{
  int ret = Object_ERROR;
  int fd = -1;
  Object confObj = Object_NULL;
  FdWrapper *fdw = NULL;
  MSMem *msmem = NULL;

  WrappedMemparcel* me = HEAP_ZALLOC_REC(WrappedMemparcel);
  if (!me) {
    return NULL;
  }

  if (isWrappedFd(wrappedFdObj, &fd)) {
    fdw = FdWrapperFromObject(wrappedFdObj);
    if (!fdw) {
      goto bail;
    }
    confObj = fdw->confinement;
    if (Object_isNull(confObj) || (NULL == ConfinementFromObject(confObj))) {
      goto bail;
    }
  } else if (isMSMem(wrappedFdObj, &fd)) {
    msmem = MSMemFromObject(wrappedFdObj);
    if (!msmem) {
      goto bail;
    }
    if (!Object_isOK(CConfinement_new(&msmem->confRules, &confObj))) {
      goto bail;
    }
  } else {
    goto bail;
  }

  ret = ShareMemory_GetMemParcelHandle(fd, confObj, MinkSocket_getDestVMName(mSock), &me->memparcelHandle);

  if (msmem) {
    // confObj in this case is a temp Object created by transport.
    Object_ASSIGN_NULL(confObj);
  }

  if (ret) {
    goto bail;
  }

#ifdef ANDROID
  LOG_TRACE("Sharing memory with dmaBufFd = %d and memparcelHdl = %lld.\n", fd, me->memparcelHandle);
#else
  LOG_TRACE("Sharing memory with dmaBufFd = %d and memparcelHdl = %ld.\n", fd, me->memparcelHandle);
#endif
  me->refs = 1;
  Object_INIT(me->wrappedFdObj, wrappedFdObj);
  return me;

bail:
  heap_free(me);
  return NULL;
}


typedef union {
  lxcom_msg msg;
  lxcom_hdr hdr;
  uint8_t buf[MSG_PREALLOCED_SIZE];
  uint64_t _aligned_unused;
} msgbuf_t;

typedef struct InvokeInfo {
  QNode qn;
  uint32_t invoke_id;
  int handle;
  ObjectOp op;
  ObjectArg *args;
  ObjectCounts k;
  int result;
  bool bComplete;
  pthread_cond_t cond;
} InvokeInfo;

static void InvokeInfo_init(InvokeInfo *me, int32_t h,
                ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  C_ZERO(*me);
  me->handle = h;
  me->op = op;
  me->args = args;
  me->k = k;
}

static inline void
InvokeInfo_setResult(InvokeInfo *me, int32_t result) {
  me->bComplete = true;
  me->result = result;
  vm_osal_cond_set(&me->cond);
}


#define FOR_ARGS(ndxvar, counts, section)                       \
  for (size_t ndxvar = ObjectCounts_index##section(counts);     \
       ndxvar < (ObjectCounts_index##section(counts)            \
                 + ObjectCounts_num##section(counts));          \
       ++ndxvar)

/*
 * Return -1 on error, 0 on success
 */
#define IO_REPEAT(func, fd, ptr, size)                         \
  while (size > 0) {                                           \
    ssize_t cb = func(fd, ptr, size, MSG_NOSIGNAL);            \
    if (cb <= 0) {                                             \
      return -1;                                               \
    }                                                          \
    if (cb <= (ssize_t) size) {                                \
      ptr = (cb + (char *) ptr);                               \
      size -= (size_t) cb;                                     \
    }                                                          \
  }                                                            \
  return 0;

/*
 * Return -1 on error, 0 on success
 */
static int send_all(int fd, void *ptr, size_t size)
{
  IO_REPEAT(send, fd, ptr, size);
}

#if defined(VNDR_VSOCK)
static int vsock_sendto(int sock, uint32_t node, uint32_t port, const void *data, unsigned int sz)
{
  struct sockaddr_vm client;
  memset(&client, 0, sizeof(client));
  VM_SOCK_ADDR_POPULATE(client, port, node);

  if (sendto(sock, data, sz, 0, (struct sockaddr *)&client, sizeof(struct sockaddr_vm)) < 0) {
    return -1;
  }

  return 0;
}
#else
static int vsock_sendto(int sock, uint32_t node, uint32_t port, const void *data, unsigned int sz)
{
    return 0;
}
#endif
/*
 * Return -1 on error, 0 or N (number of fds found) on success
 */
static int recv_msg(int fd, void *ptr, size_t size, int *fds, int num_fds)
{
  struct msghdr msg;
  C_ZERO(msg);
  struct iovec io = { .iov_base = ptr, .iov_len = size };
  struct cmsghdr *cmsg;
  int fd_count = 0;

  char buffer[sizeof(struct cmsghdr) + (num_fds * sizeof(int))];
  msg.msg_iov = &io;
  msg.msg_iovlen = 1;
  msg.msg_control = buffer;
  msg.msg_controllen = sizeof(buffer);
  //setup control data buffer
  cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg) {
    return -1;
  }
  //init fd buffer to -1
  memset(CMSG_DATA(cmsg), -1, num_fds);

  while (io.iov_len > 0) {
    C_ZERO(buffer); //reset the control buffer
    memset(CMSG_DATA(cmsg), -1, num_fds);

    ssize_t cb = recvmsg(fd, &msg, MSG_NOSIGNAL);
    if (cb <= 0) {
      return -1;
    }

    //collect ancillary data
    int msg_fds = cmsg->cmsg_len > sizeof(struct cmsghdr)
                    ? (cmsg->cmsg_len - sizeof(struct cmsghdr)) / sizeof(int)
                    : 0;

    for(int i = 0; i < msg_fds; i++) {
      if (fd_count >= num_fds) {
        for (int x = 0; x < fd_count; x++) {
          //close fds that were collected
          vm_osal_mem_close(fds[x]);
        }

        for (int y = i; y < msg_fds; y++) {
         //close fds that we weren't expecting
          int tmp;
          memcpy(&tmp, CMSG_DATA(cmsg)+(y*sizeof(int)), sizeof(int));
          vm_osal_socket_close(tmp);
        }

        return -1; // this shouldn't have happened
      }
      fd_count++;
    }

    memcpy(fds, CMSG_DATA(cmsg), fd_count*sizeof(int));
    if (cb <= (ssize_t) io.iov_len) {
        io.iov_base = (void *) (cb + (char*)io.iov_base);
        io.iov_len -= cb;
    }
  }
  return fd_count; //number of fds returned
}


#if defined(VNDR_SMCINVOKE_QRTR) || defined(VNDR_VSOCK)
static int sendv_all_remote(MinkSocket *me, struct iovec *iov, size_t iovLen, int *fds, int num_fds)
{
  size_t buf_len = 0;
  uint32_t max_payload = 0;

  fds = NULL;
  num_fds = 0;
  max_payload = (me->sockType == QIPCRTR)?MAX_QRTR_PAYLOAD:MAX_VSOCK_PAYLOAD;
  char *buf = (char *)malloc(max_payload);
  if (buf == NULL) {
    LOG_ERR("Error allocating 64KB buffer\n");
    return -1;
  }
  char *ptr = buf;

  while (iovLen > 0) {
    memcpy(ptr, iov->iov_base, iov->iov_len);
    buf_len += iov->iov_len;
    ptr = ptr + (int)iov->iov_len;
    --iovLen;
    ++iov;
  }

  if (me->sockType == QIPCRTR)
  {
    if (qrtr_sendto(me->sock, me->node, me->port, buf, buf_len) < 0) {
      LOG_ERR("qrtr_sendto failed: errno = %d, minksocket = %p, sock = %d, \
               node = %d\n", errno, me, me->sock, me->node);
      free(buf);
      return -1;
    }
  }
  else if (me->sockType == VSOCK) // VSOCK
  {
    if (vsock_sendto(me->sock, me->node, me->port, buf, buf_len) < 0) {
      LOG_ERR("qrtr_sendto failed: errno = %d, minksocket = %p, sock = %d, \
               node = %d\n", errno, me, me->sock, me->node);
      free(buf);
      return -1;
    }
  }
  else
  {
    LOG_ERR("unknown sockType = %d, minksocket = %p\n", me->sockType, me);
  }
  free(buf);
  return 0;
}
#else
static int sendv_all_remote(MinkSocket *me, struct iovec *iov, size_t iovLen, int *fds, int num_fds)
{
  return -1;
}
#endif

/*
 * Return -1 on error, 0 on success
 */
static int sendv_all(MinkSocket *me, struct iovec *iov, size_t iovLen, int *fds, int num_fds)
{
  int fd;
  struct msghdr msg;
  C_ZERO(msg);
  msg.msg_control = NULL;
  char buffer[sizeof(struct cmsghdr) + num_fds * sizeof(int)];
  C_ZERO(buffer);

  if ((me->sockType == QIPCRTR) || (me->sockType == VSOCK))
    return sendv_all_remote(me, iov, iovLen, fds, num_fds);

  fd = me->sock;

  if (num_fds > 0) {
    struct cmsghdr *cmsg;
    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);
    cmsg = CMSG_FIRSTHDR(&msg);
    if (!cmsg) {
      return -1;
    }
    cmsg->cmsg_len = msg.msg_controllen;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    memcpy(CMSG_DATA(cmsg), fds, num_fds*sizeof(int));
  }

  while (iovLen > 0) {
    msg.msg_iov = iov;
    msg.msg_iovlen = iovLen;
    ssize_t cb = sendmsg(fd, &msg, MSG_NOSIGNAL);
    if (cb < 0) {
      return -1;
    }

    if (msg.msg_control) {
      msg.msg_control = NULL;
      msg.msg_controllen = 0;
    }
    // Note corner case:
    // When cb = 0, we can still eliminate iovs, if they are zero-length.
    while (iovLen > 0) {
      if (cb < (ssize_t) iov->iov_len) {
        iov->iov_len -= cb;
        iov->iov_base = (void*) (cb + (char*)iov->iov_base);
        break;
      } else {
        cb -= iov->iov_len;
        ++iov;
        --iovLen;
      }
    }
  }
  return 0;
}



#define ERR_CLEAN(e) \
  do {                                         \
    if (!Object_isOK(err = (e))) {             \
        goto cleanup; }                        \
  } while(0)

#define CHECK_CLEAN(expr) \
  do {                                           \
    if (!(expr)) { CHECK_LOG(); goto cleanup; }  \
  } while (0)

#define ObjectCounts_numObjects(k)  (ObjectCounts_numOI(k) + \
                                     ObjectCounts_numOO(k))

#define ObjectCounts_indexObjects(k) \
  ObjectCounts_indexOI(k)

#define ObjectCounts_indexBUFFERS(k) \
  ObjectCounts_indexBI(k)

#define ObjectCounts_numBUFFERS(k) \
  (ObjectCounts_numBI(k) + ObjectCounts_numBO(k))

#define ObjectCounts_numIn(k) \
  (ObjectCounts_numBUFFERS(k) + ObjectCounts_numOI(k))

#define ObjectCounts_numOut(k) \
  (ObjectCounts_numBO(k) + ObjectCounts_numOO(k))

#define ObjectCounts_sizeofInvReq(k, extraOI) \
  (c_offsetof(lxcom_inv_req, a) + (ObjectCounts_numIn(k) + extraOI) * sizeof(lxcom_arg))

#define ObjectCounts_sizeofInvSucc(k, extraOO) \
  (c_offsetof(lxcom_inv_succ, a) + (ObjectCounts_numOut(k) + extraOO) * sizeof(lxcom_arg))

// To avoid potential risk on present platform, we just do simple check.
// TODO: we need to check it with ObjectCounts k in next generation.
#define CHECK_OBJ_INDEX_CLEAN(i)                              \
  do {                                                        \
    if ((i) >= LXCOM_MAX_ARGS) {                              \
      LOG_ERR("Out of index: index = %d\n", (uint32_t)(i));   \
      ERR_CLEAN(Object_ERROR_MAXARGS);                        \
    }                                                         \
  } while (0)

// To avoid potential risk on present platform, we just do simple check.
// TODO: we need to check it with ObjectCounts k in next generation.
#define CHECK_OBJ_INDEX_RETURN(i)                             \
  do {                                                        \
    if ((i) >= LXCOM_MAX_ARGS) {                              \
      LOG_ERR("Out of index: index = %d\n", (uint32_t)(i));   \
      return Object_ERROR_MAXARGS;                            \
    }                                                         \
  } while (0)

int32_t MinkSocket_populatePeerIdentity_remote(MinkSocket *me, uint32_t node, uint32_t port)
{
  me->vmuuid = HEAP_ZALLOC_ARRAY(uint8_t, VMUUID_MAX_SIZE);
  if (!me->vmuuid) {
    return Object_ERROR_KMEM;
  }

/* TODO: vmuuid of remote counterpart ought to be extracted from QRTR/VSOCK protocol
*  now just copy static value for simulation
*/
  me->vmuuidLen = memscpy(me->vmuuid, VMUUID_MAX_SIZE, vmuuidOEM, VMUUID_MAX_SIZE);

  return Object_OK;
}

int32_t MinkSocket_populatePeerIdentity(MinkSocket *me, int sock)
{
  socklen_t szInfo = sizeof(me->credInfo);

  me->peer_available = false;
  if (vm_osal_getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &(me->credInfo), &szInfo) == 0) {
    me->peer_available = true;
    return Object_OK;
  }

  return Object_ERROR;
}

int32_t MinkSocket_initObjectTable(MinkSocket *me, Object endpoint, bool workForModule)
{
  int32_t result;
  Object medium = endpoint;

  if (me->workForServer) {
    if (workForModule) {
      if (UNIX == me->sockType) {
        result = LinkCred_new(&(me->credInfo), ELOC_LOCAL, me->sockType,
                                     &(me->linkCredentials));
        if (result) {
          LOG_ERR("LinkCredentials_new failed, result = %d, minksocket = %p\n",
                   result, me);
          me->linkCredentials = Object_NULL;
          return result;
        }
        LOG_TRACE("constructed linkCredentials objOut = %p, linkCredentials = %p, \
                   belongs to minksocket = %p\n", &(me->linkCredentials),
                   (me->linkCredentials).context, me);
        result = LocalAdapter_new(endpoint, me->linkCredentials, &(me->forwarder));
        if (result) {
          LOG_ERR("LocalAdapter_new failed, result = %d, minksocket = %p, \
                   endpoint = %p\n", result, me, &endpoint);
          Object_ASSIGN_NULL(me->linkCredentials);
          me->forwarder = Object_NULL;
          return result;
        }
        LOG_TRACE("constructed LocalAdapter objOut = %p, LocalAdapter = %p \
                   endpoint = %p, linkCred = %p, belongs to minksocket = %p\n",
                   &(me->forwarder), (me->forwarder).context, &endpoint,
                   &me->linkCredentials, me);
      } else {
        result = LinkCredRemote_new(me->vmuuid, me->vmuuidLen, ELOC_REMOTE, me->sockType,
                                           &(me->linkCredentials));
        if (result) {
          LOG_ERR("LinkCredentials_new failed, result = %d, minksocket = %p\n",
                   result, me);
          me->linkCredentials = Object_NULL;
          return result;
        }
        LOG_TRACE("constructed linkCredentialsRemote objOut = %p, linkCredentials = %p, \
                   belongs to minksocket = %p\n", &(me->linkCredentials),
                   (me->linkCredentials).context, me);
        result = RemoteAdapter_new(endpoint, me->linkCredentials, &(me->forwarder));
        if (result) {
          LOG_ERR("RemoteAdapter_new failed, result = %d, minksocket = %p, \
                   endpoint = %p\n", result, me, &endpoint);
          Object_ASSIGN_NULL(me->linkCredentials);
          me->forwarder = Object_NULL;
          return result;
        }
        LOG_TRACE("constructed RemoteAdapter objOut = %p, RemoteAdapter = %p \
                   belongs to minksocket = %p\n", &(me->forwarder),
                   (me->forwarder).context, me);
      }
      medium = me->forwarder;
    }

    result = ObjectTableMT_addObject(&me->table, medium);
    if (GENERIC_HANDLE != result) {
      LOG_ERR("ObjectTable initialize Object GENERIC_HANDLE failed, result = %d, \
               minksocket = %p, table = %p\n", result, me, &me->table);
      if (workForModule) {
        Object_ASSIGN_NULL(me->linkCredentials);
        Object_ASSIGN_NULL(me->forwarder);
      }
      return Object_ERROR_NOSLOTS;
    }
  }

  return Object_OK;
}

MinkSocket *MinkSocket_new(Object endpoint, int32_t sockType, int32_t sock, uint32_t node, uint32_t port)
{
  MinkSocket *me = HEAP_ZALLOC_REC(MinkSocket);
  if (!me) {
    return NULL;
  }

  me->refs = 1;
  me->sock = sock;
  me->node = node;
  me->port = port;
  me->sockType = sockType;
  me->bDone = false;
  me->msForwarderCount = 0;
  me->invoke_id = BEGIN_INVOKE_ID;
  QList_construct(&me->qlInvokes);
  QList_construct(&me->qlMSForwarder);
  CHECK_CLEAN (!socketpair(AF_UNIX, SOCK_STREAM, 0, me->sockPair));
  vm_osal_mutex_init(&me->mutex, NULL);
  vm_osal_cond_init(&me->cond, NULL);
  if (!Object_isNull(endpoint)) {
    me->workForServer = true;
    if (sockType == VSOCK) {
      me->closeOnOTEmpty = true;
    }
  }
  me->pool = ThreadPool_new(me->workForServer);
  CHECK_CLEAN(me->pool);

  CHECK_CLEAN(!ObjectTableMT_construct(&me->table, MAX_OBJECT_COUNT));

  LOG_MSG("constructed threadPool %p, objectTable %p, belongs to minksocket %p of sockType %d\n",
           me->pool, &me->table, me, sockType);

  return me;

cleanup:
  MinkSocket_release(me);
  return NULL;
}

static inline bool MinkSocket_isReady(MinkSocket *me)
{
  if (me == NULL) {
    return false;
  }

  return (me->sock != -1);
}

/*
 * Retain reference count of MinkSocket to keep its lifetime
*/
void MinkSocket_retain(MinkSocket *me)
{
  vm_osal_atomic_add(&me->refs, 1);
}

bool MinkSocket_isConnected(MinkSocket *me)
{
  bool ret = false;

  vm_osal_mutex_lock(&me->mutex);
  if (!me->bDone && -1 != me->sock) {
    ret = true;
  }
  vm_osal_mutex_unlock(&me->mutex);

  return ret;
}

/*@brief: only if MinkSocket working for server, release its reference count
 *        once finish that ThreadWork.
*/
static inline
void MinkSocket_dequeue(MinkSocket *me)
{
  if (NULL == me) {
    return;
  }

  if (me->workForServer) {
    MinkSocket_release(me);
  }

  return;
}

/*@brief: only if MinkSocket working for server, retain its reference count
 *        once start new ThreadWork. Release the reference count once finish
 *        that ThreadWork. Thus MinkSocket never be freed before all TheadWorks done.
*/
void MinkSocket_enqueue(MinkSocket *me, ThreadWork *work)
{
  if (NULL == me || NULL == work) {
    return;
  }

  vm_osal_mutex_lock(&me->mutex);
  if (!me->bDone && -1 != me->sock) {
    if (me->workForServer) {
      MinkSocket_retain(me);
    }
    ThreadPool_queue(me->pool, work);
  } else {
    free(work);
  }
  vm_osal_mutex_unlock(&me->mutex);

  return;
}

/*@brief: clean up resource occupied by MinkSocket
*/
void MinkSocket_delete(MinkSocket *me)
{
  if (me->pool) {
    ThreadPool_release(me->pool);
  }
  if (me->sockPair[0] >= 0) {
    vm_osal_socket_close(me->sockPair[0]);
  }
  if (me->sockPair[1] >= 0) {
    vm_osal_socket_close(me->sockPair[1]);
  }

  Object_ASSIGN_NULL(me->forwarder);
  Object_ASSIGN_NULL(me->linkCredentials);
  if (me->vmuuid) {
    heap_free(me->vmuuid);
  }
  MinkSocket_deinitPrimordial(me);
  ObjectTableMT_destruct(&me->table);
  vm_osal_mutex_deinit(&me->mutex);
  vm_osal_cond_deinit(&me->cond);

  heap_free(me);
}

/*@brief: close socket connection but not detach MSForwarder or
 *        clean up resource
*/
void MinkSocket_close(MinkSocket *me, int32_t err)
{
  if (NULL == me) {
    return;
  }

  vm_osal_mutex_lock(&me->mutex);

  if ((me->bDone != true) && (me->sock != -1)) {
    me->bDone = true;
    for (int i = 0; i < THREADPOOL_MAX_THREADS; i++ ) {
      if (write(me->sockPair[1], "x", 1)) { } // interrupt poll.
    }
    if (UNIX == me->sockType) {
      vm_osal_socket_close(me->sock);
    }
    me->sock = -1;

    QNode *pqn;
    while ((pqn = QList_pop(&me->qlInvokes))) {
      InvokeInfo_setResult(c_containerof(pqn, InvokeInfo, qn), err);
    }

  }

  vm_osal_mutex_unlock(&me->mutex);
}

/*@brief: Release reference count of MinkSocket to monitor its lifetime
 *        Only if reference count decrease to be 0, will MinkSocket be closed,
 *        stopped and deleted.
*/
void MinkSocket_release(MinkSocket *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_MSG("released minkSocket = %p, threadPool = %p, objectTable = %p\n",
             me, me->pool, &me->table);
    MinkSocket_close(me, Object_ERROR_UNAVAIL);
    MinkSocket_delete(me);
  }
}

int MinkSocket_detach(MinkSocket *me)
{
  if (me->msForwarderCount != 0 || me->sock == -1) return -1;

  int retFd = -1;
  retFd = vm_osal_fd_dup(me->sock);
  MinkSocket_close(me, Object_ERROR_UNAVAIL);
  MinkSocket_notifyClose(me, EVENT_CONN_DETACH);
  MinkSocket_detachForwarderAll(me);

  return retFd;
}

int MinkSocket_detachObject(Object *obj)
{
  MSForwarder *msforwarder = MSForwarderFromObject(*obj);
  if (msforwarder) {
    return MSForwarder_detach(msforwarder);
  }

  return -1;
}

int32_t MinkSocket_attachObject(MinkSocket *me, int handle, Object *obj)
{
  return MSForwarder_new(me, handle, obj);
}

static uint8_t PadBuf[8] = {0xF};
static int32_t insertIOV(struct iovec *iov, int32_t nMaxIov,
                         int32_t *pnUsed,
                         void *ptr, size_t size,
                         uint32_t *szOnWire)
{
  if (*pnUsed >= nMaxIov) {
    return Object_ERROR_MAXARGS;
  }

  int32_t pad = PADDED(*szOnWire) - *szOnWire;
  if (pad) {
    iov[(*pnUsed)++] = (struct iovec) { PadBuf, pad };
    *szOnWire += pad;
  }

  if (*pnUsed >= nMaxIov) {
    return Object_ERROR_MAXARGS;
  }

  iov[(*pnUsed)++] = (struct iovec) { ptr, size };
  *szOnWire += size;
  return Object_OK;
}

static int
MinkSocket_createArgs(MinkSocket *me, const InvokeInfo *pii,
                     lxcom_inv_req *req,
                     struct iovec *iov, int32_t szIov,
                     int32_t *nIov,
                     int *fds, int num_fds)
{
  int32_t err;
  C_ZERO(*req);
  req->hdr.type = LXCOM_REQUEST;
  req->hdr.invoke_id = pii->invoke_id;
  req->op = pii->op;
  req->handle = pii->handle;
  req->k = pii->k;
  int fd_index = 0;

  LOG_TRACE("minkSocket = %p, handle = %d,objectTable = %p\n", me, req->handle,
             &me->table);
  for(int i = 0; i < num_fds; i++) {
    fds[i] = -1; //unset all the fds
  }

  // Count the extra args from memparcels
  size_t numExtraArgs = COUNT_NUM_EXTRA_ARGS_OUTBOUND(pii->k, pii->args, OI);

  // Check for too many args:
  CHECK_MAX_ARGS(pii->k, In, numExtraArgs);

  *nIov = 0;
  err = insertIOV(iov, szIov, nIov,
                  req, ObjectCounts_sizeofInvReq(pii->k, numExtraArgs),
                  &req->hdr.size);
  if (Object_OK != err) {
    if (Object_ERROR_MAXARGS == err) {
      return err;
    }
    return Object_ERROR_INVALID;
  }

  FOR_ARGS(i, pii->k, BO) {
    if (!pii->args[i].b.ptr && pii->args[i].b.size != 0) {
      return Object_ERROR_INVALID;
    }
  }

  // Req Header for:
  // HDR | BI Sizes | BO Sizes | OI handles
  FOR_ARGS(i, pii->k, BI) {
    err = insertIOV(iov, szIov, nIov,
                    pii->args[i].b.ptr, pii->args[i].b.size,
                    &req->hdr.size);
    if (Object_OK != err) {
      return Object_ERROR_INVALID;
    }
  }

  //Copy buffer-sizes into header
  FOR_ARGS(i, pii->k, BUFFERS) {
    req->a[i].size = pii->args[i].b.size;
  }

  size_t iOI = ObjectCounts_indexOI(pii->k);

  //Copy object handles into header
  FOR_ARGS(i, pii->k, OI) {
    if (Object_isNull(pii->args[i].o)) {
      // nothing to do
    } else {
      if (isPrimordialOrPrimordialFwd(pii->args[i].o)) {
        LOG_ERR("promordial or primordialFwd cannot be MinkIPC param\n");
        return Object_ERROR;
      }

      MSForwarder *msf = MSForwarderFromObject(pii->args[i].o);
      int fd = -1;
      CHECK_OBJ_INDEX_RETURN(iOI);

      if (msf && msf->conn == me) {
        // The obj is an MSForwarder
        req->a[iOI].o.flags = LXCOM_CALLEE_OBJECT;
        req->a[iOI].o.handle = msf->handle;
        LOG_TRACE("MSForwarder object with handle = %d, minkSocket = %p\n",
                   req->a[iOI].o.handle, me);
      } else if (isWrappedFd(pii->args[i].o, &fd)) {
        // The obj is a wrapped fd
        if (!is_remote_sock(me->sockType)) {
          // On a local domain socket, we just send the plain fd
          fds[fd_index] = fd;
          fd_index++;
          req->a[iOI].o.flags = LXCOM_DESCRIPTOR_OBJECT;
          req->a[iOI].o.handle = INVALID_OBJECT_HANDLE;

          Object dependency = (FdWrapperFromObject(pii->args[i].o))->dependency;

          if (!Object_isNull(dependency)) {
            MSForwarder *msf = MSForwarderFromObject(dependency);
            if (msf && msf->conn == me) {
              // msf.handle is the handle of msmemobj at tzd side
              req->a[iOI].o.handle = msf->handle;

              // we dont need to send this fd.
              // cause tzd's OT still holds the ref to the msmemobj of this fd.
              req->a[iOI].o.flags = LXCOM_CALLEE_OBJECT;
              fds[fd_index] = -1;
              fd_index--;
              LOG_TRACE("local WrappedFd object with fd = %d, msForwarder = %p \
                         handle = %d, minkSocket = %p\n", fd, msf, msf->handle, me);
            }
          }
        } else {

          LOG_MSG("ready to create new wrappedMemparcel with dmabuffd=%d\n", fd);

          Object confObj = (FdWrapperFromObject(pii->args[i].o))->confinement;

          if (!Object_isNull(confObj) && (NULL == ConfinementFromObject(confObj))) {
            LOG_ERR("FdWrapper has non-confinement!\n");
            return Object_ERROR_BADOBJ;
          }

          if (Object_isNull(confObj)) {
            if (!Object_isOK(RemoteShareMemory_attachConfinement(NULL, &pii->args[i].o))) {
              LOG_ERR("Attach default rules of MEM_SHARE failed!\n");
              return Object_ERROR_UNAVAIL;
            }
            confObj = (FdWrapperFromObject(pii->args[i].o))->confinement;
            LOG_MSG("Adopt default confinement because not configured.\n");
          }

          uint64_t specialRules = 0;
          if (!Object_isOK(IConfinement_getSpecialRules(confObj, &specialRules))) {
            LOG_ERR("Unexpected failure happens when IConfinement_getSpecialRules!\n");
            return Object_ERROR_UNAVAIL;
          }

          // On a remote socket, we need to share the memory via mem-buf
          // Creating new WrappedMemparcel shares the memory with the destination VM.
          WrappedMemparcel* wmp = WrappedMemparcel_new(pii->args[i].o, me);
          if (!wmp) {
            return Object_ERROR_UNAVAIL;
          }

          // Add this to the OT
          int h = ObjectTableMT_addObject(&me->table, WrappedMemparcel_asObject(wmp));
          if (h == -1) {
            WrappedMemparcel_release(wmp);
            return Object_ERROR_KMEM;
          }

          // OT should hold the only reference to wmp
          WrappedMemparcel_release(wmp);

          req->a[iOI].o.flags = LXCOM_CALLER_OBJECT | LXCOM_MEMPARCEL_OBJECT;
          req->a[iOI].o.handle = h;

          // Add memparcel handle information to args array
          // The next four args are used to hold the handle information
          int64_t memparcelHandle = wmp->memparcelHandle;
          for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
            iOI++;
            CHECK_OBJ_INDEX_RETURN(iOI);
            req->a[iOI].o.flags = LXCOM_MEMPARCEL_INFO;
            req->a[iOI].o.handle = (uint16_t) (memparcelHandle >> ii*16);
          }

          // Wrap specialRules into next 4 args
          for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
            iOI++;
            CHECK_OBJ_INDEX_RETURN(iOI);
            req->a[iOI].o.flags = LXCOM_MEMPARCEL_SPECIALRULES;
            req->a[iOI].o.handle = (uint16_t) (specialRules >> ii * 16);
          }

#ifdef ANDROID
          LOG_TRACE("remote WrappedFd object with wrappedMemparcel = %p,handle = %d \
                     memparcelHandle = %lld, minkSocket = %p\n", wmp, h,
                     memparcelHandle, me);
#else
          LOG_TRACE("remote WrappedFd object with wrappedMemparcel = %p,handle = %d \
                     memparcelHandle = %ld, minkSocket = %p\n", wmp, h,
                     memparcelHandle, me);
#endif
        }
      }
      else if (isMSMem(pii->args[i].o, &fd)) {
        if (!is_remote_sock(me->sockType)) {
          fds[fd_index] = fd;
          fd_index++;
          req->a[iOI].o.flags = LXCOM_DESCRIPTOR_OBJECT;

          int h = ObjectTableMT_addObject(&me->table, pii->args[i].o);
          if (h == -1) {
            return Object_ERROR_KMEM;
          } else {
            req->a[iOI].o.handle = h;
          }
          LOG_TRACE("MsMem object with fd = %d, handle = %d, minkSocket = %p\n",
                     fd, h, me);
        } else {
          MSMem *msmem = MSMemFromObject(pii->args[i].o);

          if (!msmem) {
            LOG_ERR("Unexpected error occurs as MSMem.\n");
            return Object_ERROR_BADOBJ;
          }

          if (msmem->isLocal) {
            // Set it for MEM_SHARE.
            msmem->confRules.specialRules = ITAccessPermissions_keepSelfAccess;
          }

          Object dependency = msmem->dependency;
          if (!Object_isNull(dependency)) {
            MSForwarder *msf = MSForwarderFromObject(dependency);
            if (msf && msf->conn == me) {
              // msf.handle is the handle of wrappedmemparcel located at destVM's OT.
              req->a[iOI].o.handle = msf->handle;
              req->a[iOI].o.flags = LXCOM_CALLEE_OBJECT | LXCOM_MEMPARCEL_OBJECT;
              iOI += NUM_EXTRA_ARGS_PER_MEMPARCEL;
              iOI += NUM_EXTRA_ARGS_SPECIALRULES;
              iOI++;
              continue;
            }
          }

          uint64_t specialRules = msmem->confRules.specialRules;

          /*  Remote MSMem (from HLOS) of such specialRules is hopping around QTVM and OEMVM.
           *  ACCEPTs of such DMA buffer are executed by different kernel modules:
           *    i)  QTVM: SMMU Proxy
           *   ii) OEMVM: MEM-BUF
           *  When reaching out to counterpart VM, we should get specialRules flipped over:
           *    i) ITAccessPermissions_mixedControlled -> ITAccessPermissions_smmuProxyControlled
           *   ii) ITAccessPermissions_smmuProxyControlled -> ITAccessPermissions_mixedControlled
           *  But we dont change the original specialRules in MSMem.
           */
          if ((ITAccessPermissions_mixedControlled & specialRules) ||
              (ITAccessPermissions_smmuProxyControlled & specialRules)) {
            uint64_t maskSpecialRules = ITAccessPermissions_mixedControlled | ITAccessPermissions_smmuProxyControlled;
            specialRules ^= maskSpecialRules;
            LOG_MSG("specialRules is getting flipped over.\n");
          }

          // On a remote socket, we need to share the memory via mem-buf
          // Creating new WrappedMemparcel shares the memory with the destination VM.
          WrappedMemparcel* wmp = WrappedMemparcel_new(pii->args[i].o, me);
          if (!wmp) {
            return Object_ERROR_UNAVAIL;
          }

          // Add this to the OT
          int h = ObjectTableMT_addObject(&me->table, WrappedMemparcel_asObject(wmp));
          if (h == -1) {
            WrappedMemparcel_release(wmp);
            return Object_ERROR_KMEM;
          }

          // OT should hold the only reference to wmp
          WrappedMemparcel_release(wmp);

          req->a[iOI].o.flags = LXCOM_CALLER_OBJECT | LXCOM_MEMPARCEL_OBJECT;
          req->a[iOI].o.handle = h;

          // Add memparcel handle information to args array
          // The next four args are used to hold the handle information
          int64_t memparcelHandle = wmp->memparcelHandle;
          for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
            iOI++;
            CHECK_OBJ_INDEX_RETURN(iOI);
            req->a[iOI].o.flags = LXCOM_MEMPARCEL_INFO;
            req->a[iOI].o.handle = (uint16_t) (memparcelHandle >> ii*16);
          }

          // Wrap specialRules into next 4 args
          for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
            iOI++;
            CHECK_OBJ_INDEX_RETURN(iOI);
            req->a[iOI].o.flags = LXCOM_MEMPARCEL_SPECIALRULES;
            req->a[iOI].o.handle = (uint16_t) (specialRules >> ii * 16);
          }

#ifdef ANDROID
          LOG_TRACE("remote WrappedFd object with wrappedMemparcel = %p,handle = %d \
                     memparcelHandle = %lld, minkSocket = %p\n", wmp, h,
                     memparcelHandle, me);
#else
          LOG_TRACE("remote WrappedFd object with wrappedMemparcel = %p,handle = %d \
                     memparcelHandle = %ld, minkSocket = %p\n", wmp, h,
                     memparcelHandle, me);
#endif
        }
      } else {
        // New object, need entry in the OT
        int h = ObjectTableMT_addObject(&me->table, pii->args[i].o);
        if (h == -1) {
          return Object_ERROR_KMEM;
        }

        req->a[iOI].o.flags = LXCOM_CALLER_OBJECT;
        req->a[iOI].o.handle = h;
        LOG_TRACE("add object = %p to objectTable = %p with handle = %d, \
                   minkSocket = %p\n", &pii->args[i].o, &me->table, h, me);
      }
    }
    iOI++;
  }

  return Object_OK;
}

/*
 * Process arguments returned from the kernel.  For buffer arguments there
 * is nothing to do.  For returned objects, the `inv` structure holds newly
 * allocated descriptors, around which we construct new forwarder instances.
 */
static int32_t
MinkSocket_marshalOut(MinkSocket *me, lxcom_inv_succ *succ, InvokeInfo *pii,
                      int *fds, int num_fds)
{
  int err = Object_OK;

  // In the succ args array, OOs start right after BOs
  size_t iOO = ObjectCounts_numBO(pii->k);
  int fd_index = 0;

  FOR_ARGS(i, pii->k, OO) {
    CHECK_OBJ_INDEX_CLEAN(iOO);
    uint16_t flags = succ->a[iOO].o.flags;
    uint16_t handle = succ->a[iOO].o.handle;
    if (flags & LXCOM_CALLEE_OBJECT) {

      if (flags & LXCOM_MEMPARCEL_OBJECT) {
        int64_t memparcelHandle = 0;

        // Assemble the memparcel handle from the next four args entries
        for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
          iOO++;
          CHECK_OBJ_INDEX_CLEAN(iOO);
          if (!(succ->a[iOO].o.flags & LXCOM_MEMPARCEL_INFO)) {
            ERR_CLEAN(Object_ERROR_UNAVAIL);
          }
          uint16_t handleChunk = succ->a[iOO].o.handle;
          memparcelHandle |= ((int64_t)handleChunk << ii*16);
        }

        uint64_t specialRules = 0;
        // Unwrap the specialRules from the next 4 args
        for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
          iOO++;
          CHECK_OBJ_INDEX_CLEAN(iOO);
          if (!(succ->a[iOO].o.flags & LXCOM_MEMPARCEL_SPECIALRULES)) {
            ERR_CLEAN(Object_ERROR_UNAVAIL);
          }
          uint16_t handleChunk = succ->a[iOO].o.handle;
          specialRules |= ((uint64_t)handleChunk << ii * 16);
        }

        Object wmpMsf = Object_NULL;
        err = MSForwarder_new(me, handle, &wmpMsf);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        ITAccessPermissions_rules *wmpConfRules = HEAP_ZALLOC_REC(ITAccessPermissions_rules);
        if (!wmpConfRules) {
          LOG_ERR("cannot allocate memory for confinement struct!\n");
          ERR_CLEAN(Object_ERROR_KMEM);
        }

        wmpConfRules->specialRules = specialRules;

        Object MSMemObj = Object_NULL;
        err = ShareMemory_GetMSMem(memparcelHandle, MinkSocket_getDestVMName(me), wmpConfRules, &MSMemObj);
        heap_free(wmpConfRules);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        // when MSMemObj is releasing, it should trigger sendClose to remote WMPObj.
        MSMem *returnedMSMem = MSMemFromObject(MSMemObj);
        if (returnedMSMem == NULL) {
          LOG_ERR("Unexpected error occurs as msmem.\n");
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        returnedMSMem->dependency = wmpMsf;

        pii->args[i].o = MSMemObj;

#ifdef ANDROID
        LOG_TRACE("remote WrappedFd object(callee) with memparcelHandle = %lld \
                   handle = %d, dmaBufFd = %d, msForwarder = %p, \
                   minkSocket = %p\n", memparcelHandle, handle, returnedMSMem->dmaBufFd, &wmpMsf,
                   me);
#else
        LOG_TRACE("remote WrappedFd object(callee) with memparcelHandle = %ld \
                   handle = %d, dmaBufFd = %d, msForwarder = %p, \
                   minkSocket = %p\n", memparcelHandle, handle, returnedMSMem->dmaBufFd, &wmpMsf,
                   me);
#endif

      } else {
        // Normal callee object, create new outbound object
        err = MSForwarder_new(me, succ->a[iOO].o.handle, &pii->args[i].o);
        if (err) {
          ERR_CLEAN(Object_ERROR_KMEM);
        }
        LOG_TRACE("receive handle = %d, constructed msForwarder = %p, \
                   minkSocket = %p\n", handle, &pii->args[i].o, me);
      }

    } else if (flags & LXCOM_CALLER_OBJECT) {
      // Familiar object
      Object obj = ObjectTableMT_recoverObject(&me->table,
                                               succ->a[iOO].o.handle);
      if (Object_isNull(obj)) {
        ERR_CLEAN(Object_ERROR_BADOBJ);
      }

      // TODO: this case is a legacy case which is no longer valid.
      if (flags & LXCOM_MEMPARCEL_OBJECT) {
        // It's a WMP, need to give back the FdWarpper
        WrappedMemparcel* wmp = WrappedMemparcel_fromObject(obj);
        if (!wmp) {
          Object_release(obj);
          ERR_CLEAN(Object_ERROR_BADOBJ);
        }
        pii->args[i].o = WrappedMemparcel_getWrappedFdObj(wmp);
        WrappedMemparcel_release(wmp);
        LOG_TRACE("remote WrappedFd object(caller) with fdWrapper = %p\n",
                   &pii->args[i].o);
      } else {
        // Normal object
        pii->args[i].o = obj;
        LOG_TRACE("MSForwarder object with handle = %d, obj(normal object) = %p, \
                   minkSocket = %p\n", handle, &obj, me);
      }
    } else if (flags & LXCOM_DESCRIPTOR_OBJECT) {
      if (fd_index >= num_fds) {
        //We didn't receive enough Fds to give out
        ERR_CLEAN(Object_ERROR_UNAVAIL);
      }
      pii->args[i].o = FdWrapper_new(fds[fd_index]);
      if (Object_isNull(pii->args[i].o)) {
        ERR_CLEAN(Object_ERROR_KMEM);
      }

      //so we don't double close if an error occurs
      fds[fd_index] = -1;
      fd_index++;
      if (handle != INVALID_OBJECT_HANDLE) {
        Object msf = Object_NULL;
        err = MSForwarder_new(me, handle, &msf);
        if (Object_isNull(msf)) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        // The fdwrapper (obj) holds the only reference to msf
        FdWrapper *returnedFdWrapper = FdWrapperFromObject(pii->args[i].o);
        if (returnedFdWrapper == NULL) {
          LOG_ERR("Unexpected error occurs as fdwrapper.\n");
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        returnedFdWrapper->dependency = msf;
      }
      LOG_TRACE("local WrappedFd object with obj = %p\n", &pii->args[i].o);
    } else {
      pii->args[i].o = Object_NULL;
    }
    ++iOO;
  }

  return Object_OK;

  /*
   * If marshaling of any object has failed, we will be returning an error.
   * In that case the caller will not receive output objects so we must
   * free any allocated object data (and release the descriptors they hold).
   */

cleanup:

  FOR_ARGS(i, pii->k, OO) {
    Object_ASSIGN_NULL(pii->args[i].o);
  }

  for (int i = 0; i < num_fds; i++) {
    if (fds[i] != -1) {
      vm_osal_mem_close(fds[i]);
    }
  }
  return err;
}



//Called from invoker thread...
int32_t
MinkSocket_invoke(MinkSocket *me, int32_t h,
                ObjectOp op, ObjectArg *args, ObjectCounts k)
{
  if (!MinkSocket_isReady(me)) {
    return Object_ERROR_UNAVAIL;
  }

  int32_t err = Object_OK;
  lxcom_inv_req req;
  struct iovec iov[LXCOM_MAX_ARGS*2];
  int32_t nIov = 0;
  InvokeInfo ii;
  int fds[ObjectCounts_maxOI];

  LOG_TRACE("minkSocket = %p, handle = %d, sock = %d\n", me, h, me->sock);
  InvokeInfo_init(&ii, h, op, args, k);

  if (0 != vm_osal_cond_init(&ii.cond, NULL)) {
    return Object_ERROR_KMEM;
  }

  err = MinkSocket_createArgs(me, &ii, &req, iov,
                              C_LENGTHOF(iov), &nIov,
                              fds, C_LENGTHOF(fds));
  if (err) {
    LOG_ERR("MinkSocket_createArgs fail\n");
    goto cleanup;
  }

  int num_fds = 0;
  for (int i = 0; i < (int) C_LENGTHOF(fds); i++) {
    if (fds[i] != -1) {
      num_fds++;
    }
  }

  vm_osal_mutex_lock(&me->mutex);

  //Allocate an ID for this invocation
  ii.invoke_id = req.hdr.invoke_id = me->invoke_id++;
  QList_appendNode(&me->qlInvokes, &ii.qn);

  if (-1 == sendv_all(me, iov, nIov, fds, num_fds)) {
    QNode_dequeue(&ii.qn);
    err = Object_ERROR_UNAVAIL;
  }

  //wait for the response
  if (Object_OK == err) {
    while (!ii.bComplete) {
      vm_osal_cond_wait(&ii.cond, &me->mutex, NULL);
    }
    QNode_dequeueIf(&ii.qn);
    err = ii.result;
  }
  vm_osal_mutex_unlock(&me->mutex);

cleanup:

  vm_osal_cond_deinit(&ii.cond);

  return err;
}

static InvokeInfo *
MinkSocket_getInvokeInfo(MinkSocket *me, uint32_t id)
{
  vm_osal_mutex_lock(&me->mutex);

  QNode *pqn;
  QLIST_FOR_ALL(&me->qlInvokes, pqn) {
    InvokeInfo *pii = c_containerof(pqn, InvokeInfo, qn);
    if (pii->invoke_id == id) {
      vm_osal_mutex_unlock(&me->mutex);
      return pii;
    }
  }
  vm_osal_mutex_unlock(&me->mutex);
  return NULL;
}

/******************************************************************
  send close(LXCOM_CLOSE) massage to the receiver
  caller takes care of the mutex
*******************************************************************/
int32_t
MinkSocket_sendClose(MinkSocket *me, int handle)
{
  int32_t ret;
  if (!MinkSocket_isReady(me)) {
    return Object_ERROR_UNAVAIL;
  }

  int sz = sizeof(lxcom_inv_close);
  lxcom_inv_close cls = (lxcom_inv_close) { sz, LXCOM_CLOSE, handle};

  if (me->sockType == QIPCRTR) {
    ret = qrtr_sendto(me->sock, me->node, me->port, &cls, sz);
  }
  else if (me->sockType == VSOCK) {
    struct sockaddr_vm client;
    memset(&client, 0, sizeof(client));
    VM_SOCK_ADDR_POPULATE(client, me->port, me->node);

    ret = sendto(me->sock, &cls, sz, 0, (struct sockaddr *)&client, sizeof(struct sockaddr_vm));
  }
  else {
    ret = send_all(me->sock, &cls, sz);
  }

  return (ret == -1) ? Object_ERROR_UNAVAIL : Object_OK;
}


/* Handles Invoke Success messages and marshals out arguments
 *
 * The Success buffer (mb->msg.succ of type lxcom_inv_succ) is
 * used to send information about out buffers and out objects.
 * MinkSocket_SendInvokeSuccess populates this with output argument
 * details.
 * Indicies from 0-numBO pertain to out Buffers and
 * Indicies from numBo-numOO pertain to out Objects
*/
static int32_t
MinkSocket_recvInvocationSuccess(MinkSocket *me, msgbuf_t *mb,
                                 ThreadWork *work, int *fds, int num_fds)
{
  int32_t err = Object_OK;
  InvokeInfo *pii = NULL;

  //queue up the next read
  if (work) {
    MinkSocket_enqueue(me, work);
    work = NULL;
  }

  pii = MinkSocket_getInvokeInfo(me, mb->hdr.invoke_id);
  if (NULL == pii) {
    LOG_ERR("incredible failure to get invokeInfo of invoke_id %d in minksocket %p\
         Leading to caller never be replied\n", mb->hdr.invoke_id, (void *)me);
    return Object_ERROR_UNAVAIL;
  }

  // Count the "extra" args not represented in the ObjectCounts
  size_t numExtraArgs = COUNT_NUM_EXTRA_ARGS_INBOUND(
                          pii->k,
                          mb->msg.succ.a, OO,
                          ObjectCounts_numBO(pii->k));

  size_t size = ObjectCounts_sizeofInvSucc(pii->k, numExtraArgs);
  if (size > mb->hdr.size) {
    LOG_ERR("failed hdr size check, size %d, hdr.size %d, numExtraArgs %d, k %x\n ",
         (uint32_t)size, (uint32_t)mb->hdr.size, (uint32_t)numExtraArgs, pii->k);
    ERR_CLEAN(Object_ERROR_MAXARGS);
  }

  if (ObjectCounts_total(pii->k) > C_LENGTHOF(mb->msg.succ.a)) {
    LOG_ERR("unexpected error\n");
    ERR_CLEAN(Object_ERROR_MAXARGS);
  }

  int iBO = 0;
  FOR_ARGS(i, pii->k, BO) {
    size = PADDED(size);
    if (size > mb->hdr.size) {
      LOG_ERR("unexpected error\n");
      ERR_CLEAN(Object_ERROR_MAXARGS);
    }

    if (pii->args[i].b.size < mb->msg.succ.a[iBO].size) {
      LOG_ERR("unexpected error\n");
      ERR_CLEAN(Object_ERROR_INVALID);
    }

    memcpy(pii->args[i].b.ptr,
           mb->buf + size, mb->msg.succ.a[iBO].size);
    pii->args[i].b.size = mb->msg.succ.a[iBO].size;
    size += mb->msg.succ.a[iBO].size;
    if (size > mb->hdr.size) {
      LOG_ERR("unexpected error\n");
      ERR_CLEAN(Object_ERROR_MAXARGS);
    }
    iBO++;
  }

  LOG_TRACE("minkSocket = %p\n", me);
  ERR_CLEAN(MinkSocket_marshalOut(me, &mb->msg.succ, pii, fds, num_fds));

cleanup:
  InvokeInfo_setResult(pii, err);

  return err;
}


static int32_t
MinkSocket_recvInvocationError(MinkSocket *me, msgbuf_t *mb)
{
  int32_t err = Object_OK;
  InvokeInfo *pii = NULL;

  if (mb->hdr.size != sizeof(lxcom_inv_err)) {
    return Object_ERROR_INVALID;
  }

  LOG_TRACE("minkSocket = %p, err = %d\n", me, mb->msg.err.err);
  pii = MinkSocket_getInvokeInfo(me, mb->hdr.invoke_id);
  if (NULL == pii) {
    LOG_ERR("incredible failure to get invokeInfo of invoke_id %d in minksocket %p\
         Leading to caller never be replied\n", mb->hdr.invoke_id, (void *)me);
    return Object_ERROR_UNAVAIL;
  }

  InvokeInfo_setResult(pii, mb->msg.err.err);

  return err;
}

static int32_t MinkSocket_wireToBOArgs(MinkSocket *me, lxcom_inv_req *req,
                                       ObjectArg *args,
                                       void **ppvBuf, int32_t size)
{
  int32_t boSize = 0;
  void *bo;

  FOR_ARGS(i, req->k, BO) {
    args[i].b.size = req->a[i].size;
    if (args[i].b.size != 0)
      boSize += PADDED(args[i].b.size);
    else
      boSize += LXCOM_MSG_ALIGNMENT;
  }

  if (boSize > size) {
    if (boSize > MAX_BUFFER_ALLOCATION - 4) {
      return Object_ERROR_INVALID;
    }
    bo = heap_zalloc(boSize+4);
    if (NULL == bo) {
      return Object_ERROR_INVALID;
    }
    *ppvBuf = bo;
  } else {
    bo = *ppvBuf;
  }

  BBuf bbuf;
  BBuf_construct(&bbuf, bo, boSize+4);
  FOR_ARGS(i, req->k, BO) {
    args[i].b.ptr = BBuf_alloc(&bbuf, req->a[i].size);
  }

  return Object_OK;
}


static int32_t
MinkSocket_recvClose(MinkSocket *me, msgbuf_t *mb)
{
  if (mb->hdr.size != sizeof(lxcom_inv_close)) {
    return Object_ERROR_INVALID;
  }

  LOG_TRACE("minkSocket = %p, table = %p, handle = %d\n", me, &me->table,
             mb->msg.close.handle);
  if (ObjectTableMT_releaseHandle(&me->table, mb->msg.close.handle)) {
    return Object_ERROR;
  }

  if ((VSOCK == me->sockType) && (true == me->closeOnOTEmpty)) {
    if (ObjectTableMT_isCleaned(&me->table)) {
      MinkSocket_close(me, Object_ERROR_UNAVAIL);
      MinkSocket_notifyClose(me, EVENT_CONN_CLOSE);
      MinkSocket_detachForwarderAll(me);
    }
  }

  return Object_OK;
}

static int32_t
MinkSocket_sendInvokeSuccess(MinkSocket *me, lxcom_inv_req *req,
                             ObjectArg *args)
{
  lxcom_inv_succ succ;
  C_ZERO(succ);
  succ.hdr.type = LXCOM_SUCCESS;
  succ.hdr.invoke_id = req->hdr.invoke_id;

  int32_t err;
  int32_t nIov = 0;
  struct iovec iov[LXCOM_MAX_ARGS*2];
  FdWrapper *returnedFdWrapper = NULL;
  MSMem *msmem = NULL;

  // Count the extra args from memparcels
  size_t numExtraArgs = COUNT_NUM_EXTRA_ARGS_OUTBOUND(req->k, args, OO);

  // Check for too many args:
  CHECK_MAX_ARGS(req->k, Out, numExtraArgs);

  err = insertIOV(iov, C_LENGTHOF(iov), &nIov,
                  &succ, ObjectCounts_sizeofInvSucc(req->k, numExtraArgs),
                  &succ.hdr.size);
  if (Object_OK != err) {
    LOG_ERR("err %x from insertIOV 1\n", err);
    if (Object_ERROR_MAXARGS == err) {
      return err;
    }
    return Object_ERROR_INVALID;
  }

  size_t numBO = ObjectCounts_numBO(req->k);
  //Copy BO sizes into header
  for (size_t i=0; i<numBO; ++i) {
    int argi = ObjectCounts_indexBO(req->k)+i;
    succ.a[i].size = args[argi].b.size;

    err = insertIOV(iov, C_LENGTHOF(iov), &nIov,
                  args[argi].b.ptr, args[argi].b.size,
                  &succ.hdr.size);
    if (Object_OK != err) {
      LOG_ERR("err %x from insertIOV 2\n", err);
      return err;
    }
  }

  int fds[ObjectCounts_maxOO];
  int fd_index = 0;
  for (int i=0; i < ObjectCounts_maxOO; i++) {
    fds[i] = -1;
  }

  int iOO = numBO;
  FOR_ARGS(i, req->k, OO) {
    CHECK_OBJ_INDEX_RETURN(iOO);
    if (Object_isNull(args[i].o)) {
      succ.a[iOO].o.flags = 0;
      succ.a[iOO].o.handle = INVALID_OBJECT_HANDLE;
    } else {
      if (isPrimordialOrPrimordialFwd(args[i].o)) {
        LOG_ERR("promordial or primordialFwd cannot be MinkIPC param\n");
        return Object_ERROR;
      }

      int fd = -1;
      if (isWrappedFd(args[i].o, &fd)) {
        LOG_TRACE("sending FdWrapper, fd = %d.\n", fd);
      } else if (isMSMem(args[i].o, &fd)) {
        LOG_TRACE("sending MSMem, fd = %d.\n", fd);
      } else {
        fd = -1;
        LOG_TRACE("can not extract fd.\n");
      }

      MSForwarder *po = MSForwarderFromObject(args[i].o);
      if (po && po->conn == me) {
        //Forwarder already.. peel and send the handle
        succ.a[iOO].o.flags = LXCOM_CALLER_OBJECT;
        succ.a[iOO].o.handle = po->handle;
        LOG_TRACE("MSForwarder object with OO handle = %d, minkSocket = %p\n",
                   po->handle, me);
      } else if (fd > 0) {
        // It's a wrapped fd
        if (!is_remote_sock(me->sockType)) {
          if (NULL == MSMemFromObject(args[i].o)) {
            succ.a[iOO].o.flags = LXCOM_DESCRIPTOR_OBJECT;
            succ.a[iOO].o.handle = INVALID_OBJECT_HANDLE;
            fds[fd_index] = fd;
            fd_index++;
            returnedFdWrapper = FdWrapperFromObject(args[i].o);
            if (returnedFdWrapper == NULL) {
              LOG_ERR("Unexpected error occurs as fdwrapper.\n");
              return Object_ERROR_BADOBJ;
            }
            Object dependency = returnedFdWrapper->dependency;
            if (!Object_isNull(dependency)) {
              MSForwarder *msf = MSForwarderFromObject(dependency);
              if (msf && msf->conn == me ) {
                int h = ObjectTableMT_addObject(&me->table, dependency);
                if (h == -1) {
                  LOG_ERR("Error adding object to table, minksocket = %p, \
                           table = %p\n", me, &me->table);
                  return Object_ERROR_KMEM;
                }
                succ.a[iOO].o.handle = h;
                LOG_TRACE("local MSMem object with fd = %d, msForwarder = %p \
                           handle = %d, minkSocket = %p\n", fd, msf, h, me);
              }
            }
          } else {
            succ.a[iOO].o.flags = LXCOM_DESCRIPTOR_OBJECT;
            int h = ObjectTableMT_addObject(&me->table, args[i].o);
            if (h == -1) {
              LOG_ERR("Error adding object to table, minksocket = %p, \
                       table = %p\n", me, &me->table);
              return Object_ERROR_KMEM;
            }
            succ.a[iOO].o.handle = h;
            fds[fd_index] = fd;
            fd_index++;
            LOG_TRACE("local WrappedFd object with fd = %d, OO handle = %d, \
                       minkSocket = %p\n", fd, h, me);
          }
        } else {
          returnedFdWrapper = FdWrapperFromObject(args[i].o);
          if (NULL == returnedFdWrapper) {
            // This outbound object is a MSMem.

            msmem = MSMemFromObject(args[i].o);
            if (!msmem || !msmem->isLocal) {
              LOG_ERR("Outbound MSMem should be from local.\n");
              return Object_ERROR_BADOBJ;
            }

            // Set outbound SMO allocated from Memory Service for MEM_SHARE
            uint64_t specialRules = ITAccessPermissions_keepSelfAccess;

            // On a remote socket, we need to share the memory via mem-buf
            // Creating new WrappedMemparcel shares the memory with the destination VM.
            WrappedMemparcel* wmp = WrappedMemparcel_new(args[i].o, me);
            if (!wmp) {
              return Object_ERROR_UNAVAIL;
            }

            // Add this to the OT
            int h = ObjectTableMT_addObject(&me->table, WrappedMemparcel_asObject(wmp));
            if (h == -1) {
              WrappedMemparcel_release(wmp);
              return Object_ERROR_KMEM;
            }

            // OT should hold the only reference to wmp
            WrappedMemparcel_release(wmp);

            succ.a[iOO].o.flags = LXCOM_CALLEE_OBJECT | LXCOM_MEMPARCEL_OBJECT;
            succ.a[iOO].o.handle = h;

            // Add memparcel handle information to args array
            // The next four args are used to hold the handle information
            int64_t memparcelHandle = wmp->memparcelHandle;
            for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
              iOO++;
              CHECK_OBJ_INDEX_RETURN(iOO);
              succ.a[iOO].o.flags = LXCOM_MEMPARCEL_INFO;
              succ.a[iOO].o.handle = (uint16_t) (memparcelHandle >> ii*16);
            }

            // Wrap specialRules into next 4 args
            for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
              iOO++;
              CHECK_OBJ_INDEX_RETURN(iOO);
              succ.a[iOO].o.flags = LXCOM_MEMPARCEL_SPECIALRULES;
              succ.a[iOO].o.handle = (uint16_t) (specialRules >> ii * 16);
            }

#ifdef ANDROID
            LOG_TRACE("remote WrappedMemparcel object with wrappedMemparcel \
                      = %p, handle = %d, memparcelHandle = %lld, minkSocket = %p\n",
                      wmp, h, memparcelHandle, me);
#else
            LOG_TRACE("remote WrappedMemparcel object with wrappedMemparcel \
                      = %p, handle = %d, memparcelHandle = %ld, minkSocket = %p\n",
                      wmp, h, memparcelHandle, me);
#endif
          } else {
            // This outbound object is a FdWrapper.

            Object dep = returnedFdWrapper->dependency;

            // This FdWrapper holds a reference to the MSForwarder we want
            MSForwarder* poDep = MSForwarderFromObject(dep);
            if (poDep && poDep->conn == me) {
              // This is the msf for a memory object we created
              //  during MinkSocket_recvInvocationRequest()
              // TODO: this case is a legacy case which is no longer valid.
              succ.a[iOO].o.flags = LXCOM_CALLER_OBJECT | LXCOM_MEMPARCEL_OBJECT;
              succ.a[iOO].o.handle = poDep->handle;
            } else {
              // We will ignore the msf in this memObj's dep
              // if not sending this memObj back via the same connection.
              Object confObj = returnedFdWrapper->confinement;

              if (!Object_isNull(confObj) && (NULL == ConfinementFromObject(confObj))) {
                LOG_ERR("FdWrapper has non-confinement!\n");
                return Object_ERROR_BADOBJ;
              }

              if (Object_isNull(confObj)) {
                if (!Object_isOK(RemoteShareMemory_attachConfinement(NULL, &args[i].o))) {
                  LOG_ERR("Attach default rules of MEM_SHARE failed!\n");
                  return Object_ERROR_UNAVAIL;
                }
                confObj = (FdWrapperFromObject(args[i].o))->confinement;
                LOG_MSG("Adopt default confinement because not configured.\n");
              }

              uint64_t specialRules = 0;
              if (!Object_isOK(IConfinement_getSpecialRules(confObj, &specialRules))) {
                LOG_ERR("Unexpected failure happens when IConfinement_getSpecialRules!\n");
                return Object_ERROR_UNAVAIL;
              }

              // On a remote socket, we need to share the memory via mem-buf
              // Creating new WrappedMemparcel shares the memory with the destination VM.
              WrappedMemparcel* wmp = WrappedMemparcel_new(args[i].o, me);
              if (!wmp) {
                return Object_ERROR_UNAVAIL;
              }

              // Add this to the OT
              int h = ObjectTableMT_addObject(&me->table, WrappedMemparcel_asObject(wmp));
              if (h == -1) {
                WrappedMemparcel_release(wmp);
                return Object_ERROR_KMEM;
              }

              // OT should hold the only reference to wmp
              WrappedMemparcel_release(wmp);

              succ.a[iOO].o.flags = LXCOM_CALLEE_OBJECT | LXCOM_MEMPARCEL_OBJECT;
              succ.a[iOO].o.handle = h;

              // Add memparcel handle information to args array
              // The next four args are used to hold the handle information
              int64_t memparcelHandle = wmp->memparcelHandle;
              for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
                iOO++;
                CHECK_OBJ_INDEX_RETURN(iOO);
                succ.a[iOO].o.flags = LXCOM_MEMPARCEL_INFO;
                succ.a[iOO].o.handle = (uint16_t) (memparcelHandle >> ii*16);
              }

              // Wrap specialRules into next 4 args
              for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
                iOO++;
                CHECK_OBJ_INDEX_RETURN(iOO);
                succ.a[iOO].o.flags = LXCOM_MEMPARCEL_SPECIALRULES;
                succ.a[iOO].o.handle = (uint16_t) (specialRules >> ii * 16);
              }

#ifdef ANDROID
              LOG_TRACE("remote WrappedMemparcel object with wrappedMemparcel \
                        = %p, handle = %d, memparcelHandle = %lld, minkSocket = %p\n",
                        wmp, h, memparcelHandle, me);
#else
              LOG_TRACE("remote WrappedMemparcel object with wrappedMemparcel \
                        = %p, handle = %d, memparcelHandle = %ld, minkSocket = %p\n",
                        wmp, h, memparcelHandle, me);
#endif
            }
          }
        }
      } else {
        //New object, need entry in table
        int h = ObjectTableMT_addObject(&me->table, args[i].o);
        if (h == -1) {
          LOG_ERR("Error adding object to table, minksocket = %p, table = %p\n",
                    me, &me->table);
          return Object_ERROR_KMEM;
        }
        succ.a[iOO].o.flags = LXCOM_CALLEE_OBJECT;
        succ.a[iOO].o.handle = h;
        LOG_TRACE("add object = %p to objectTable = %p with handle = %d, \
                   minkSocket = %p\n", &args[i].o, &me->table, h, me);
      }
    }
    ++iOO;
  }

  vm_osal_mutex_lock(&me->mutex);
  if (-1 == sendv_all(me, iov, nIov, fds, fd_index)) {
    if (errno == ENOMEM) {
      LOG_ERR("memory not available in transport\n");
      err = Object_ERROR_KMEM;
    } else {
      LOG_ERR("Error sending message: returning Object_ERROR_DEFUNCT\n");
      err = Object_ERROR_DEFUNCT;
    }
  }

  vm_osal_mutex_unlock(&me->mutex);
  return err;
}



static int32_t
MinkSocket_sendInvokeError(MinkSocket *me, lxcom_inv_req *req,
                           int32_t error)
{
  int32_t ret;
  lxcom_inv_err err;
  err.hdr.type = LXCOM_ERROR;
  err.hdr.size = sizeof(err);
  err.hdr.invoke_id = req->hdr.invoke_id;
  err.err = error;

  LOG_TRACE("minkSocket = %p, sock = %d, error = %d\n", me, me->sock, error);
  vm_osal_mutex_lock(&me->mutex);
  if (me->sockType == QIPCRTR) {
    ret = qrtr_sendto(me->sock, me->node, me->port, &err, err.hdr.size);

  } else if (me->sockType == VSOCK) {
    struct sockaddr_vm client;
    memset(&client, 0, sizeof(client));
    VM_SOCK_ADDR_POPULATE(client, me->port, me->node);

    ret = sendto(me->sock, &err, err.hdr.size, 0, (struct sockaddr *)&client, sizeof(struct sockaddr_vm));

  } else {
    ret = send_all(me->sock, &err, err.hdr.size);
  }
  vm_osal_mutex_unlock(&me->mutex);

  return (ret == -1) ? Object_ERROR_DEFUNCT : Object_OK;
}


static int32_t
MinkSocket_recvInvocationRequest(MinkSocket *me, msgbuf_t *mb,
                                 ThreadWork *work, int *fds, int num_fds)
{
  int32_t err = Object_OK, errInvoke = Object_OK;
  struct {
    uint8_t buf[MSG_PREALLOCED_SIZE];
    uint64_t _aligned_unused;
  } bufBO;
  void *pvBO = bufBO.buf;
  Object targetObj = Object_NULL;

  //queue up the next read
  if (work) {
   MinkSocket_enqueue(me, work);
   work = NULL;
  }

  // Count the "extra" args not represented in the ObjectCounts
  size_t numExtraArgs = COUNT_NUM_EXTRA_ARGS_INBOUND(
                          mb->msg.req.k,
                          mb->msg.req.a, OI,
                          ObjectCounts_indexOI(mb->msg.req.k));

  CHECK_MAX_ARGS(mb->msg.req.k, In, numExtraArgs);
  size_t size = ObjectCounts_sizeofInvReq(mb->msg.req.k, numExtraArgs);
  if (size > mb->hdr.size) {
    ERR_CLEAN(Object_ERROR_INVALID);
  }

  ObjectArg args[LXCOM_MAX_ARGS] = {{{0,0}}};
  FOR_ARGS(i, mb->msg.req.k, BI) {
    size = PADDED(size);
    if (size > mb->hdr.size) {
      ERR_CLEAN(Object_ERROR_INVALID);
    }

    args[i].b.ptr = (uint8_t *)mb->buf + size;
    args[i].b.size = mb->msg.req.a[i].size;
    size += mb->msg.req.a[i].size;
    if (size > mb->hdr.size) {
      ERR_CLEAN(Object_ERROR_INVALID);
    }
  }

  if (0 != ObjectCounts_numBO(mb->msg.req.k)) {
    ERR_CLEAN(MinkSocket_wireToBOArgs(me, &mb->msg.req, args,
                                      &pvBO, MSG_PREALLOCED_SIZE));
  }

  size_t iOI = ObjectCounts_indexOI(mb->msg.req.k);

  int fd_index = 0;

  FOR_ARGS(i, mb->msg.req.k, OI) {
    CHECK_OBJ_INDEX_CLEAN(iOI);
    uint16_t flags = mb->msg.req.a[iOI].o.flags;
    uint16_t handle = mb->msg.req.a[iOI].o.handle;

    if (flags & LXCOM_CALLER_OBJECT) {
      // new remote object

      if (flags & LXCOM_MEMPARCEL_OBJECT) {
        int64_t memparcelHandle = 0;

        // Assemble the memparcel handle from the next four args entries
        for (int ii=0; ii<NUM_EXTRA_ARGS_PER_MEMPARCEL; ii++) {
          iOI++;
          CHECK_OBJ_INDEX_CLEAN(iOI);
          if (!(mb->msg.req.a[iOI].o.flags & LXCOM_MEMPARCEL_INFO)) {
            ERR_CLEAN(Object_ERROR_UNAVAIL);
          }
          uint16_t handleChunk = mb->msg.req.a[iOI].o.handle;
          memparcelHandle |= ((int64_t)handleChunk << ii*16);
        }

        uint64_t specialRules = 0;
        // Unwrap the specialRules from the next 4 args
        for (int ii = 0; ii < NUM_EXTRA_ARGS_SPECIALRULES; ii++) {
          iOI++;
          CHECK_OBJ_INDEX_CLEAN(iOI);
          if (!(mb->msg.req.a[iOI].o.flags & LXCOM_MEMPARCEL_SPECIALRULES)) {
            ERR_CLEAN(Object_ERROR_UNAVAIL);
          }
          uint16_t handleChunk = mb->msg.req.a[iOI].o.handle;
          specialRules |= ((uint64_t)handleChunk << ii * 16);
        }

        Object wmpMsf = Object_NULL;
        err = MSForwarder_new(me, handle, &wmpMsf);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        ITAccessPermissions_rules *wmpConfRules = HEAP_ZALLOC_REC(ITAccessPermissions_rules);
        if (!wmpConfRules) {
          LOG_ERR("cannot allocate memory for confinement struct!\n");
          ERR_CLEAN(Object_ERROR_KMEM);
        }

        wmpConfRules->specialRules = specialRules;

        Object MSMemObj = Object_NULL;
        err = ShareMemory_GetMSMem(memparcelHandle, MinkSocket_getDestVMName(me), wmpConfRules, &MSMemObj);
        heap_free(wmpConfRules);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        // when MSMemObj is releasing, it should trigger sendClose to remote WMPObj.
        MSMem *returnedMSMem = MSMemFromObject(MSMemObj);
        if (returnedMSMem == NULL) {
          LOG_ERR("Unexpected error occurs as msmem.\n");
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        returnedMSMem->dependency = wmpMsf;

        args[i].o = MSMemObj;
#ifdef ANDROID
        LOG_TRACE("MSMem object with minkSocket = %p, memparcelHandle = %lld \
                   handle = %d, dmaBufFd = %d\n", me, memparcelHandle, handle, returnedMSMem->dmaBufFd);
#else
        LOG_TRACE("MSMem object with minkSocket = %p, memparcelHandle = %ld \
                   handle = %d, dmaBufFd = %d\n", me, memparcelHandle, handle, returnedMSMem->dmaBufFd);
#endif
      } else {
        // Regular caller object, just hand out the MSForwarder
        Object msf;
        err = MSForwarder_new(me, handle, &msf);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        args[i].o = msf;
        LOG_TRACE("receive handle = %d, constructed msForwarder = %p, \
                   minkSocket = %p\n", handle, &msf, me);
      }
    } else if (flags & LXCOM_CALLEE_OBJECT) {
      args[i].o = ObjectTableMT_recoverObject(&me->table, handle);

      if (Object_isNull(args[i].o)) {
        ERR_CLEAN(Object_ERROR_UNAVAIL);
      }

      if (flags & LXCOM_MEMPARCEL_OBJECT) {
        // args[i].o is actually a wrappedmemparcel.
        WrappedMemparcel *wmp = WrappedMemparcel_fromObject(args[i].o);
        if (!wmp) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        // Note that this MSMem will be retained.
        Object MSMemObj = WrappedMemparcel_getWrappedFdObj(wmp);
        if (Object_isNull(MSMemObj)) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }

        // Decrease ref of wrappedmemparcel by 1.
        Object_release(args[i].o);

        args[i].o = MSMemObj;
        iOI += NUM_EXTRA_ARGS_PER_MEMPARCEL;
        iOI += NUM_EXTRA_ARGS_SPECIALRULES;

        LOG_TRACE("Transferred callee wrappedmemparcel to msmem with\
                  minksocket = %p, handle = %d.\n", me, handle);
      } else {
        LOG_TRACE("MSForwarder object with minkSocket = %p, handle = %d\n", me,
                  handle);
      }
    } else if (flags & LXCOM_DESCRIPTOR_OBJECT) {
      if (fd_index >= num_fds) {
        // The expected fd object count doesn't match the number of fds
        // actually received
        ERR_CLEAN(Object_ERROR_UNAVAIL);
      }
      args[i].o = FdWrapper_new(fds[fd_index]);
      if (Object_isNull(args[i].o)) {
        ERR_CLEAN(Object_ERROR_UNAVAIL);
      }

      // the handle is that of msmemObj in tzd's OT
      if (handle != INVALID_OBJECT_HANDLE) {
        Object msf = Object_NULL;
        err = MSForwarder_new(me, handle, &msf);
        if (err) {
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        FdWrapper *returnedFdWrapper = FdWrapperFromObject(args[i].o);
        if (returnedFdWrapper == NULL) {
          LOG_ERR("Unexpected error occurs as fdwrapper.\n");
          ERR_CLEAN(Object_ERROR_UNAVAIL);
        }
        returnedFdWrapper->dependency = msf;
      }
      LOG_TRACE("FdWrapper object with minkSocket = %p, fd = %d, handle = %d\n",
                 me, fds[fd_index], handle);

      //so we don't double close if an error occurs
      fds[fd_index] = -1;
      fd_index++;
    } else {
      LOG_ERR("Found an object with no flags\n");
      args[i].o = Object_NULL;
    }
    iOI++;
  }

  LOG_TRACE("get object out with minkSocket = %p, objectTable = %p, request handle = %d\n",
             me, &me->table, mb->msg.req.handle);
  //get the object out
  targetObj = ObjectTableMT_recoverObject(&me->table, mb->msg.req.handle);
  if (Object_isNull(targetObj)) {
    LOG_ERR("target object %d is NULL !!\n", mb->msg.req.handle);
    err = MinkSocket_sendInvokeError(me, &mb->msg.req, Object_ERROR_INVALID);
    goto cleanup;
  }

  errInvoke = Object_invoke(targetObj, ObjectOp_methodID(mb->msg.req.op),
                            args, mb->msg.req.k);
  if (Object_OK != errInvoke) {
    LOG_ERR("fail to invoke for request on target handle %d, error = %d\n",
         mb->msg.req.handle, errInvoke);
    err = MinkSocket_sendInvokeError(me, &mb->msg.req, errInvoke);
    goto cleanup;
  }

  //send Success unless there is an internal error
  err = MinkSocket_sendInvokeSuccess(me, &mb->msg.req, args);
  if (Object_OK != err) {
    LOG_ERR("fail to sendInvokeSuccess for request on target handle %d,\
         error = %d\n", mb->msg.req.handle, err);
    goto cleanup;
  }

cleanup:

  if (Object_OK != err) {
    MinkSocket_sendInvokeError(me, &mb->msg.req, err);
  }

  if (!Object_isNull(targetObj)) {
    //Release all output Object references since we're done with them
    if (Object_isOK(errInvoke)) {
      FOR_ARGS(i, mb->msg.req.k, OO) {
        Object_ASSIGN_NULL(args[i].o);
      }
    }
    Object_release(targetObj);
  }

  //Release all input Object references since we're done with them
  FOR_ARGS(i, mb->msg.req.k, OI) {
    Object_ASSIGN_NULL(args[i].o);
  }

  if (pvBO != bufBO.buf) {
    heap_free(pvBO);
  }

  for (int i = 0; i < num_fds; i++) {
    //close all fds found on input
    if (fds[i] != -1) {
      vm_osal_mem_close(fds[i]);
    }
  }

  return err;
}

#if defined(VNDR_SMCINVOKE_QRTR) || defined(VNDR_VSOCK)
struct Mink_remote_Context {
  MinkSocket *conn;
  void *data;
  size_t data_len;
};

void* __process_message(void *data)
{
  int32_t err = Object_OK, ret = Object_OK;
  struct Mink_remote_Context *cxt = (struct Mink_remote_Context *)data;
  msgbuf_t *mb = (msgbuf_t *)cxt->data;
  ThreadWork *work = NULL;
  switch (mb->hdr.type) {
    case LXCOM_REQUEST:
      err = MinkSocket_recvInvocationRequest(cxt->conn, mb, work, NULL, 0);
      break;
    case LXCOM_SUCCESS:
      err = MinkSocket_recvInvocationSuccess(cxt->conn, mb, work, NULL, 0);
      break;
    case LXCOM_ERROR:
      err = MinkSocket_recvInvocationError(cxt->conn, mb);
      break;
    case LXCOM_CLOSE:
      err = MinkSocket_recvClose(cxt->conn, mb);
      break;
    default:
      err = Object_ERROR_DEFUNCT;
  }
  if (Object_ERROR_DEFUNCT == err) {
    cxt->conn->dispatchErr = err;
    MinkSocket_close(cxt->conn, err);
    MinkSocket_notifyClose(cxt->conn, EVENT_CONN_CRASH);
    // close all gateway handles as remote
    // cannot trigger a release after fd is closed
    ObjectTableMT_closeAllHandles(&cxt->conn->table);
    ret = MinkSocket_detachForwarderAll(cxt->conn);
  }

  // Object_ERROR_UNAVAIL indicates the MinkSocket instance has been freed in disorder
  if (Object_ERROR_UNAVAIL != ret) {
    MinkSocket_dequeue(cxt->conn);
  }
  free(mb);
  free(data);

  return NULL;
}

int32_t process_message(MinkSocket *me, int sock, void *data, size_t data_len)
{
  /* memory allocated here would be freed in thread handling this req */
  struct Mink_remote_Context *cxt = HEAP_ZALLOC_REC(struct Mink_remote_Context);
  if (!cxt) {
    LOG_ERR("Memory allocation failure for Mink_remote_Context\n");
    return Object_ERROR_KMEM;
  }
  cxt->data = (char *)malloc(data_len);
  if (!cxt->data) {
    LOG_ERR("error memory allocation\n");
    free(cxt);
    return -1;
  }
  memcpy(cxt->data, data, data_len);
  me->sock = sock;
  cxt->conn = me;
  cxt->data_len = data_len;

  ThreadWork* work = NULL;
  if (!me->bDone && me->sock != -1) {
    work = HEAP_ZALLOC_REC(ThreadWork);
    if (!work) {
      free(cxt->data);
      free(cxt);
      return Object_ERROR_KMEM;
    }
    ThreadWork_init(work, __process_message, cxt);
    MinkSocket_enqueue(me, work);
  }
  return 0;
}
#else
int32_t process_message(MinkSocket *me, int sock, void *data, size_t data_len)
{
  return 0;
}
#endif

static int32_t
MinkSocket_reader(MinkSocket *me)
{
  int32_t ret = 0, err = Object_OK;
  bool locked = false;
  msgbuf_t msgbuf;
  msgbuf_t *mb = &msgbuf;

  if (!me->bDone && me->sock != -1) {
    struct pollfd pbits[2];
    pbits[0].fd = me->sockPair[0];
    pbits[0].events = POLLIN;

    pbits[1].fd = me->sock;
    pbits[1].events = POLLIN;

    while(1) {
      ret = vm_osal_poll(pbits, 2, -1);
      if (ret < 0) {
        if (EINTR == errno) {
          LOG_ERR("event occurs before poll(), try again\n");
          continue;
        } else {
          LOG_ERR("poll() failed with %s, connection shutdown\n", strerror(errno));
          return Object_ERROR_DEFUNCT;
        }
      } else {
        break;
      }
    }

    // when endpoint exits proactively, main thread tells all
    // working threads to stop by pipeline message
    if (pbits[0].revents & POLLIN) {
      return Object_ERROR_DEFUNCT;
    }
    // when sockfd closed by other thread, poll() will be waked up by
    // socket events (POLLHUP|POLLIN) or POLLNVAL
    if ((pbits[1].revents & POLLHUP) || (pbits[1].revents & POLLNVAL)) {
      return Object_ERROR_DEFUNCT;
    }

    // hold mutex while minksocket receiving message
    // actual execution of MinkSocket_release() will be delayed until receiving finished
    vm_osal_mutex_lock(&me->mutex);
    locked = true;

    int fds[ObjectCounts_maxOI];
    int hdr_status = recv_msg(me->sock, mb, sizeof(lxcom_hdr),
                              fds, C_LENGTHOF(fds));
    if (hdr_status < 0) {
      err = Object_ERROR_DEFUNCT;
      goto cleanup;
    }

    if (mb->hdr.size > MSG_PREALLOCED_SIZE) {
      if (mb->hdr.size > MAX_BUFFER_ALLOCATION) {
        err = Object_ERROR_INVALID;
        goto cleanup;
      }
      mb = (msgbuf_t *)heap_zalloc(mb->hdr.size);
      if (NULL == mb) {
        err = Object_ERROR_KMEM;
        goto cleanup;
      }
      mb->hdr = msgbuf.hdr;
    }

    int bdy_status = recv_msg(me->sock, (uint8_t *)mb+sizeof(lxcom_hdr),
                      mb->hdr.size - sizeof(lxcom_hdr),
                      fds+hdr_status, C_LENGTHOF(fds)-hdr_status);
    if (bdy_status < 0) {
      for (int i = 0; i < hdr_status; i++) {
        vm_osal_mem_close(fds[i]); //close any fds we picked up previously
      }
      err = Object_ERROR_DEFUNCT;
      goto cleanup;
    }

    vm_osal_mutex_unlock(&me->mutex);
    locked = false;

    ThreadWork* work = NULL;
    if (!me->bDone && me->sock != -1 && err == Object_OK) {
      work = HEAP_ZALLOC_REC(ThreadWork);
      if (!work) {
          return Object_ERROR_KMEM;
      }
      ThreadWork_init(work, MinkSocket_dispatch, me);
      if (mb->hdr.type != LXCOM_REQUEST &&
          mb->hdr.type != LXCOM_SUCCESS) {
        /* Invoke Request and Success messages may have
         * objects to marshal. So they will queue up this
         * work after marshalling objects. For everything else
         * queue up the work now.
         */
        MinkSocket_enqueue(me, work);
      }
    }

    LOG_TRACE("minksocket = %p, sock = %d, type = %d\n", me, me->sock,
               mb->hdr.type);
    switch (mb->hdr.type) {
    case LXCOM_REQUEST:
      err = MinkSocket_recvInvocationRequest(me, mb, work, fds,
                                             hdr_status + bdy_status);
      break;
    case LXCOM_SUCCESS:
      err = MinkSocket_recvInvocationSuccess(me, mb, work, fds,
                                             hdr_status + bdy_status);
      break;
    case LXCOM_ERROR:
      err = MinkSocket_recvInvocationError(me, mb);
      break;
    case LXCOM_CLOSE:
      err = MinkSocket_recvClose(me, mb);
      break;
    default:
      err = Object_ERROR_DEFUNCT;
    }
  }

cleanup:
  if (mb != &msgbuf) {
    heap_free(mb);
  }

  if (locked) {
    vm_osal_mutex_unlock(&me->mutex);
  }

  if (Object_OK != err) {
    LOG_ERR("Failed with error: %d\n", err);
  }

  return err;
}

void *MinkSocket_dispatch(void *pv)
{
  MinkSocket *me = (MinkSocket *)pv;
  CredInfo info = me->credInfo;
  int32_t err = Object_OK, ret = Object_OK;

  if (me->peer_available) {
    vm_osal_store_TLS_key(gMinkPeerUIDTLSKey, &info.uid);
    vm_osal_store_TLS_key(gMinkPeerGIDTLSKey, &info.gid);
  } else {
    vm_osal_store_TLS_key(gMinkPeerUIDTLSKey, NULL);
    vm_osal_store_TLS_key(gMinkPeerGIDTLSKey, NULL);
  }

  LOG_TRACE("minkSocket = %p, uid = %d, gid = %d\n", me, info.uid, info.gid);
  if (MinkSocket_isReady(me)) {
    //if ((me->sockType == QIPCRTR) || (me->sockType == VSOCK)) {
    //    LOG_ERR("Enter: SVM port: %d Remote Reader\n", me->sock);
    //    err = MinkSocket_remote_reader(me);
    //} else {
      err = MinkSocket_reader(me);
    //}
  }

  if (Object_ERROR_DEFUNCT == err) {
    me->dispatchErr = err;
    MinkSocket_close(me, err);
    MinkSocket_notifyClose(me, EVENT_CONN_CRASH);
    // close all gateway handles as remote
    // cannot trigger a release after fd is closed
    ObjectTableMT_closeAllHandles(&me->table);
    ret = MinkSocket_detachForwarderAll(me);
  }

  vm_osal_store_TLS_key(gMinkPeerUIDTLSKey, NULL);
  vm_osal_store_TLS_key(gMinkPeerGIDTLSKey, NULL);

  // Object_ERROR_UNAVAIL indicates the MinkSocket instance has been freed in disorder
  if (Object_ERROR_UNAVAIL != ret) {
    MinkSocket_dequeue(me);
  }

  return NULL;
}

void MinkSocket_start(MinkSocket *me, int sock)
{
  me->sock = sock;

  ThreadWork* work = HEAP_ZALLOC_REC(ThreadWork);
  if (!work) {
    return;
  }
  ThreadWork_init(work, MinkSocket_dispatch, me);
  MinkSocket_enqueue(me, work);

  LOG_MSG("minkSocket = %p, sock = %d, threadWork = %p, threadPool = %p\n", me,
           me->sock, work, me->pool);
}
