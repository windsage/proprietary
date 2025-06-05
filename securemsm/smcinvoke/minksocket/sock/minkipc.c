/********************************************************************
 Copyright (c) 2016-2017, 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include <sys/socket.h>
#include <sys/un.h>
#include "cdefs.h"
#include "check.h"
#include "ConnEventHandler.h"
#include "ConnEventReleaser.h"
#include "heap.h"
#include "IPrimordial.h"
#include "minkipc.h"
#include "minksocket.h"
#include "msforwarder.h"
#include "fdwrapper.h"
#include "Primordial.h"
#include "ProtocolVsock.h"
#include "qlist.h"
#include "MinkTransportUtils.h"
#if defined(VNDR_SMCINVOKE_QRTR)
#include "libqrtr.h"
#define MINK_QRTR_LA_SERVICE_VERSION 1
#define MINK_QRTR_LA_SERVICE_INSTANCE 1
#endif
#if defined(VNDR_VSOCK)
#include <linux/vm_sockets.h>
#endif

#define MAX_QUEUE_LENGTH     5

#define CHECK_CLEAN(expr) \
  do { if (!(expr)) { CHECK_LOG(); goto cleanup; } } while (0)

extern
size_t strlcpy(char *dst, const char *src, size_t size);

extern
int itoa(long in, char *buffer);

typedef union {
    struct sockaddr_un addr;
    #if defined(VNDR_SMCINVOKE_QRTR)
    struct sockaddr_qrtr qaddr;
    #endif
    #if defined(VNDR_VSOCK)
    struct sockaddr_vm vaddr;
    #endif
} sockaddr_t;

struct MinkIPC {
  int refs;
  bool bServer;
  bool bReady;
  sockaddr_t sockaddr;
  SOCK_TYPE sockType;
  int sock;
  bool bDone;
  bool bServerDone;
  Object endpoint;
  Object primordial;
  bool endpointIsModule;
  vm_osal_thread listenerThread;
  MinkSocket *conn;
  int exitMsgPipe[2];
  QList servedConnectionsList;
  vm_osal_mutex mutex;
  vm_osal_cond cond;
};

typedef struct {
  QNode qn;
  MinkSocket *conn;
  uint32_t node; // In case of Vsock node is same as cid
  uint32_t port;
} ServerNode;

vm_osal_key gMinkPeerUIDTLSKey;
vm_osal_key gMinkPeerGIDTLSKey;

int32_t MinkIPC_registerConnEventHandler(Object target, Object handler, Object *releaser)
{
  int32_t res;
  MSForwarder *fwd;
  Object counterPmd = Object_NULL;
  Object subReleaser = Object_NULL;

  fwd = MSForwarderFromObject(target);
  if (NULL == fwd) {
    // if the param handler is ConnEventHander type
    // it must be the initial invocation, which supports MSFwd only
    if (isConnEventHandler(handler)) {
      LOG_ERR("ConnEventHandler only support MSForwarder\n");
      return Object_ERROR;
    // while the param handler is not ConnEventHandler type
    // it must be recursive invocations, where non-MSFwd means end.
    }

    return Object_OK;

  } else {
    if (MSForwarder_derivePrimordial(fwd, &counterPmd)) {
      LOG_ERR("fail to get promordial\n");
      return Object_ERROR;
    }

    res = IPrimordial_registerHandler(counterPmd, target, handler, &subReleaser);
    if (res) {
      LOG_ERR("fail to invoke primordial %d\n", res);
      Object_ASSIGN_NULL(counterPmd);
      return Object_ERROR;
    }

    Object_ASSIGN_NULL(counterPmd);
  }

  LOG_TRACE("constructed ConnEventReleaser = %p, handler = %p, subReleaser = %p\n",
             releaser, &handler, &subReleaser);
  res = ConnEventReleaser_new(handler, subReleaser, releaser);
  if (res) {
    LOG_ERR("ConnEventReleaser_new failed %d\n", res);
    return Object_ERROR;
  }

  MSForwarder_attachConnEventReleaser(target, *releaser);

  return Object_OK;
}

/*@brief: Close all affiliated MinkSocket connections after MinkIPC set to be bDone
 *        Considering performance impact, the list is not protected by mutex because
 *        nothing added/removed during iteration. Or user take care of the mutex.
*/
static
void MinkIPC_disconnect(MinkIPC *me)
{
  QNode *pqn = NULL, *pqn_next = NULL;
  ServerNode* node = NULL;

  if (me->bServer) {
    QLIST_NEXTSAFE_FOR_ALL(&me->servedConnectionsList, pqn, pqn_next) {
      node = c_containerof(pqn, ServerNode, qn);
      if (node) {
        MinkSocket_close(node->conn, Object_ERROR_UNAVAIL);
      }
    }
  }
}

/*@brief: Much performance sacrificed if introduce mutex to protect list operation here
 *        As a compromise, caller must ensure there is no race condition invoking
 *
 * @param[deadOnly] discriminate cleanup closed all connections or not
*/
static void MinikIPC_cleanupConnections(MinkIPC *me, bool deadOnly) {
  QNode * pqn = NULL;
  QNode * pqn_next = NULL;
  QLIST_NEXTSAFE_FOR_ALL(&me->servedConnectionsList, pqn, pqn_next) {
    ServerNode* node = c_containerof(pqn, ServerNode, qn);
    if (node) {
      if (!MinkSocket_isConnected(node->conn) || !deadOnly) {
        QNode_dequeue(pqn);
        MinkSocket_release(node->conn);
        heap_free(node);
      }
    }
  }
}

static
ServerNode *ServerNode_new(MinkIPC *me, int32_t fd, uint32_t node, uint32_t port)
{
  ServerNode *connection = HEAP_ZALLOC_REC(ServerNode);
  if (!connection) {
    return NULL;
  }

  MinkSocket *sock = MinkSocket_new(me->endpoint, me->sockType, fd, node, port);
  if (!sock) {
    heap_free(connection);
    return NULL;
  }
  LOG_MSG("constructed minksocket = %p with sock = %d, node = %d, port = %d, \
           sockType = %d, belongs to minkipc = %p\n",sock, fd, node, port,
           me->sockType, me);

  if (UNIX == me->sockType) {
    CHECK_CLEAN (!MinkSocket_populatePeerIdentity(sock, fd));
  } else {
    CHECK_CLEAN (!MinkSocket_populatePeerIdentity_remote(sock, node, port));
  }

  CHECK_CLEAN (!MinkSocket_initPrimordial(sock, me->primordial));
  CHECK_CLEAN (!MinkSocket_initObjectTable(sock, me->endpoint, me->endpointIsModule));

  connection->conn = sock;
  connection->node = node;
  connection->port= port;
  return connection;

cleanup:
  MinkSocket_release(sock);
  heap_free(connection);
  return NULL;
}

static ServerNode *get_server_node(MinkIPC *me, uint32_t c_node, uint32_t c_port, bool noCreate)
{
  QNode * pqn = NULL;
  ServerNode* node = NULL;
  QLIST_FOR_ALL(&me->servedConnectionsList, pqn) {
    node = c_containerof(pqn, ServerNode, qn);
    if (node) {
      if (node->node == c_node && node->port == c_port) {
        return node;
      }

      /* Currently, VSOCK has to work in single connection mode because it doesn't support close
       * detection. Close the existing connection when receiving request to establish new one
       * When MinkSocket be closed and detached, the threads within it may be still working
       * so the cleanup is delayed until receiving next connecting request, by invocatoin of
       * MinikIPC_cleanupConnections()
      */
      if (me->sockType == VSOCK) {
        MinkSocket_close(node->conn, Object_ERROR_UNAVAIL);
        MinkSocket_notifyClose(node->conn, EVENT_CONN_CLOSE);
        MinkSocket_detachForwarderAll(node->conn);
      }
    }
  }

  /* this will be set when client has been killed and we need to do cleanup */
  if (noCreate)
    return NULL;

  node = ServerNode_new(me, -1, c_node, c_port);
  if (!node) {
    return node;
  }
  QList_appendNode(&me->servedConnectionsList, &node->qn);
  return node;
}

static void disconnect_cleanup(MinkIPC *me, uint32_t c_node)
{
  LOG_ERR("node = %d\n", c_node);

  // check if is modem node and return
  if (c_node) {
      LOG_ERR("NOT a modem SSR, skip\n");
      return;
  }

  QNode * pqn = NULL;
  QNode * pqn_next = NULL;
  ServerNode* node = NULL;
  QLIST_NEXTSAFE_FOR_ALL(&me->servedConnectionsList, pqn, pqn_next) {
    node = c_containerof(pqn, ServerNode, qn);
    if (node) {
      if (node->node == c_node) {
        QNode_dequeue(pqn);
        MinkSocket_close(node->conn, Object_ERROR_UNAVAIL);
        MinkSocket_notifyClose(node->conn, EVENT_CONN_CRASH);
        MinkSocket_detachForwarderAll(node->conn);
        MinkSocket_release(node->conn);
        free(node);
        LOG_ERR("dequeue node and close connection\n");
      }
    } else {
        LOG_ERR("node = %d not found\n", c_node);
    }
  }
}

#if defined(VNDR_SMCINVOKE_QRTR)
static void *MinkIPC_QRTR_Service(MinkIPC *me)
{
  char *buf = NULL;
  buf = (char *)malloc(MAX_QRTR_PAYLOAD);
  if (!buf) {
    LOG_ERR("Mem allocation failure for UDP packet\n");
    goto serverDone;
  }

  vm_osal_pollfd pfd[2];
  sockaddr_t sq, sq_server;
  socklen_t sl = sizeof(struct sockaddr_qrtr);
  int32_t rc = -1, len = -1;
  int32_t timeout = -1;
  struct qrtr_packet pkt;
  if (getsockname(me->sock, (void *)&sq_server, &sl)) {
    LOG_ERR("Invalid socket, errno is %s\n", strerror(errno));
    goto serverDone;
  }

  pfd[0].fd = me->sock;
  pfd[0].events = POLLIN;
  pfd[1].fd = me->exitMsgPipe[0];
  pfd[1].events = POLLIN;

  do {
    rc = vm_osal_poll(pfd, 2, timeout);
    if (rc < 0) {
      if (EINTR == errno) {
        LOG_ERR("event occurs before poll(), try again\n");
        continue;
      } else {
        LOG_ERR("poll() failed with %s, connection shutdown\n", strerror(errno));
        goto serverDone;
      }
    }

    if ((pfd[1].revents & POLLHUP) || (pfd[1].revents & POLLNVAL) || (pfd[1].revents & POLLERR)) {
      LOG_ERR("%s:, poll pfd[1] error, tid: %lx: minkipc: %p sock %d, revents: %d\n", __func__,
             vm_osal_thread_self(), me, me->sock, pfd[1].revents);
      goto serverDone;
    }
    if (pfd[1].revents & POLLIN) {
      LOG_ERR("minkipc: %p sock %d , received exit message, exitMsgPipe[0]: %d, exitMsgPipe[1]:%d\n",
              me, me->sock, me->exitMsgPipe[0], me->exitMsgPipe[1]);
      goto serverDone;
    }
    if ((pfd[0].revents & POLLHUP) || (pfd[0].revents & POLLNVAL) || (pfd[0].revents & POLLERR)) {
      LOG_ERR("%s: poll pfd[0], error tid: %lx: minkipc: %p sock %d, revents: %d\n", __func__,
             vm_osal_thread_self(), me, me->sock, pfd[0].revents);
      goto serverDone;
    }
    if (pfd[0].revents & POLLIN) {
      len = recvfrom(me->sock, (void*)buf, MAX_QRTR_PAYLOAD, 0, (void*)&sq, &sl);
      if (len < 0) {
        /* EINTR is thread was signaled, ignore the signal and retry */
        rc = errno;
        if (rc == EINTR || rc == EAGAIN) {
          continue;
        }
        LOG_ERR("%s: tid: %lx: minkipc: %p sock %d returning errno %d\n", __func__,
               vm_osal_thread_self(), me, me->sock, errno);
        goto serverDone;
      }
    }

    rc = qrtr_decode(&pkt, buf, len, (struct sockaddr_qrtr *)&sq);
    if (pkt.type == QRTR_TYPE_NEW_SERVER || pkt.type == QRTR_TYPE_DEL_SERVER) {
      if (pkt.node == 0 && pkt.port == 0)
        continue;
    }

    if (pkt.type == QRTR_TYPE_DEL_CLIENT) {
      LOG_MSG("minkipc = %p sock = %d, client with node %x port %x went down\n",
               me, me->sock, pkt.node, pkt.port);
      ServerNode *node = get_server_node(me, pkt.node, pkt.port, true);
      if (node) {
        QNode_dequeue(&node->qn);
        MinkSocket_close(node->conn, Object_ERROR_UNAVAIL);
        MinkSocket_notifyClose(node->conn, EVENT_CONN_CLOSE);
        MinkSocket_detachForwarderAll(node->conn);
        MinkSocket_release(node->conn);
        free(node);
        continue;
      }
    } else if (pkt.type == QRTR_TYPE_DEL_SERVER) {
      LOG_MSG("minkipc = %p sock = %d, server with node %x port %x went down\n",
               me, me->sock, pkt.node, pkt.port);
      // we live on broadcat network. so check if its our server
      if (pkt.node == me->sockaddr.qaddr.sq_node &&
          pkt.port == me->sockaddr.qaddr.sq_port) {
        LOG_MSG("minkipc = %p sock = %d, Server went down, closing connection\n",
                 me, me->sock);
        MinkSocket_close(me->conn, Object_ERROR_UNAVAIL);
        /* Minksocket_close wakes up all threads waiting for response from server
         * however due to scheduling, those threads are  scheduled later than this
         * thread. This causes crash as MinkSocket_delete would delete full socket
         * data and invoke thread tries to access socket data. So sleep few usec to
         * to give invoke thread a chance to run before this one.
         */
        usleep(5);
        MinkSocket_notifyClose(me->conn, EVENT_CONN_CLOSE);
        MinkSocket_detachForwarderAll(me->conn);
        MinkSocket_release(me->conn);
        /* Assign NULL to me->conn inorder to ensure that Minksocket_release is not
         * called for same socket in MinkIPC_stop.
         */
        me->conn = NULL;
        goto serverDone;
      }
    } else if (pkt.type == QRTR_TYPE_BYE) {
      LOG_MSG("minkipc = %p sock = %d, subsystem with node %x port %x went down\n",
               me, me->sock, pkt.node, pkt.port);
      disconnect_cleanup(me, pkt.node);
      continue;
    }

    if (pkt.type != QRTR_TYPE_DATA) {
      continue;
    }

    if (me->bServer) {
      ServerNode *node = get_server_node(me, sq.qaddr.sq_node, sq.qaddr.sq_port, false);
      if (!node) {
        LOG_ERR("minkipc  = %p sock = %d, QRTR get server node failed\n",
                 me, me->sock);
        goto serverDone;
      }
      process_message(node->conn, me->sock, pkt.data, pkt.data_len);
    } else {
      process_message(me->conn, me->sock, pkt.data, pkt.data_len);
    }
  } while(!me->bDone);

serverDone:
  if (buf) {
    heap_free(buf);
  }

  vm_osal_mutex_lock(&me->mutex);
  MinkIPC_disconnect(me);
  me->bServerDone = true;
  vm_osal_cond_set(&me->cond);
  vm_osal_mutex_unlock(&me->mutex);

  return NULL;
}
#else
static void *MinkIPC_QRTR_Service(MinkIPC *me)
{
  return NULL;
}
#endif

#if defined(VNDR_VSOCK)
static void *MinkIPC_VSOCK_Service(MinkIPC *me)
{
  char *buf = NULL;
  buf = (char *)malloc(MAX_VSOCK_PAYLOAD);
  if (!buf) {
    LOG_ERR("Mem allocation failure for UDP packet\n");
    goto serverDone;
  }

  sockaddr_t sq, sq_server;
  socklen_t sl = sizeof(struct sockaddr_vm);
  int32_t rc = -1, len = -1;

  if (getsockname(me->sock, (void *)&sq_server, &sl)) {
    LOG_ERR("minkipc = %p sock %d, Invalid socket, errno is %s\n",
             me, me->sock, strerror(errno));
    goto serverDone;
  }

  do {
    len = recvfrom(me->sock, (void*)buf, MAX_VSOCK_PAYLOAD, 0, (void*)&sq, &sl);
    if (len < 0) {
      /* EINTR is thread was signaled, ignore the signal and retry */
      rc = errno;
      if (rc == EINTR || rc == EAGAIN)
        continue;
      LOG_ERR("minkipc = %p sock %d returning errno %d\n", me, me->sock, errno);
      goto serverDone;
    }

    if (me->bDone) {
      break;
    }

    //Check and clean up resouce of closed connection
    //Within receiving process, it is the only invoking
    //Before receiving process exits, there isn't anyone would launch another invoking
    //So we don't need mutex protection here
    MinikIPC_cleanupConnections(me, true);

    if (me->bServer) {
      ServerNode *node = get_server_node(me, sq.vaddr.svm_cid, sq.vaddr.svm_port, false);
      if (!node) {
        LOG_ERR("minkipc = %p, VSOCK get server node failed\n", me);
        goto serverDone;
      }
      process_message(node->conn, me->sock, buf, len);
    } else {
      process_message(me->conn, me->sock, buf, len);
    }
  } while(!me->bDone);

serverDone:
  if (buf) {
    heap_free(buf);
  }

  vm_osal_mutex_lock(&me->mutex);
  MinkIPC_disconnect(me);
  me->bServerDone = true;
  vm_osal_cond_set(&me->cond);
  vm_osal_mutex_unlock(&me->mutex);


  return NULL;
}
#else
static void *MinkIPC_VSOCK_Service(MinkIPC *me)
{
  return NULL;
}
#endif

static void *MinkIPC_remote_Service(void *pv)
{
  MinkIPC *me = (MinkIPC *)pv;
  vm_osal_mutex_lock(&me->mutex);
  me->bReady = true;
  vm_osal_cond_set(&me->cond);
  vm_osal_mutex_unlock(&me->mutex);

  if (me->sockType == QIPCRTR)
    return MinkIPC_QRTR_Service(me);
  else if (me->sockType == VSOCK)
    return MinkIPC_VSOCK_Service(me);
  else
    return NULL;
}

static void *MinkIPC_service(void *pv)
{
  MinkIPC *me = (MinkIPC *)pv;

  if (me == NULL) {
    LOG_ERR("Abort as me is NULL unexpectedly\n");
    return NULL;
  }
  vm_osal_mutex_lock(&me->mutex);
  me->bReady = true;
  vm_osal_cond_set(&me->cond);
  vm_osal_mutex_unlock(&me->mutex);

  do {
    int sock = accept(me->sock, NULL, NULL);
    if (sock > 0) {
      ServerNode *node = ServerNode_new(me, sock, -1, -1);
      if (node) {
        vm_osal_mutex_lock(&me->mutex);
        QList_appendNode(&me->servedConnectionsList, &node->qn);
        MinkSocket_start(node->conn, sock);
        //Check and clean up resouce of closed connection
        //Within accepting process, it is the only invoking
        //Before accepting process exits, there isn't anyone would launch another invoking
        //So we don't need mutex protection here
        MinikIPC_cleanupConnections(me, true);
        vm_osal_mutex_unlock(&me->mutex);
      } else {
        shutdown(sock, SHUT_RDWR);
        vm_osal_socket_close(sock);
      }
    }
    if (me == NULL) {
        LOG_ERR("Abort as me is NULL unexpectedly\n");
        return NULL;
    }
  } while(!me->bDone);

  vm_osal_mutex_lock(&me->mutex);
  MinkIPC_disconnect(me);
  me->bServerDone = true;
  vm_osal_cond_set(&me->cond);
  vm_osal_mutex_unlock(&me->mutex);

  return NULL;
}

static MinkIPC *
MinkIPC_new(const char *address, int32_t sock, Object endpoint, int32_t sockType)
{
  MinkIPC *me = HEAP_ZALLOC_REC(MinkIPC);
  if (!me) {
    return NULL;
  }

  me->refs = 1;
  me->bDone = false;
  me->conn = NULL;

  if (address) {
    me->sockaddr.addr.sun_family = AF_UNIX;
    strlcpy(me->sockaddr.addr.sun_path, address, sizeof(me->sockaddr.addr.sun_path) - 1);
    me->sock = socket(AF_UNIX, SOCK_STREAM, 0);
  } else {
    me->sock = sock;
  }

  if (vm_osal_pipe(me->exitMsgPipe) < 0) {
    LOG_ERR("Failed to initialize the exitMsgPipe\n");
    goto cleanup;
  }

  me->sockType = sockType;
  QList_construct(&me->servedConnectionsList);
  Object_ASSIGN(me->endpoint, endpoint);
  me->bServer = !(Object_isNull(endpoint));
  CHECK_CLEAN (me->sock != -1);
  CHECK_CLEAN (!vm_osal_mutex_init(&me->mutex, NULL));
  CHECK_CLEAN (!vm_osal_cond_init(&me->cond, NULL));

  return me;

 cleanup:
  LOG_ERR("release minkIPC = %p\n", me);
  MinkIPC_release(me);
  return NULL;
}

#if defined(VNDR_SMCINVOKE_QRTR)
int32_t MinkIPC_QRTR_new(MinkIPC *me, int32_t addrValue)
{
    int32_t fd = qrtr_open(0);
    if (fd < 0) {
      LOG_ERR("qrtr_open failed: errno %d\n", errno);
      return -1;
    }

    if (qrtr_new_lookup(fd, addrValue,
                                MINK_QRTR_LA_SERVICE_VERSION,
                                MINK_QRTR_LA_SERVICE_INSTANCE) == -1) {
      LOG_ERR("lookup failed\n");
      vm_osal_socket_close(fd);
      return -1;
    }

    struct sockaddr_qrtr sq = {};
    struct qrtr_packet pkt = {};
    socklen_t sl = 0;
    char buf[4096];
    int recvd_buf = 0;

    while (1) {
      sl = sizeof(sq);
      recvd_buf = recvfrom(fd, buf, sizeof(buf), 0, (void *)&sq, &sl);
      if (recvd_buf < 0) {
        LOG_ERR("did not recv control pkt from server: errno = %d\n", errno);
        continue;
      }
      qrtr_decode(&pkt, buf, recvd_buf, &sq);
      if (pkt.node == 0 && pkt.port == 0)
        break;

      me->sockaddr.qaddr.sq_family = AF_QIPCRTR;
      me->sockaddr.qaddr.sq_node = pkt.node;
      me->sockaddr.qaddr.sq_port = pkt.port;
    }
    me->sock = fd;
    return 0;
}
#else
int32_t MinkIPC_QRTR_new(MinkIPC *me, int32_t addrValue)
{
    return 0;
}
#endif


#if defined(VNDR_VSOCK)
int32_t MinkIPC_vsock_new(MinkIPC *me, int32_t addrValue)
{
    int32_t fd = -1;

    fd = ProtocolVsock_constructFd();
    if (fd < 0) {
        LOG_ERR("vsock_open failed: errno %d\n", errno);
        return -1;
    }
    me->sockaddr.vaddr.svm_family = AF_VSOCK;
    me->sockaddr.vaddr.svm_cid = VMADDR_CID_ANY; // Not used by underlying transport layer
    me->sockaddr.vaddr.svm_port = addrValue; // Is address can be used for port on client or hardcoded?

    me->sock = fd;
    return 0;
}
#else
int32_t MinkIPC_vsock_new(MinkIPC *me, int32_t addrValue)
{
   return 0;
}
#endif

static
MinkIPC *MinkIPC_remote_new(int32_t addrValue, int32_t sock, Object endpoint, int32_t sockType)
{
  if (addrValue < 0) {
    return NULL;
  }

  MinkIPC *me = HEAP_ZALLOC_REC(MinkIPC);
  int32_t ret;
  if (!me) {
    return NULL;
  }

  me->refs = 1;
  me->bDone = false;
  me->conn = NULL;

  if (sock < 0) {
    if (sockType == QIPCRTR) {
      ret = MinkIPC_QRTR_new(me, addrValue);
    } else {
      ret = MinkIPC_vsock_new(me, addrValue);
    }

    if (ret != 0) {
      goto cleanup;
    }
  } else {
    me->sock = sock;
  }

  if (vm_osal_pipe(me->exitMsgPipe) < 0) {
    LOG_ERR("Failed to initialize the exitMsgPipe\n");
    goto cleanup;
  }

  me->sockType = sockType;
  QList_construct(&me->servedConnectionsList);
  Object_ASSIGN(me->endpoint, endpoint);
  me->bServer = !(Object_isNull(endpoint));
  CHECK_CLEAN (me->sock != -1);
  CHECK_CLEAN (!vm_osal_mutex_init(&me->mutex, NULL));
  CHECK_CLEAN (!vm_osal_cond_init(&me->cond, NULL));

  return me;

 cleanup:
  LOG_ERR("release minkIPC = %p\n", me);
  MinkIPC_release(me);
  return NULL;
}

MinkIPC *MinkIPC_beginService(const char *address, int32_t sock, int32_t sockType,
                                     Object endpoint, int32_t endpointType)
{
  MinkIPC *me = NULL;

  MinkTransportUtils_configTrace();

  if (QIPCRTR == sockType || VSOCK == sockType) {
    me = MinkIPC_remote_new(0, sock, endpoint, sockType);
  } else {
    me = MinkIPC_new(address, sock, endpoint, sockType);
  }
  if (!me) {
    LOG_ERR("MinkIPC_new failed\n");
    return NULL;
  }

  if (endpointType == MODULE)
    me->endpointIsModule = true;

  CHECK_CLEAN(!vm_osal_create_TLS_key(&gMinkPeerUIDTLSKey, NULL));
  CHECK_CLEAN(!vm_osal_create_TLS_key(&gMinkPeerGIDTLSKey, NULL));

  CHECK_CLEAN (!Primordial_new(&me->primordial));
  LOG_MSG("constructed primordial = %p, belongs to minkIPC = %p\n",
           (me->primordial).context, me);
  CHECK_CLEAN (!Primordial_setRegisterHandlerFunc(&me->primordial,
                                             MinkIPC_registerConnEventHandler));

  if (address != NULL) {
    if (VSOCK == sockType) {
      #if defined(VNDR_VSOCK)
      struct sockaddr_vm svm;
      memset(&svm, 0, sizeof(svm));
      svm.svm_family = AF_VSOCK;
      svm.svm_port = VSOCK_PORT_NO;
      svm.svm_cid = VMADDR_CID_ANY;
      CHECK_CLEAN (!bind(me->sock, (struct sockaddr*)&svm, sizeof(svm)));
      #endif
    }
    if (UNIX == sockType || SIMULATED == sockType) {
      //Recreate the file if one exists already
      unlink(me->sockaddr.addr.sun_path);
      CHECK_CLEAN (!bind(me->sock, (struct sockaddr*)&me->sockaddr.addr,
                    sizeof(me->sockaddr.addr)));
    }
  }

  if ((sockType == QIPCRTR) || (sockType == VSOCK)) {
    CHECK_CLEAN (!vm_osal_thread_create(&me->listenerThread, MinkIPC_remote_Service, me, NULL));
  } else {
    CHECK_CLEAN (!listen(me->sock, MAX_QUEUE_LENGTH) );
    CHECK_CLEAN (!vm_osal_thread_create(&me->listenerThread, MinkIPC_service, me, NULL));
  }

  vm_osal_mutex_lock(&me->mutex);
  while (!me->bReady) {
    vm_osal_cond_wait(&me->cond, &me->mutex, NULL);
  }
  vm_osal_mutex_unlock(&me->mutex);

  LOG_MSG("constructed minkipc = %p, sock = %d, socktype = %d, endpoint = %p, \
           primordial = %p\n", me, me->sock, me->sockType, &endpoint, &me->primordial);
  return me;

cleanup:
  if (!Object_isNull(me->primordial)) {
    Object_ASSIGN_NULL(me->primordial);
  }
  MinkIPC_release(me);
  return NULL;
}

MinkIPC *MinkIPC_startService(const char *address, Object endpoint)
{
  return MinkIPC_beginService(address, -1, UNIX, endpoint, OPENER);
}

MinkIPC * MinkIPC_startServiceOnSocket(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, UNIX, endpoint, OPENER);
}

MinkIPC *MinkIPC_startServiceModule(const char *address, Object endpoint)
{
  return MinkIPC_beginService(address, -1, UNIX, endpoint, MODULE);
}

MinkIPC * MinkIPC_startServiceModuleOnSocket(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, UNIX, endpoint, MODULE);
}

MinkIPC *MinkIPC_startServiceModule_simulated(const char *address, Object endpoint)
{
  return MinkIPC_beginService(address, -1, SIMULATED, endpoint, MODULE);
}

MinkIPC * MinkIPC_startServiceModuleOnSocket_simulated(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, SIMULATED, endpoint, MODULE);
}

MinkIPC * MinkIPC_startServiceOnSocket_vsock(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, VSOCK, endpoint, OPENER);
}

MinkIPC * MinkIPC_startServiceModuleOnSocket_vsock(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, VSOCK, endpoint, MODULE);
}

MinkIPC * MinkIPC_startServiceOnSocket_QRTR(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, QIPCRTR, endpoint, OPENER);
}

MinkIPC * MinkIPC_startServiceModuleOnSocket_QRTR(int32_t sock, Object endpoint)
{
  return MinkIPC_beginService(NULL, sock, QIPCRTR, endpoint, MODULE);
}

/**
   wait for the service to finish ..
   waits until stopped or the service dies
**/
void MinkIPC_join(MinkIPC *me) {
  if (me->bServer && me->listenerThread) {
    //wait for thread to die
    vm_osal_thread_join(me->listenerThread, NULL);
    me->listenerThread = 0;
  }
}

static
int32_t MinkIPC_populateSockAddr(MinkIPC *me, int32_t sockType, struct sockaddr **paddr,
                                 uint32_t *addrSize, uint32_t *node, uint32_t *port)
{
  if (NULL == me || NULL == paddr) {
    LOG_ERR("invalid input parameters\n");
    return Object_ERROR;
  }
  if (NULL == addrSize || NULL == node || NULL == port) {
    LOG_ERR("invalid input parameters\n");
    return Object_ERROR;
  }

  if (QIPCRTR == sockType) {
    #if defined(VNDR_SMCINVOKE_QRTR)
    *paddr = (struct sockaddr *)&me->sockaddr.qaddr;
    *addrSize = sizeof(me->sockaddr.qaddr);
    *node = me->sockaddr.qaddr.sq_node;
    *port = me->sockaddr.qaddr.sq_port;
    #endif

  } else if (VSOCK == sockType) {
    #if defined(VNDR_VSOCK)
    *paddr = (struct sockaddr *)&me->sockaddr.vaddr;
    *addrSize = sizeof(me->sockaddr.vaddr);
    *node = me->sockaddr.vaddr.svm_cid;
    *port = me->sockaddr.vaddr.svm_port;
    #endif

  } else if (UNIX == sockType || SIMULATED == sockType) {
    *paddr = (struct sockaddr *)&me->sockaddr.addr;
    *addrSize = sizeof(me->sockaddr.addr);
    *node = 0;
    *port = 0;

  } else {
    LOG_ERR("invalid sockTpye\n");
    return Object_ERROR;
  }

  return Object_OK;
}

MinkIPC* MinkIPC_connect_common(const char *address, Object *proxyOut, int32_t sockType)
{
  struct sockaddr *addr = NULL;
  uint32_t addrSize = 0, node = 0, port = 0;
  MinkIPC *me = NULL;

  MinkTransportUtils_configTrace();

  if (QIPCRTR == sockType || VSOCK == sockType) {
    me = MinkIPC_remote_new(atoi(address), -1, Object_NULL, sockType);
  } else {
    me = MinkIPC_new(address, -1, Object_NULL, sockType);
  }
  if (!me) {
    LOG_ERR("MinkIPC_new failed\n");
    return NULL;
  }

  CHECK_CLEAN (!Primordial_new(&me->primordial));
  LOG_TRACE("constructed primordial = %p, belongs to minkIPC = %p\n",
             (me->primordial).context, me);
  CHECK_CLEAN (!Primordial_setRegisterHandlerFunc(&me->primordial,
                                                 MinkIPC_registerConnEventHandler));

  CHECK_CLEAN (!MinkIPC_populateSockAddr(me, sockType, &addr, &addrSize, &node, &port));
  CHECK_CLEAN (!connect(me->sock, addr, addrSize));

  me->conn = MinkSocket_new(Object_NULL, sockType, me->sock, node, port);
  CHECK_CLEAN (NULL != me->conn);
  MinkSocket_initPrimordial(me->conn, me->primordial);

  if (sockType == QIPCRTR || sockType == VSOCK) {
    CHECK_CLEAN (!vm_osal_thread_create(&me->listenerThread, MinkIPC_remote_Service, me, NULL));
    vm_osal_mutex_lock(&me->mutex);
    while (!me->bReady) {
      vm_osal_cond_wait(&me->cond, &me->mutex, NULL);
    }
    vm_osal_mutex_unlock(&me->mutex);
  } else {
    MinkSocket_start(me->conn, me->sock);
  }

  CHECK_CLEAN (!MSForwarder_new(me->conn, GENERIC_HANDLE, proxyOut));

  LOG_MSG("constructed minkipc = %p, sock = %d, socktype = %d, minksocket = %p, \
           primordial = %p, msforwarder = %p\n", me, me->sock, me->sockType,
           me->conn, (me->primordial).context, proxyOut->context);
  return me;

cleanup:
  LOG_ERR("ERROR releasing minkIPC = %p\n", me);
  if (!Object_isNull(me->primordial)) {
    Object_ASSIGN_NULL(me->primordial);
  }
  MinkIPC_release(me);
  return NULL;
}

MinkIPC* MinkIPC_connect(const char *address, Object *proxyOut)
{
  return MinkIPC_connect_common(address, proxyOut, UNIX);
}

MinkIPC* MinkIPC_connect_simulated(const char *address, Object *proxyOut)
{
  return MinkIPC_connect_common(address, proxyOut, SIMULATED);
}

MinkIPC *MinkIPC_connectModule(const char *address, Object *proxyOut)
{
  return MinkIPC_connect_common(address, proxyOut, UNIX);
}

MinkIPC *MinkIPC_connectModule_simulated(const char *address, Object *proxyOut)
{
  return MinkIPC_connect_common(address, proxyOut, SIMULATED);
}

#if defined(VNDR_SMCINVOKE_QRTR)
MinkIPC* MinkIPC_connect_QRTR(int32_t addrValue, Object *proxyOut)
{
  char address[MAX_SOCKADDR_LEN] = {0};
  itoa(addrValue, address);
  return MinkIPC_connect_common(address, proxyOut, QIPCRTR);
}

MinkIPC* MinkIPC_connectModule_QRTR(int32_t addrValue, Object *proxyOut)
{
  return MinkIPC_connect_QRTR(addrValue, proxyOut);
}
#else
MinkIPC* MinkIPC_connect_QRTR(int32_t addrValue, Object *proxyOut)
{
  return NULL;
}

MinkIPC* MinkIPC_connectModule_QRTR(int32_t addrValue, Object *proxyOut)
{
  return NULL;
}
#endif

#if defined(VNDR_VSOCK)
MinkIPC* MinkIPC_connect_vsock(int32_t addrValue, Object *proxyOut)
{
  char address[MAX_SOCKADDR_LEN] = {0};
  itoa(addrValue, address);
  return MinkIPC_connect_common(address, proxyOut, VSOCK);
}

MinkIPC* MinkIPC_connectModule_vsock(int32_t addrValue, Object *proxyOut)
{
  return MinkIPC_connect_vsock(addrValue, proxyOut);
}

#else
MinkIPC* MinkIPC_connect_vsock(int32_t addrValue, Object *proxyOut)
{
  return NULL;
}

MinkIPC* MinkIPC_connectModule_vsock(int32_t addrValue, Object *proxyOut)
{
  return NULL;
}

#endif

int MinkIPC_getClientInfo(uid_t* uid, gid_t* gid)
{
  gid_t* pgid = (gid_t*) vm_osal_retrieve_TLS_key(gMinkPeerGIDTLSKey);
  uid_t* puid = (uid_t*) vm_osal_retrieve_TLS_key(gMinkPeerUIDTLSKey);

  if (pgid == NULL || puid == NULL)
    return -1;

  *uid = *puid;
  *gid = *pgid;

  return 0;
}

static void MinkIPC_stop(MinkIPC *me)
{
  vm_osal_mutex_lock(&me->mutex);
  me->bDone = true;

  if (me->sock != -1) {
    if (me->sockType == QIPCRTR && me->exitMsgPipe[1] > 0) {
      LOG_TRACE("minkipc %p write to exitMsgPipe, exitMsgPipe[0]:%d, exitMsgPipe[1]:%d\n",
                 me, me->exitMsgPipe[0], me->exitMsgPipe[1]);
      if (write(me->exitMsgPipe[1], "\0", 1) <= 0) {
        LOG_ERR("write to exitMsgPipe failed\n");
      }
    }

    if ((!me->bServer) && (UNIX == me->sockType)) {
      //Only for client & unix protocol, MinkIPC and its MinkSocket member share the same sockfd
      //Ensure the sockfd just be closed once to avoid unexpected failure, especially other
      //thread of the process has occupied that fd again (OS alway allocates fd of minimum value)
      MinkSocket_close(me->conn, Object_ERROR_UNAVAIL);
    } else {
      shutdown(me->sock, SHUT_RDWR);
      vm_osal_socket_close(me->sock);
    }
    me->sock = -1;
  }
  vm_osal_mutex_unlock(&me->mutex);

  if (me->listenerThread) {
    //Wait for thread to die, but we cannot join here, since the caller
    //might have caller MinkIPC_join. So let's use cond for it
    vm_osal_mutex_lock(&me->mutex);
    while (!me->bServerDone) {
      vm_osal_cond_wait(&me->cond, &me->mutex, NULL);
    }
    vm_osal_thread_join(me->listenerThread, NULL);
    me->listenerThread = 0;
    vm_osal_mutex_unlock(&me->mutex);
  }

  vm_osal_mutex_lock(&me->mutex);
  if (me->conn) {
    MinkSocket_release(me->conn);
    me->conn = NULL;
  }
  vm_osal_mutex_unlock(&me->mutex);

  //MinikIPC_cleanupConnections() is not allowed to be invoked in parallel
  //Because it contains list operation but lack mutex protection for performance
  //bServerDone flag indicates receiving/accepting process has exited
  //Thus it is the only invoking in whole MinkIPC, not need mutex protection
  MinikIPC_cleanupConnections(me, false);
}

void MinkIPC_retain(MinkIPC *me)
{
  vm_osal_atomic_add(&me->refs, 1);
}

void MinkIPC_release(MinkIPC *me)
{
  if (vm_osal_atomic_add(&me->refs, -1) == 0) {
    LOG_MSG("released minkipc = %p, minksocket = %p, primordial = %p, sock = %d, \
             endpoint = %p\n", me, me->conn, (me->primordial).context, me->sock,
             &me->endpoint);

    MinkIPC_stop(me);

    if (me->exitMsgPipe[0] > 0) {
      vm_osal_fd_close(me->exitMsgPipe[0]);
      vm_osal_fd_close(me->exitMsgPipe[1]);
      me->exitMsgPipe[0] = -1;
      me->exitMsgPipe[1] = -1;
    }

    Object_ASSIGN_NULL(me->endpoint);
    Object_ASSIGN_NULL(me->primordial);
    vm_osal_mutex_deinit(&me->mutex);
    vm_osal_cond_deinit(&me->cond);
    heap_free(me);
  }
}

void MinkIPC_wrapFd(int fd, Object *obj) {
  *obj = FdWrapper_new(fd);
}
