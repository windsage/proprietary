////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  synx.h
/// @brief The Synx API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SYNX_H
#define SYNX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    //////////////////////////////////////////////////////////////////////////
    // Entry Point
    //////////////////////////////////////////////////////////////////////////
    #if defined (_WIN32)
    #define SYNX_DLL_EXPORT            __declspec(dllexport)
    #define SYNX_DLL_IMPORT            __declspec(dllimport)
    #elif defined(__GNUC__)
    #define SYNX_DLL_EXPORT            __attribute((visibility("default")))
    #define SYNX_DLL_IMPORT
    #endif

    #if defined(SYNX_LIB_EXPORT)
    #define SYNX_API                   SYNX_DLL_EXPORT
    #define SYNX_INLINE                inline
    #else
    #define SYNX_INLINE
    #define SYNX_API                   SYNX_DLL_IMPORT
    #endif


#define SYNX_DEVICE_NAME "synx_device"



/**
 * Error codes returned from framework
 *
 * Return codes are mapped to platform specific
 * return values.
 */
#define SYNX_SUCCESS      0
#define SYNX_NOMEM        ENOMEM
#define SYNX_NOSUPPORT    EPERM
#define SYNX_NOPERM       EPERM
#define SYNX_TIMEOUT      ETIMEDOUT
#define SYNX_ALREADY      EALREADY
#define SYNX_NOENT        ENOENT
#define SYNX_INVALID      EINVAL
#define SYNX_ERROR_FAILED EPERM

#define SYNX_NO_TIMEOUT        ((uint64_t)-1)

/**
 * SYNX_INVALID_HANDLE      : client can assign the synx handle variable with this value
 *                            when it doesn't hold a valid synx handle
 */
#define SYNX_INVALID_HANDLE 0

/**
 * struct synx_session - Client session identifier.
 *
 * @type   : Session type.
 *           Internal Member (Do not access/modify).
 * @client : Pointer to client session.
 *           Internal Member (Do not access/modify).
 */
struct synx_session {
	uint32_t type;
	void     *client;
};

/**
 * enum synx_client_id : Unique identifier of the supported clients.
 *
 * @SYNX_CLIENT_NATIVE   : Native Client to be passed during
 *                         session initialization.
 */
enum synx_client_id {
	SYNX_CLIENT_NATIVE = 0,
	SYNX_CLIENT_MAX,
};

/**
 * enum synx_init_flags - Session initialization flag.
 * @SYNX_INIT_DEFAULT    : Intialization flag to be passed
 *                         while initializing session.
 * @SYNX_INIT_MAX        : Used for internal checks.
 */
enum synx_init_flags {
	SYNX_INIT_DEFAULT = 0x00,
	SYNX_INIT_MAX     = 0x01,
};

/**
 * struct synx_initialization_params - Session params.
 *
 * @name  : Client session name
 *          Only first 64 bytes are accepted, rest will be ignored.
 * @id    : Client identifier.
 * @flags : Synx initialization flags.
 */
struct synx_initialization_params {
	const char             *name;
	enum synx_client_id    id;
	enum synx_init_flags   flags;
};

/**
 * enum synx_create_flags - Flags passed during synx_create call.
 *
 * SYNX_CREATE_LOCAL_FENCE  : Instructs the framework to create local synx object,
 *                            for local synchronization i.e  within core.
 * SYNX_CREATE_GLOBAL_FENCE : Instructs the framework to create global synx object,
 *                            for global synchronization i.e across cores.
 * SYNX_CREATE_DMA_FENCE    : Create a synx object by wrapping the provided dma fence.
 *                            Need to pass the fd backed by the SyncFile through fence
 *                            variable if this flag is set. (NOT SUPPORTED)
 * SYNX_CREATE_CSL_FENCE    : Create a synx object with provided csl fence.
 *                            Establishes interop with the csl fence through
 *                            bind operations. (NOT SUPPORTED)
 */
enum synx_create_flags {
	SYNX_CREATE_LOCAL_FENCE  = 0x01,
	SYNX_CREATE_GLOBAL_FENCE = 0x02,
	SYNX_CREATE_DMA_FENCE    = 0x04,
	SYNX_CREATE_CSL_FENCE    = 0x08,
};

/**
 * struct synx_create_params - Synx creation parameters.
 *
 * @name     : Optional parameter associating a name with the synx
 *             object for debug purposes.
 *             Only first 64 bytes are accepted,
 *             rest will be ignored.
 * @h_synx   : Pointer to synx object handle. (filled by function)
 * @fence    : Pointer to external dma fence or csl fence. (NOT SUPPORTED)
 * @flags    : Synx create flags for customization.
 */
struct synx_create_params {
	const char             *name;
	uint32_t               *h_synx;
	void                   *fence;
	enum synx_create_flags flags;
};

/**
 * synx_user_callback - Callback function registered by clients.
 *
 * User callback registered for non-blocking wait. Dispatched with
 * status of synx object if it is signaled or SYNX_STATUS_TIMEOUT
 * in case of timeout.
 */
typedef void (*synx_user_callback)(uint32_t h_synx, int status, void *data);

/**
 * struct synx_callback_params - Synx callback parameters.
 *
 * @h_synx         : Synx object handle.
 * @cb_func        : Pointer to callback func to be invoked.
 * @userdata       : Opaque pointer passed back with callback as data.
 * @cancel_cb_func : Pointer to callback to ack cancellation.
 * @timeout_ms     : Timeout in ms. SYNX_NO_TIMEOUT if timeout is not
 *                   required.
 */
struct synx_callback_params {
	uint32_t           h_synx;
	synx_user_callback cb_func;
	void               *userdata;
	synx_user_callback cancel_cb_func;
	uint64_t           timeout_ms;
};

/**
 * enum synx_signal_status - Signal status.
 *
 * SYNX_STATE_SIGNALED_SUCCESS : Signal success.
 * SYNX_STATE_SIGNALED_CANCEL  : Signal cancellation.
 * SYNX_STATE_SIGNALED_MAX     : Clients can send custom notification beyond
 *                               the max value (only positive). Clients should not
 *                               pass SYNX_STATE_SIGNALED_MAX, only values greater
 *                               than SYNX_STATE_SIGNALED_MAX are to be used for
 *                               sending custom notification.
 */
enum synx_signal_status {
	SYNX_STATE_SIGNALED_SUCCESS = 2,
	SYNX_STATE_SIGNALED_CANCEL  = 4,
	SYNX_STATE_SIGNALED_MAX     = 64,
};


/**
 * enum synx_import_flags - Import flags
 * SYNX_IMPORT_LOCAL_FENCE  : Instructs the framework to create local synx object
 *                            for local synchronization i.e. within same core.
 * SYNX_IMPORT_GLOBAL_FENCE : Instructs the framework to create global synx object
 *                            for global synchronization i.e. across supported cores.
 * SYNX_IMPORT_SYNX_FENCE   : Import native Synx handle for synchronization.
 *                            Need to pass the Synx handle ptr through fence variable
 *                            if this flag is set. Client must pass:
 *                            a. SYNX_IMPORT_SYNX_FENCE|SYNX_IMPORT_LOCAL_FENCE
 *                               to import a synx handle as local synx handle.
 *                            b. SYNX_IMPORT_SYNX_FENCE|SYNX_IMPORT_GLOBAL_FENCE
 *                               to import a synx handle as global synx handle.
 * SYNX_IMPORT_DMA_FENCE    : Import dma fence and create Synx handle for interop
 *                            Need to pass the dma_fence fd ptr through fence variable.
 *                            if this flag is set. Client must pass:
 *                            a. SYNX_IMPORT_DMA_FENCE|SYNX_IMPORT_LOCAL_FENCE
 *                               to import a dma fence and create local synx handle
 *                               for interop.
 *                            b. SYNX_IMPORT_GLOBAL_FENCE|SYNX_IMPORT_DMA_FENCE
 *                               to import a dma fence and create global synx handle
 *                               for interop.
 * SYNX_IMPORT_EX_RELEASE   : Flag to inform relaxed invocation where release call
 *                            need not be called by client on this handle after import.
 *                            (NOT SUPPORTED)
 */
enum synx_import_flags {
	SYNX_IMPORT_LOCAL_FENCE  = 0x01,
	SYNX_IMPORT_GLOBAL_FENCE = 0x02,
	SYNX_IMPORT_SYNX_FENCE   = 0x04,
	SYNX_IMPORT_DMA_FENCE    = 0x08,
	SYNX_IMPORT_EX_RELEASE   = 0x10,
};

/**
 * SYNX_IMPORT_INDV_PARAMS : Import filled with synx_import_indv_params struct.
 * SYNX_IMPORT_ARR_PARAMS  : Import filled with synx_import_arr_params struct.
 */
enum synx_import_type {
	SYNX_IMPORT_INDV_PARAMS = 0x01,
	SYNX_IMPORT_ARR_PARAMS  = 0x02,
};

/**
 * struct synx_import_indv_params - Synx import indv parameters
 *
 * @new_h_synxs : Pointer to new synx object.
 *                (filled by the function)
 * @flags       : Synx import flags.
 * @fence       : Pointer to DMA fence fd or synx handle.
 */
struct synx_import_indv_params {
	uint32_t               *new_h_synx;
	enum synx_import_flags flags;
	void                   *fence;
};

/**
 * struct synx_import_arr_params - Synx import arr parameters
 *
 * @list        : List of synx import indv parameters.
 * @num_fences  : Number of fences or synx handles to be imported.
 */
struct synx_import_arr_params {
	struct synx_import_indv_params *list;
	uint32_t    			num_fences;
};

/**
 * struct synx_import_params - Synx import parameters
 *
 * @type : Import params type filled by client.
 * @indv : Params to import an individual handle or fence.
 * @arr  : Params to import an array of handles or fences.
 */
struct synx_import_params {
	enum synx_import_type type;
	union {
		struct synx_import_indv_params indv;
		struct synx_import_arr_params  arr;
	};
};

/**
 * SYNX_MERGE_LOCAL_FENCE   : Create local composite synx object. To be passed along
 *                            with SYNX_MERGE_NOTIFY_ON_ALL.
 * SYNX_MERGE_GLOBAL_FENCE  : Create global composite synx object. To be passed along
 *                            with SYNX_MERGE_NOTIFY_ON_ALL.
 * SYNX_MERGE_NOTIFY_ON_ALL : Notify on signaling of ALL synx objects.
 *                            Client must pass:
 *                            a. SYNX_MERGE_LOCAL_FENCE|SYNX_MERGE_NOTIFY_ON_ALL
 *                               to create local composite synx object and notify
 *                               it when all child synx objects are signaled.
 *                            b. SYNX_MERGE_GLOBAL_FENCE|SYNX_MERGE_NOTIFY_ON_ALL
 *                               to create global composite synx object and notify
 *                               it when all child synx objects are signaled.
 * SYNX_MERGE_NOTIFY_ON_ANY : Notify on signaling of ANY object. (NOT SUPPORTED)
 */
enum synx_merge_flags {
	SYNX_MERGE_LOCAL_FENCE   = 0x01,
	SYNX_MERGE_GLOBAL_FENCE  = 0x02,
	SYNX_MERGE_NOTIFY_ON_ALL = 0x04,
	SYNX_MERGE_NOTIFY_ON_ANY = 0x08,
};

/*
 * struct synx_merge_params - Synx merge parameters
 *
 * @h_synxs      : Pointer to a array of synx handles to be merged.
 * @flags        : Merge flags.
 * @num_objs     : Number of synx handles to be merged.
 * @h_merged_obj : Merged synx handle. (filled by function)
 */
struct synx_merge_params {
	uint32_t              *h_synxs;
	enum synx_merge_flags flags;
	uint32_t              num_objs;
	uint32_t              *h_merged_obj;
};

/*
 * enum synx_status - Synx status definitions
 *
 * @SYNX_STATUS_INVALID             : Invalid synx object.
 * @SYNX_STATUS_UNSIGNALED          : Synx object has not been signalled.
 * @SYNX_STATUS_SIGNALED_SUCCESS    : Synx object has been signalled.
 * @SYNX_STATUS_SIGNALED_ERROR      : Synx object signalled with error.
 * @SYNX_STATUS_SIGNALED_CANCEL     : Synx object signalling cancelled.
 * @SYNX_STATUS_SIGNALED_EXTERNAL   : Synx object signaled by external dma client.
 * @SYNX_STATUS_SIGNALED_SSR        : Synx object signaled with SSR.
 * @SYNX_STATUS_TIMEOUT             : Callback status for synx object
 *                                    in case of timeout.
 */
enum synx_status {
    SYNX_STATUS_INVALID = 0,
    SYNX_STATUS_UNSIGNALED,
    SYNX_STATUS_SIGNALED_SUCCESS,
    SYNX_STATUS_SIGNALED_ERROR,
    SYNX_STATUS_SIGNALED_CANCEL,
    SYNX_STATUS_SIGNALED_EXTERNAL,
    SYNX_STATUS_SIGNALED_SSR,
    SYNX_STATUS_TIMEOUT,
};


/* Userspace APIs */

/**
 * @brief: Initializes a new client session.
 *
 * @param params : Pointer to session init params
 *
 * @return Client session pointer on success. NULL in case of failure.
 */
SYNX_API struct synx_session *synx_initialize(struct synx_initialization_params *pParam);

/**
 * @brief: Destroys the client session. Every initialized session should be
 *         uninitialized.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_uninitialize(struct synx_session *pSession);

/**
 * @brief: Creates a synx object.
 *
 * Creates a new synx obj and returns the handle to client. There can be
 * maximum of 4095 global synx handles or local synx handles across
 * sessions.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param pParam  : Pointer to create params.
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_create(struct synx_session *pSession, struct synx_create_params *pParam);

/**
 * @brief: Returns the status of the synx object.
 *         This API should not be used in polling mode to check if
 *         the handle is signaled or not, client needs to
 *         explicitly wait using synx_wait() or synx_async_wait()
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param hSynx    : Synx object handle.
 *
 * @return Status of the synx object(same as synx_status enum).
 */
SYNX_API int32_t synx_get_status(struct synx_session *pSession, uint32_t hSynx);

/**
 * @brief: Releases the synx object.
 *         Every created, imported or merged synx object should be
 *         released.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param hSynx    : Synx object handle to be destroyed
 *
 * @return Status of operation. SYNX_ERROR_FAILED in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_release(struct synx_session *pSession, uint32_t hSynx);

/**
 * @brief: Signals a synx object with the status argument.
 *
 * This function will signal the synx object referenced by h_synx
 * and invoke any external binding synx objs.
 * The status parameter will indicate whether the entity
 * performing the signaling wants to convey an error case or a success case.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param hSynx    : Synx object handle
 * @param status   : Status of signaling.
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_signal(struct synx_session *pSession, uint32_t hSynx, enum synx_signal_status state);

/**
 * @brief: Waits for a synx object synchronously
 *
 * Does a wait on the synx object identified by h_synx for a maximum
 * of timeout_ms milliseconds. Must not be called from interrupt context as
 * this API can sleep.
 * Will return status(enum synx_signal_status) if handle was signaled.
 *
 * @param pSession    : Session ptr (returned from synx_initialize)
 * @param hSynx       : Synx object handle to be waited upon
 * @param timeout_ms  : Timeout in ms
 *
 * @return Signal status. -SYNX_INVAL if synx object is in bad state or
 *         arguments are invalid, -SYNX_TIMEOUT if wait times out.
 *         SYNX_ERROR_FAILED in case of error.
 */
SYNX_API int32_t synx_wait(struct synx_session *pSession, uint32_t hSynx, uint64_t timeout_ms);

/**
 * @brief: Registers a callback with a synx object.
 *
 * Clients can register maximum of 64 callback functions per
 * synx session.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param pParams  : Callback params.
 *                   cancel_cb_func in callback params is optional.
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_async_wait(struct synx_session *pSession, struct synx_callback_params *pParams);

/**
 * @brief: De-registers a callback with a synx object
 *
 * This API will only cancel one instance of callback function(mapped
 * with userdata and h_synx) provided in cb_func of callback params.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param pParams  : Callback params.
 *
 * @return Status of operation.-SYNX_ALREADY if object has already
 *         been signaled or timedout, and cannot be cancelled. Negative in case
 *         of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_cancel_async_wait(struct synx_session *pSession, struct synx_callback_params *pParams);

/**
 * @brief: Merges multiple synx objects.
 *
 * This function will merge multiple synx objects into a synx group.
 * Subsequently composite merged synx objects can also be merged.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param pParams  : Merge params
 *
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_merge( struct synx_session *pSession, struct synx_merge_params *pParams);

/**
 * @brief: Imports (looks up) synx object from given handle or fence.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param pParams  : Pointer to import params
 *
 * @return Status of operation. Negative in case of error. SYNX_SUCCESS otherwise.
 */
SYNX_API int32_t synx_import(struct synx_session *pSession, struct synx_import_params *pParams);

/**
 * @brief: Obtain an fd for the synx object
 *
 * The platform native fence is wrapped and
 * returned through fence fd, which could be shared
 * with other processes through binder interface. It is client's
 * reponsibility to destroy fd once used.
 *
 * @param pSession : Session ptr (returned from synx_initialize)
 * @param hSynx    : Synx object handle
 *
 * @return File Decriptor on success. Negative in case of error.
 */
SYNX_API int32_t synx_get_fd(struct synx_session *pSession, uint32_t hSynx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SYNX_H
