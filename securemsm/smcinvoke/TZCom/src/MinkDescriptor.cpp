/********************************************************************
Copyright (c) 2016-2019,2021-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#define LOG_TAG "SmcInvoke_MinkDescriptor"
//#define LOG_NDEBUG 0	//comment out to disable ALOGV
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <string>
#include <vector>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/smcinvoke.h>
#include "MinkDescriptor.h"
#include "InvokeUtils.h"
#include <sys/mman.h>
#include <linux/dma-buf.h>
#include <limits.h>
#include <setjmp.h>
#include "qlist.h"
#include "fdwrapper.h"
#include "cdefs.h"

#ifdef ANDROID
#include <utils/Log.h>
#endif
#ifdef OE
#include <stdint.h>
#include <signal.h>
#include <memory>
#ifdef ENABLE_QTVM_PRINT_LOGGING
#define ALOGV PRINTF
#define ALOGD PRINTF
#define ALOGE PRINTF
#else
#include <syslog.h>
#define ALOGV(...) syslog(LOG_NOTICE,"INFO:" __VA_ARGS__)
#define ALOGD(...) syslog(LOG_DEBUG,"INFO:" __VA_ARGS__)
#define ALOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#endif
#endif

using namespace std;

#define MAX_LOCOBJ_HANDLE	-2
#define MAX_OBJ_ARG_COUNT	(ObjectCounts_maxBI + \
				ObjectCounts_maxBO + \
				ObjectCounts_maxOI + \
				ObjectCounts_maxOO)
#ifdef SMCINVOKE_LOG_BUF_SIZE
#define LOG_LINE_LENGTH_SANITY_CHECK (SMCINVOKE_LOG_BUF_SIZE-45)
#endif

class MinkCBContext : public std::enable_shared_from_this<MinkCBContext> {

public:

	MinkCBContext();

	~MinkCBContext();

	int getObjectFromHandle(int handle, Object &obj);

	int getHandleFromObject(Object obj, int &handle);

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
private:
	int mServerFd = -1;

	size_t mCBBufLen;

	std::vector<Object> mLocObjTab;

	QList memRegions;

	int mLocObjCnt = 0;

	std::vector<pthread_t> mTidTab;

	pthread_mutex_t mLocObjTableMutex;

	pthread_mutex_t mShutdownMutex;

	pthread_mutex_t mSyncMutex;

	pthread_mutex_t mMemObjTableMutex;

	pthread_cond_t mSyncCondVar;

	bool is_accept_threads_spawned;

	static void *worker(void *arg);

	void *worker2(MinkCBContext* me);

public:
	MinkCBContext(size_t cbthread_cnt, size_t cbbuf_len, int cb_fd);

	int getServerFd() {return mServerFd;};

	int getLocObjHandle(Object locObj, int *handle);

	int addMemObjToList(Object locobj, int32_t handle);

	int clearLocalMemObj(int32_t handle);

	int clearLocalObj(int handle);

	int getLocalObj(int32_t handle, Object *localobj);

	bool check_dma_buf_fd(int fd);

	int marshalRequestIn(
		struct smcinvoke_accept *req, ObjectArg *args);

	int marshalResponseOut(
		struct smcinvoke_accept*req, ObjectArg *args);

	void WaitTillAcceptThreadsAreCreated();

	void shutdown();
#endif
};

__thread sigjmp_buf thread_exit_point;

typedef struct MinkDescriptor {
	int fd;
	int refs;
	shared_ptr<MinkCBContext>  cbctx;
#ifdef SMCINVOKE_IOCTL_LOG	/* provide log info if kernel supports the smcinvoke logging feature */
	/* The following implement a way for this code to enhance the
	 * smcinvoke driver's debug/trace log.  (ObjectOp_METHOD_MASK-3) is
	 * a special ObjectOp intended for code using MinkDescriptor to pass
	 * that info to us (kept temporarily in log_lines), which we decorate
	 * and pass on to the driver via a special ioctl code.
	 */
	int logging_on;
	pthread_mutex_t logging_mutex;
	std::queue<std::string> log_lines;
#endif
} MinkDescriptor;

#define SMCINVOKE_ARGS_ALIGN_SIZE	(sizeof(uint64_t))
#define ACPT 1

static Object MinkDescriptor_new(int fd, shared_ptr<MinkCBContext> cbcxt);

static int invoke_over_ioctl(ObjectCxt cxt,
			ObjectOp op,
			ObjectArg *args,
			ObjectCounts counts);

static int remap_error(int err)
{
    switch (err) {
    case ENOMEM:
        return Object_ERROR_KMEM;
    case EBUSY:
        return Object_ERROR_BUSY;
    default:
        break;
    }

    return Object_ERROR_UNAVAIL;
}

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
int MinkCBContext::getLocObjHandle(Object locObj, int32_t *handle)
{
	int ret = Object_OK;
	size_t i;

	if (handle == NULL) {
		ALOGE("NULL handle\n");
		return Object_ERROR_BADOBJ;
	}
	pthread_mutex_lock(&mLocObjTableMutex);
	for (i = 0; i < mLocObjTab.size(); i++) {
		if (Object_isNull(mLocObjTab[i])) {
			*handle = MAX_LOCOBJ_HANDLE - i;
			Object_ASSIGN(mLocObjTab[i], locObj);
			break;
		}
	}

	if (i == mLocObjTab.size()) {
		mLocObjTab.push_back(Object_NULL);
		*handle = MAX_LOCOBJ_HANDLE - i;
		Object_ASSIGN(mLocObjTab[i], locObj);
	}
	mLocObjCnt ++;
	ALOGV("getLocObjHandle %d, cnt %d\n", *handle, mLocObjCnt);

	pthread_mutex_unlock(&mLocObjTableMutex);
	return ret;
}

int MinkCBContext::addMemObjToList(Object locobj, int32_t handle){

	int ret = Object_OK;
	pthread_mutex_lock(&mMemObjTableMutex);
	if (locobj.invoke == FdWrapper_invoke) {
		FdWrapper *ctx = (FdWrapper *)locobj.context;
		if (ctx != NULL) {
			QList_appendNode(&memRegions,&(ctx->qn));
		} else {
			ALOGE("Failed to get fdwrapper context for mem loc obj");
			ret = Object_ERROR_BADOBJ;
		}
	} else {
		ALOGE("%d handle is not a memory object.", handle);
		ret = Object_ERROR_BADOBJ;
	}
	pthread_mutex_unlock(&mMemObjTableMutex);
	return ret;

}

int MinkCBContext::clearLocalMemObj(int32_t handle) {

	int32_t ret = Object_ERROR;
	QNode* pqn= NULL;
	pthread_mutex_lock(&mMemObjTableMutex);
	QLIST_FOR_ALL(&memRegions, pqn) {
		FdWrapper* ob = c_containerof(pqn, struct FdWrapper, qn);
		if(ob->descriptor == handle)  {
			ALOGE("Handle found in qnode, clearing mem obj");
			FdWrapper_release(ob);
			ret = Object_OK;
			break;
		}
	}
	pthread_mutex_unlock(&mMemObjTableMutex);
	return ret;

}

int MinkCBContext::clearLocalObj(int32_t handle)
{
	int32_t ret = Object_OK;
	size_t i = MAX_LOCOBJ_HANDLE - (size_t)handle;

	if (handle > MAX_LOCOBJ_HANDLE) {
		ALOGE("Invalid handle %d\n", handle);
		return Object_ERROR_BADOBJ;
	}

	pthread_mutex_lock(&mLocObjTableMutex);
	if (i >= mLocObjTab.size()) {
		ALOGE("Invalid local object handle %d\n", handle);
		ret = Object_ERROR_BADOBJ;
		goto exit;
	}
	Object_ASSIGN_NULL(mLocObjTab[i]);
	mLocObjCnt --;
	ALOGV("clearLocObj %d, cnt %d\n", handle, mLocObjCnt);

exit:
	pthread_mutex_unlock(&mLocObjTableMutex);
	return ret;
}

int MinkCBContext::getLocalObj(int32_t handle, Object *localobj)
{
	if (handle > MAX_LOCOBJ_HANDLE || localobj == NULL) {
		ALOGE("NULL localobj or invalid handle %d\n", handle);
		return Object_ERROR_BADOBJ;
	}

	int32_t ret = Object_OK;
	size_t i = MAX_LOCOBJ_HANDLE - (size_t)handle;

	pthread_mutex_lock(&mLocObjTableMutex);
	if (i >= mLocObjTab.size() || Object_isNull(mLocObjTab[i])) {
		ALOGE("Invalid local object handle %d\n", handle);
		ret = Object_ERROR_BADOBJ;
	} else
		Object_ASSIGN(*localobj, mLocObjTab[i]);

	pthread_mutex_unlock(&mLocObjTableMutex);
	ALOGV("Get local obj idx = %zu, handle = %d", i, handle);
	return ret;
}

bool MinkCBContext::check_dma_buf_fd(int fd)
{
	struct dma_buf_sync buf_sync;
	int ret = 0;
	buf_sync.flags = ~DMA_BUF_SYNC_VALID_FLAGS_MASK;

	ret = ioctl(fd, DMA_BUF_IOCTL_SYNC, &buf_sync);
	if (ret) {
		if (errno == EINVAL) {
			ALOGV("DMA_BUF_IOCTL_SYNC test errno = %d, detected a dma_buf fd %d",
				errno, fd);
			return true;
		}
	}
	return false;
}

#endif

int MinkCBContext::getObjectFromHandle(int handle, Object &obj)// *-->&
{
	int ret = 0;

	if (handle == SMCINVOKE_USERSPACE_OBJ_NULL) {
		/* NULL object*/
		Object_ASSIGN_NULL(obj);
	} else if (handle > SMCINVOKE_USERSPACE_OBJ_NULL) {
		/* remote object */
		obj = MinkDescriptor_new(handle, shared_from_this());
		ALOGV("%s: handle = %d, obj.invoke=%p, obj.context=%p", __func__, handle, (void*) obj.invoke, (void*) obj.context);
		if (Object_isNull(obj))
			ret = Object_ERROR_BADOBJ;
	} else {
#ifdef SMCINVOKE_IOCTL_SERVER_REQ
		/* local object */
		ret = getLocalObj(handle, &obj);
		if (Object_isNull(obj))
			ret = Object_ERROR_BADOBJ;
#else
		ALOGD("CBobj not supported\n");
		ret = Object_ERROR_BADOBJ;
#endif
	}
	if (ret)
		ALOGE("Failed to get obj for handle %d", handle);

	return ret;
}

int MinkCBContext::getHandleFromObject(Object obj, int &handle)
{
	int ret = 0;

	if (Object_isNull(obj)) {
		/* set NULL Object's fd to be -1 */
		handle = SMCINVOKE_USERSPACE_OBJ_NULL;
		return ret;
	}

	if (obj.invoke == invoke_over_ioctl) {
		/* kernel object */
		MinkDescriptor *ctx = (MinkDescriptor *)(obj.context);
		if (ctx != NULL)
			handle = ctx->fd;
		else {
			ALOGE("Failed to get MinkDescriptor obj handle, ret = %d", ret);
			ret = Object_ERROR_BADOBJ;
		}
	} else {
#ifdef SMCINVOKE_IOCTL_SERVER_REQ
		ret = Object_unwrapFd(obj, &handle);
		if (ret == 0 && handle >= 0) {
			if (check_dma_buf_fd(handle)) {
				/* This is a memory object */
				ALOGV("get memobj handle %d", handle);
			} else {
				ALOGE("invalid memobj handle %d", handle);
				ret = Object_ERROR_BADOBJ;
			}
		} else {
			/* This is a local callback object */
			ret = getLocObjHandle(obj, &handle);
			if (ret) {
				ALOGE("Failed to get local obj handle, ret = %d", ret);
			}
		}
#else
		ALOGD("CBobj not supported\n");
		ret = Object_ERROR_BADOBJ;
#endif
	}

	return ret;
}

class MinkCBException : public exception
{
	virtual const char* what() const throw() {
		return "Failed to create cb thread";
	}
} cbex;

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
int MinkCBContext::marshalRequestIn(
		struct smcinvoke_accept *req, ObjectArg *args)
{
	int ret = 0;
	union smcinvoke_arg *argptr = (union smcinvoke_arg *)(uintptr_t)req->buf_addr;

	FOR_ARGS(i, req->counts, BI) {
		args[i].bi.ptr = (void *)(uintptr_t)argptr[i].b.addr;
		args[i].bi.size = argptr[i].b.size;
	}

	FOR_ARGS(i, req->counts, BO) {
		args[i].b.ptr = (void *)(uintptr_t)argptr[i].b.addr;
		args[i].b.size = argptr[i].b.size;
	}

	FOR_ARGS(i, req->counts, OI) {
		ret = getObjectFromHandle(argptr[i].o.fd, args[i].o);
		if (ret) {
			break;
		}
	}

	if (ret) {
		FOR_ARGS(i, req->counts, OI) {
			Object_ASSIGN_NULL(args[i].o);
		}
		ret = Object_ERROR_BADOBJ;
	}

	return ret;

}

int MinkCBContext::marshalResponseOut(
		struct smcinvoke_accept*req, ObjectArg *args)
{
	int ret = 0;
	union smcinvoke_arg *argptr = (union smcinvoke_arg *)(uintptr_t)req->buf_addr;

	FOR_ARGS(i, req->counts, BO) {
		argptr[i].b.size = args[i].b.size;
	}

	FOR_ARGS(i, req->counts, OO) {
		int handle = 0;
		ret |= getHandleFromObject(args[i].o, handle);
		argptr[i].o.fd = handle;
		argptr[i].o.cb_server_fd = mServerFd;
		if (check_dma_buf_fd(handle)) {
			/* This is a memory object */
			ret = addMemObjToList(args[i].o,handle);
			if (ret) {
				ALOGE("Failed to add memobj handle:%d to list, ret: %d", handle, ret);
				// Reset ret value to zero and continue the marshalling out process.
				ret = 0;
			}
		}
	}

	// release all local handles if OO failed
	if (ret) {
		FOR_ARGS(i, req->counts, OO) {
			if (argptr[i].o.fd <= MAX_LOCOBJ_HANDLE) //MACRO
				clearLocalObj(argptr[i].o.fd);
		}
	}
	return ret;
}

void MinkCBContext::WaitTillAcceptThreadsAreCreated()
{
	pthread_mutex_lock(&mSyncMutex);
	while (!is_accept_threads_spawned)
		pthread_cond_wait(&mSyncCondVar, &mSyncMutex);
	pthread_mutex_unlock(&mSyncMutex);
}

void *MinkCBContext::worker(void *args)
{
	return(((MinkCBContext *)args)->worker2((MinkCBContext *)args));
}

void *MinkCBContext::worker2(MinkCBContext* me)
{
	int ret = 0;
	struct smcinvoke_accept req;
	ObjectArg objArgs[MAX_OBJ_ARG_COUNT] = {{{0, 0}}};
	pthread_t tid = pthread_self();

	memset(&req, 0, sizeof(req));
	req.has_resp = false;
	req.argsize = sizeof(union smcinvoke_arg);
	req.buf_len = mCBBufLen;
	req.buf_addr = (uintptr_t)ZALLOC_ARRAY(uint8_t, mCBBufLen);
	if (!req.buf_addr) {
		ALOGE("Failed to allocate request buffer size %zu\n", mCBBufLen);
		return NULL;
	}

	if (sigsetjmp(thread_exit_point, 1) == ACPT) {
		goto out;
	}

	//Unblock SIGUSR1 now
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

	while (1) {
		ALOGV(" cbthread: tid %ld, send IOCTL_ACCEPT_REQ: has_response %d, txn_id %d, result %d, cbobj_id %d, op 0x%x, counts 0x%x,\n\n",
			tid, req.has_resp, req.txn_id, req.result, req.cbobj_id, req.op, req.counts);
		ret = ioctl(mServerFd, SMCINVOKE_IOCTL_ACCEPT_REQ, &req);
		if (ret) {

			req.has_resp = false;
			if (errno == EINTR) {
				ALOGD("cbthread[tid %ld] interrupted, continue \n", tid);
				continue;
			} else if (errno == EAGAIN) {
				ALOGD("cbthread[tid %ld] continue with errno %d\n", tid, errno);
				continue;
			} else if (errno == ESHUTDOWN) {
				ALOGE("cbthread[tid %ld] shutdown server %d with errno %d\n", tid, mServerFd, errno);
				close(mServerFd);
				break;
			} else {
				ALOGE("cbthread[tid %ld] return %d, errno %d: has_response %d, txn_id %d, result %d, cbobj_id %d, op 0x%x, counts 0x%x,\n",
                                tid, ret, errno, req.has_resp, req.txn_id, req.result, req.cbobj_id, req.op, req.counts);
				break;
			}
		}

		req.has_resp = true;

		if (ObjectOp_methodID(req.op) != Object_OP_release) {
			Object locObj = Object_NULL;
			req.result = getLocalObj(req.cbobj_id, &locObj);
			if (req.result) {
				ALOGE("Faild to get LocalObj from handle %d",req.cbobj_id);
				continue;
			}
			memset((void *)objArgs, 0, sizeof(ObjectArg)*MAX_OBJ_ARG_COUNT);
			req.result = marshalRequestIn(&req, objArgs);
			if (req.result) {
				ALOGE("marshalRequestIn failed with result %d", req.result);
				continue;
			}
			ALOGV("Invoke CBObj: context %p, cbobj_id %d, invoke %p, op 0x%x, counts 0x%x\n",
				locObj.context, req.cbobj_id, locObj.invoke, req.op, req.counts);

			req.result = Object_invoke(locObj, req.op, objArgs, req.counts);

			//release tz & local objs in OIs
			FOR_ARGS(i, req.counts, OI) {
				Object_release(objArgs[i].o);
			}
			if (req.result == 0) {
				req.result = marshalResponseOut(&req, objArgs);
			}
			Object_release(locObj);
		} else {
			if (check_dma_buf_fd(req.cbobj_id)) {
				ret = clearLocalMemObj(req.cbobj_id);
				if (ret) {
					ALOGE("mem obj with id :%d not found in list", req.cbobj_id);
				}
			} else {
				clearLocalObj(req.cbobj_id);
			}
		}
		ALOGV("locObj table size = %d\n", mLocObjCnt);
	}

out:
	free((void *)(req.buf_addr));
	pthread_exit(NULL);
	return NULL;
}

void MinkCBContext::shutdown()
{
	pthread_mutex_lock(&mShutdownMutex);

	pthread_mutex_lock(&mLocObjTableMutex);
	for (vector<Object>::iterator it = mLocObjTab.begin();
		it != mLocObjTab.end(); ++it) {
		Object_ASSIGN_NULL(*it);
	}
	pthread_mutex_unlock(&mLocObjTableMutex);

	//Send SIGUSR1 to all threads so that they can do cleanup and exit
	while (!mTidTab.empty()) {
		pthread_t tid = mTidTab.back();
		pthread_kill(tid, SIGUSR1);
		pthread_join(tid, NULL);
		mTidTab.pop_back();
		ALOGV("cbthread[tid %ld] is killed\n", tid);
	}

	if (mServerFd != -1) {
		close(mServerFd);
		mServerFd = -1;
	}

	pthread_mutex_unlock(&mShutdownMutex);

	}

void minkcbserver_signal_handler(int sig)
{
#ifndef OE
	/*syslog is not a async-signal-safe function and seen to cause undefined
	 *behaviour, like forver stuck here during testing in LE
	 */
	ALOGV("SIGUSR1 detected - to cancel ioctl operation, sig = %d\n", sig);
#endif
	/*Use sigsetjmp and siglongjmp to handle the cleanup in shutdown
	 *path. When a call to shutdown is made, send SIGUSR1 signal to all
	 *the threads. Inside the signal handler, make a long jump to a pre-
	 *set point in code from where cleanup and exit of pthreads is handled.
	 *Potential problems with  siglongjmp and sigsetjmp have been evaluated.
	 *No memory leak, No risk of lock, No dependent release sequence and don't
	 *care about stack corruption since we are anyway going to exit.
	 */
	siglongjmp(thread_exit_point, ACPT);
}

MinkCBContext::MinkCBContext(size_t cbthread_cnt,
			size_t cbbuf_len, int server_fd) :
			mServerFd(server_fd), mCBBufLen(cbbuf_len)
{
	pthread_attr_t attr;
	size_t i = 0;
	int ret = Object_OK;
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = minkcbserver_signal_handler;
	sigaction(SIGUSR1, &action, NULL);
	siginterrupt(SIGUSR1, true); /* allow SIGUSR1 to interrupt system call */

	/*Block the SIGUSR1 signal until the thread_exit_point is set in worker2 thread.
	 *This makes sure that when SIGUSR1 is received, the signal handler behaves as expected.
	 */
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	pthread_mutex_init(&mLocObjTableMutex, 0);
	pthread_mutex_init(&mShutdownMutex, 0);
	pthread_mutex_init(&mSyncMutex, 0);
	pthread_mutex_init(&mMemObjTableMutex, 0);
	pthread_mutex_lock(&mSyncMutex);
	is_accept_threads_spawned = false;
	QList_construct(&memRegions);
	pthread_mutex_unlock(&mSyncMutex);
	pthread_cond_init(&mSyncCondVar, 0);
	pthread_attr_init(&attr);
	// SAFETY: See notice in MinkCBContext::shutdown before changing this!
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < cbthread_cnt; i++) {
		pthread_t tid;
		ret = pthread_create(&tid, &attr, worker, (void *)this);
		if (ret) {
			ALOGE("Faile to create cb thread %zu", i);
			shutdown();
			throw cbex;
		} else {
			ALOGV("Created cbthread[%zu]: tid = %ld, server_id = %d\n", i, tid, server_fd);
			mTidTab.push_back(tid);
		}
	}
	pthread_mutex_lock(&mSyncMutex);
	is_accept_threads_spawned = true;
	pthread_mutex_unlock(&mSyncMutex);
	pthread_cond_signal(&mSyncCondVar);

	ALOGV("Succeed to open invoke driver, server_fd = %d", server_fd);
}

MinkCBContext::~MinkCBContext()
{
	shutdown();
	ALOGV("close server_fd %d\n", mServerFd);
}

#else
MinkCBContext::MinkCBContext() {
	ALOGD("MinkCBContext() is called without CBobj support\n");
}

MinkCBContext::~MinkCBContext()
{
	ALOGV("~MinkCBContext() is called");
}

#endif

static void MinkDescriptor_delete(MinkDescriptor *me)
{
	ALOGV("Delete object: context = %p, fd = %d", me, me->fd);
	if (me->fd >= 0)
		close(me->fd);
	else {
#ifdef SMCINVOKE_IOCTL_SERVER_REQ
		me->cbctx->clearLocalObj(me->fd);
#endif
	}
	me->cbctx.reset();
	free(me);
}

static int marshalIn(struct smcinvoke_cmd_req *req, union smcinvoke_arg *argptr,
			ObjectOp op, ObjectArg *args, ObjectCounts counts, MinkDescriptor *me)
{
	req->op = op;
	req->counts = counts;
	req->argsize = sizeof(union smcinvoke_arg);
	req->args = (uintptr_t)argptr;

	FOR_ARGS(i, counts, Buffers) {
		argptr[i].b.addr = (uintptr_t) args[i].b.ptr;
		argptr[i].b.size = args[i].b.size;
	}

	FOR_ARGS(i, counts, OI) {
		int handle = -1;
		int ret = me->cbctx->getHandleFromObject(args[i].o, handle);
		if (ret) {
			ALOGE("invalid OI[%zu]", i);
			return Object_ERROR_BADOBJ;
		}
		argptr[i].o.fd = handle;

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
		/* get server id for callback object */
		if (handle <= MAX_LOCOBJ_HANDLE || me->cbctx->check_dma_buf_fd(handle)) {
			ALOGV("OI[%d], handle = %d, server_fd = %d",
				(int)i, handle, me->cbctx->getServerFd());
			argptr[i].o.cb_server_fd = me->cbctx->getServerFd();
		}
#endif
	}

	FOR_ARGS(i, counts, OO) {
		/* init OO's fd to -1, to avoid close(fd=0) in the failed case of ioctl */
		argptr[i].o.fd = SMCINVOKE_USERSPACE_OBJ_NULL;
	}
	return Object_OK;
}

static int marshalOut(struct smcinvoke_cmd_req *req, union smcinvoke_arg *argptr,
			ObjectArg *args, ObjectCounts counts, MinkDescriptor *me)
{
	int ret = req->result;
	bool failed = false;

	argptr = (union smcinvoke_arg *)(uintptr_t)(req->args);

	FOR_ARGS(i, counts, BO) {
		args[i].b.size = argptr[i].b.size;
	}

	FOR_ARGS(i, counts, OO) {
		ret = me->cbctx->getObjectFromHandle(argptr[i].o.fd, args[i].o);
		if (ret) {
			ALOGE("Failed to marshalOut OO[%zu], handle = %d",
				i, (int)argptr[i].o.fd);
			failed = true;
			break;
		}
#ifdef SMCINVOKE_IOCTL_SERVER_REQ
		if (argptr[i].o.fd <= MAX_LOCOBJ_HANDLE) {
			ret = ioctl(me->cbctx->getServerFd(), SMCINVOKE_IOCTL_ACK_LOCAL_OBJ, &(argptr[i].o.fd));
			if (ret) {
				ALOGE("Failed to send IOCTL_ACK_LOCAL_OBJ for OO[%zu], handle = %d",
					i, (int)argptr[i].o.fd);
				failed = true;
				break;
			}
		}
#endif
		ALOGV("%s: fd=0x%x, o.invoke=%p, o.context=%p",
			__func__, (int)argptr[i].o.fd, args[i].o.invoke, args[i].o.context);
		ALOGV("Succeed to create MinkDescriptor obj for args[%zu].o, fd = %d",
			i, (int)argptr[i].o.fd);
	}
	if (failed) {
		FOR_ARGS(i, counts, OO) {
			Object_ASSIGN_NULL(args[i].o);
		}
		/* Only overwrite ret value if invoke result is 0 */
		if (ret == 0)
			ret = Object_ERROR_BADOBJ;
	}
	return ret;
}

static int invoke_over_ioctl(ObjectCxt cxt,
			ObjectOp op,
			ObjectArg *args,
			ObjectCounts counts)
{
	int ret = Object_OK;
	struct smcinvoke_cmd_req req = {0};

	memset(&req, 0, sizeof(struct smcinvoke_cmd_req));

	/* initialize each OO as NULL obj, then they can be safely released in error cases. */
	FOR_ARGS(i, counts, OO) {
		args[i].o = Object_NULL;
	}

	MinkDescriptor *me = (MinkDescriptor*) cxt;
	if ( me == NULL ) {
		ALOGE("cxt is Null ");
		return Object_ERROR_BADOBJ;
	}

	ObjectOp method = ObjectOp_methodID(op);

	if (ObjectOp_isLocal(op)) {
		switch (method) {
		case Object_OP_retain:
		    if (me->refs > INT_MAX - 1)
		        return Object_ERROR_MAXDATA;
		    atomic_add(&me->refs, 1);
		    return Object_OK;
		case Object_OP_release:
		    if (me->refs < 1)
		        return Object_ERROR_MAXDATA;
		    if (atomic_add(&me->refs, -1) == 0) {
		        MinkDescriptor_delete(me);
		    }
		    return Object_OK;
#ifdef SMCINVOKE_IOCTL_LOG	/* queue log info if kernel supports the smcinvoke logging feature */
		case (ObjectOp_METHOD_MASK - 3):
			if (me->logging_on) {
				if (counts != ObjectCounts_pack(1, 0, 0, 0)) {
					ALOGE("%s: ERROR op=0x%x, counts=0x%x", __func__, op, counts);
					break;
				}
				if (args[0].bi.size > LOG_LINE_LENGTH_SANITY_CHECK) {
					/* not fatal error, but need to draw attention to the caller's string being truncated */
					ALOGE("%s: ERROR, SMCINVOKE_IOCTL_LOG args[0].bi.size=%zu > %d, %s",
							__func__, args[0].bi.size, LOG_LINE_LENGTH_SANITY_CHECK,
							(const char*)args[0].bi.ptr);
				}
				pthread_mutex_lock(&me->logging_mutex);
				std::string line((const char*)args[0].bi.ptr, (int)args[0].bi.size);
				me->log_lines.push(line);
				pthread_mutex_unlock(&me->logging_mutex);
				ALOGV("%s: SMCINVOKE_IOCTL_LOG line=%s", __func__, line.c_str());
				return Object_OK;
			}
#endif
		}
		return Object_ERROR_REMOTE;
	}

	union smcinvoke_arg *argptr = (union smcinvoke_arg *)calloc(ObjectCounts_total(counts), sizeof(union smcinvoke_arg));
	if (argptr == NULL)
		return Object_ERROR_KMEM;

	ret = marshalIn(&req, argptr, op, args, counts, me);
	if (ret) {
		goto exit;
	}
#ifdef SMCINVOKE_IOCTL_LOG	/* provide log info if kernel supports the smcinvoke logging feature */
	/* if smcinvoke logging isn't turned off, and we have something, send debug info over */
	if (me->logging_on) {
		pthread_mutex_lock(&me->logging_mutex);
		if (!me->log_lines.empty()) {
			char buf[SMCINVOKE_LOG_BUF_SIZE];
			memset(buf, 0, SMCINVOKE_LOG_BUF_SIZE);
			std::string line = me->log_lines.front();
			me->log_lines.pop();
			snprintf(buf, SMCINVOKE_LOG_BUF_SIZE,
					"HLOS fd=%d op=0x%x counts=0x%x %s",
					me->fd, op, counts, line.c_str());
			ret = ioctl(me->fd, SMCINVOKE_IOCTL_LOG, buf);
			if (ret) {
				ALOGE("%s: SMCINVOKE_IOCTL_LOG %s, ret=%d", __func__, buf, ret);
				me->logging_on = 0;
			}
		}
		pthread_mutex_unlock(&me->logging_mutex);
	}
#endif
	ALOGV("invoke_over_ioctl, obj.context = %p, fd = %d, op = %d, counts = 0x%x", me, me->fd, op, counts);

	ret = ioctl(me->fd, SMCINVOKE_IOCTL_INVOKE_REQ, &req);

	if (ret) {
		ALOGE("Error::ioctl call to INVOKE failed with ret = %d, result = %d, errno = %d",
			ret, req.result, errno);
		/* If ioctl return error, valid OO fd should be closed */
		FOR_ARGS(i, counts, OO) {
			struct smcinvoke_obj obj = argptr[i].o;
			if (obj.fd >= 0) {
				ALOGE("Close OO[%zu].fd = %d\n", i, (int)obj.fd);
				close(obj.fd);
			}
		}
                ret = remap_error(errno);
		goto exit;
	}

	if (!req.result)
		ret = marshalOut(&req, argptr, args, counts, me);
exit:

	free(argptr);
	return ret | req.result;
}

Object MinkDescriptor_new(int fd, shared_ptr<MinkCBContext> cbcxt)
{
	MinkDescriptor *me = ZALLOC_REC(MinkDescriptor);
	if (!me) {
		return Object_NULL;
	}

	me->refs = 1;
	me->fd = fd;
	me->cbctx = cbcxt;
#ifdef SMCINVOKE_IOCTL_LOG	/* provide log info if kernel supports the smcinvoke logging feature */
	me->logging_on = 1;
	pthread_mutex_init(&me->logging_mutex, 0);
#endif
	ALOGV("New object: context = %p, fd = %d", me, fd);
	return (Object) { invoke_over_ioctl, me };
}

#ifdef __cplusplus
extern "C" {
#endif

int MinkDescriptor_getRootEnv (size_t cbthread_cnt, size_t cbbuf_len, Object *obj)
{
	/* get rootObj fd */
	int root_fd = -1;
	root_fd = open("/dev/smcinvoke", O_RDWR);
	if (root_fd < 0) {
		ALOGE("Failed to open invoke driver, errno = %d", errno);
		return Object_ERROR;
	}

	/*serer request*/
	shared_ptr<MinkCBContext> cbctx(nullptr);
#ifdef SMCINVOKE_IOCTL_SERVER_REQ
	int server_fd = -1;
	struct smcinvoke_server server_req = {0};

	server_req.cb_buf_size = (uint32_t)cbbuf_len;

	server_fd = ioctl(root_fd, SMCINVOKE_IOCTL_SERVER_REQ, &server_req);
	if (server_fd < 0) {
		ALOGE("ioctl SERVER_REQ with buf_len %zu failed ret = %d, errno = %d\n",
			cbbuf_len, server_fd, errno);
		return Object_ERROR;
	}
	/* Start Callback service */
	try {
		cbctx = make_shared<MinkCBContext>(
			cbthread_cnt, cbbuf_len, server_fd);
	}
#else
	try {
		cbctx = make_shared<MinkCBContext>();
	}
#endif
	catch(const exception & e) {
		ALOGE("Caught exception, %s", e.what());
		ALOGE("Failed to construct MinkCBContext");
	}
	if (cbctx == nullptr) {
		ALOGE("Failed to create MinkCBContext");
		return Object_ERROR;
	}

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
	cbctx->WaitTillAcceptThreadsAreCreated();
#endif
	/* create rootObj */
	*obj = MinkDescriptor_new(root_fd, cbctx);
	if (Object_isNull(*obj)){
			ALOGE("TZObject_new failed, errno = %d", errno);
			close(root_fd);
			cbctx = NULL;
			return Object_ERROR;
	}

	ALOGV("%s: root_fd = %d, obj.invoke=%p, obj.context=%p", __func__, root_fd, (void*) obj->invoke, (void*) obj->context);
	ALOGV("cbthread_cnt = %zu, cbbuf_len = %zu", cbthread_cnt, cbbuf_len);

	return Object_OK;
}

#ifdef SMCINVOKE_IOCTL_SERVER_REQ
int32_t MinkDescriptor_shutdownCBService(Object obj)
{
	if (obj.invoke == invoke_over_ioctl) {
		MinkDescriptor	*ctx = (MinkDescriptor *)(obj.context);
		if (ctx == NULL) {
			ALOGE("Failed to get MinkDescriptor obj context");
			return Object_ERROR_BADOBJ;
		}
		ctx->cbctx->shutdown();
		ctx->cbctx.reset();
		return Object_OK;
	} else
		return Object_ERROR;
}
#endif

#ifdef __cplusplus
}
#endif
