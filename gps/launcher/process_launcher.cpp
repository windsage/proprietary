
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Copyright (c) 2014-2016, 2018, 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#define LOG_TAG "LocSvc_launcher"

#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <loc_cfg.h>
#include <sys/prctl.h>
#include <loc_misc_utils.h>
#include <sys/capability.h>
#include <cutils/properties.h>
#include <log_util.h>
#include <loc_pla.h>
#include <gps_extended_c.h>
#include <mq_client/mq_client.h>
#include <base_util/postcard.h>
#include <DataItemId.h>
#include <map>
#include <mutex>

#define MAX_CHILD_PROC_RESTARTS 5
#define ONE_DAY_SECONDS 86400
#define updateBitInMask(mask, bit, status) \
     (mask) = (status) ? ((mask) | (bit)) : ((mask) & ~(bit))

// Notify main thread from signal handling thread
pthread_mutex_t signal_handling_done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  signal_handling_done_cond  = PTHREAD_COND_INITIALIZER;

// Handling waitpid for signals from child process
pthread_mutex_t  signal_handler_mutex = PTHREAD_MUTEX_INITIALIZER;

static loc_process_info_s_type *child_proc = nullptr;
static unsigned int proc_list_length = 0;
static unsigned char* num_restarts_ptr = nullptr;
static bool thread_loop_continue = true;
static LocLaunchTriggerMask locProcessEventBringUpMask = 0;
static const char MQ_CLIENT_LOC_LAUNCHER[] = "LOC_LAUNCHER";
static std::map<std::string, LocLaunchTriggerMask> locProcessEventBringUpMap;
static std::mutex locProcessEventBringUpMaskMutex;
static bool locProcLauncherMaskConsumed;
static void* signal_handler_thread_func(void* arg);
static void process_launcher_start_ipc_listener();
static void* mq_client_thread_fn(void* arg);
static bool enableProcessOnTrigger(int index, std::map<std::string,
        LocLaunchTriggerMask>& locProcessEventBringUpMap, LocLaunchTriggerMask in);
static std::string getProcessKey(int index);

int main()
{
    volatile unsigned int i = 0;
    unsigned int j = 0;
    gid_t gid_list[LOC_PROCESS_MAX_NUM_GROUPS];
    sigset_t signal_set;
    int ngroups = 0, ret = 0;

    pthread_t signal_handler_thread;
    int result = 0;

    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);

    if (0 == getuid()) {
#if defined(INIT_SYSTEM_SYSTEMD) || defined(OPENWRT_BUILD)
        // started as root.
        LOC_LOGE("Error !!! loc_launcher started as root");
        goto err_capset;
#else
        struct __user_cap_header_struct cap_hdr = {};
        struct __user_cap_data_struct cap_data = {};

        cap_hdr.version = _LINUX_CAPABILITY_VERSION;
        cap_hdr.pid = 0;

        if(prctl(PR_SET_KEEPCAPS, 1) < 0) {
            LOC_LOGE("%s:%d]: Error: prctl failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }

        // Set the group id first and then set the effective userid, to gps.
        if (-1 == setgid(GID_GPS)) {
            LOC_LOGE("%s:%d]: Error: setgid failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }
        // Set the loc_launcher uid to 'gps'.
        if (-1 == setuid(UID_GPS)) {
            LOC_LOGE("%s:%d]: Error: setuid failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }

        cap_data.permitted = (1<<CAP_SETUID) | (1<<CAP_SETGID) |
                (1<<CAP_NET_BIND_SERVICE) | (1<<CAP_NET_ADMIN);
        cap_data.effective = cap_data.permitted;

        LOC_LOGD("cap_data.permitted: %d", (int)cap_data.permitted);

        if(capset(&cap_hdr, &cap_data)) {
            LOC_LOGE("%s:%d]: Error: capset failed. %s\n",
                     __func__, __LINE__, strerror(errno));
            ret = -1;
            goto err_capset;
        }
#endif
    }

    LOC_LOGD("Parent- pid: %d, uid: %d, euid: %d, gid: %d, egid: %d\n",
             getpid(), getuid(), geteuid(), getgid(), getegid());

    //Read current group subscriptions
    memset(gid_list, 0, sizeof(gid_list));
    ngroups = getgroups(LOC_PROCESS_MAX_NUM_GROUPS, gid_list);
    if(ngroups == -1) {
        LOC_LOGE("Could not find groups. ngroups:%d\n", ngroups);
    }
    else {
        IF_LOC_LOGD {
            LOC_LOGD("GIDs from parent: ");
            for(ngroups = 0; ngroups<LOC_PROCESS_MAX_NUM_GROUPS; ngroups++) {
                if(gid_list[ngroups]) {
                    LOC_LOGD("%d ", gid_list[ngroups]);
                }
            }
            LOC_LOGD("\n");
        }
    }

    // parse the process conf file: izat.conf
    ret = loc_read_process_conf(LOC_PATH_IZAT_CONF, &proc_list_length, &child_proc);
    if (ret != 0) {
        goto err_conf_file;
    }
    // allocate memory to keep track for number of restarts
    num_restarts_ptr = (unsigned char*) malloc (proc_list_length * sizeof (unsigned char));
    if (num_restarts_ptr == NULL) {
        free (child_proc);
        child_proc = NULL;
        goto err_conf_file;
    }
    memset (num_restarts_ptr, 0, proc_list_length * sizeof (unsigned char));

    for (int i = 0; i < proc_list_length; i++) {
        if (ENABLED == child_proc[i].proc_status && (child_proc[i].launch_trigger_mask > 0)) {
            std::string key(getProcessKey(i));
            LOC_LOGv("Adding process: %s with Key: %s in Event based bring up with mask: 0x%x",
                    child_proc[i].name[0], key.c_str(), child_proc[i].launch_trigger_mask);
            // Add all the process subscribed for trigger based start.
            locProcessEventBringUpMap.insert(std::pair<std::string, LocLaunchTriggerMask>(key,
                    (LocLaunchTriggerMask)child_proc[i].launch_trigger_mask));
            // Process will be enabled later if respective process trigger happens.
            child_proc[i].proc_status = DISABLED;
        }
    }

    // Block signal SIGCHLD and SIGALRM in main process thread
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGCHLD);
    sigaddset(&signal_set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    // Create a thread to handle signals and set a name for it.
    result = pthread_create(&signal_handler_thread, NULL,
            signal_handler_thread_func, (void *) &signal_set);
    if (result != 0) {
        LOC_LOGE("pthread_create failed. Error = %d\n", result);
        goto err_signal_thread_fail;
    }
    result = pthread_setname_np(signal_handler_thread, (char *) "loc_sig_hndlr");
    if (result != 0) {
        LOC_LOGE("pthread_setname_np failed. Error = %d\n", result);
        goto err;
    }

    // Start listening for IPC Messages from other location processes
    process_launcher_start_ipc_listener();

    //Set an alarm to be woken up after 24 hours to clear
    //the number of restarts for active child processes
    alarm(ONE_DAY_SECONDS);

    //Start recurring loop. This loop does the following things:
    //1. Launch enabled child processes
    while (1) {
        locProcessEventBringUpMaskMutex.lock();
        LocLaunchTriggerMask bringupMask = locProcessEventBringUpMask;
        locProcessEventBringUpMaskMutex.unlock();

        pthread_mutex_lock(&signal_handling_done_mutex);
        locProcLauncherMaskConsumed = true;
        pthread_mutex_unlock(&signal_handling_done_mutex);
        //Start making children
        for (i=0; i<proc_list_length;i++) {

            if (enableProcessOnTrigger(i, locProcessEventBringUpMap, bringupMask)) {
                child_proc[i].proc_status = ENABLED;
            }

            if (child_proc[i].proc_status == ENABLED) {
                LOC_LOGd("Forking child: %d, name: %s, launch event mask 0x%x",
                    i, child_proc[i].name[0], child_proc[i].launch_trigger_mask);
                if((child_proc[i].proc_id = fork()) < 0) {
                    LOC_LOGE("%s: fork of child failed:%s\n",
                          child_proc[i].name[0], strerror(errno));
                    exit(1);
                }
                //This block executed by child process
                else if(!child_proc[i].proc_id) {
                    LOC_LOGD("Child born- pid: %d, uid: %d, euid: %d, gid: %d, egid: %d\n",
                             getpid(),getuid(), geteuid(), getgid(), getegid());
                    ngroups = setgroups(child_proc[i].num_groups, child_proc[i].group_list);
                    if(ngroups == -1) {
                        LOC_LOGE("Could not set groups. errno:%d, %s\n",
                              errno, strerror(errno));
                    }
                    LOC_LOGD("%s:%d]: Groups set\n", __func__, __LINE__);

                    memset(gid_list, 0, sizeof(gid_list));
                    ngroups = getgroups(LOC_PROCESS_MAX_NUM_GROUPS, gid_list);
                    if(ngroups == -1) {
                        LOC_LOGE("Could not find groups. ngroups:%d\n", ngroups);
                    }
                    else {
                        IF_LOC_LOGD {
                            LOC_LOGD("GIDs from child:\n");
                            for(ngroups = 0; ngroups<LOC_PROCESS_MAX_NUM_GROUPS; ngroups++) {
                                if(gid_list[ngroups]) {
                                    LOC_LOGD("%d ", gid_list[ngroups]);
                                }
                            }
                            LOC_LOGD("\n");
                        }
                    }
                    ngroups=0;
                    //Set up the child process to receive exit when it receives
                    //a SIGHUP.
                    //SIGHUP is sent to the child process when the parent process
                    //exits. We want all child processes to exit when the launcher
                    //exits so that restarting the launcher will re-launch all the
                    //child processes.
                    if(prctl(PR_SET_PDEATHSIG, SIGHUP, 0, 0, 0) == -1) {
                        LOC_LOGE("%s:%d]: prctl failed. %s", __func__, __LINE__, strerror(errno));
                    }

                    char *args_temp[LOC_PROCESS_MAX_NUM_ARGS];
                    memset (args_temp, 0, sizeof (args_temp));
                    uint32_t args_count;
                    for (args_count = 0; args_count < LOC_PROCESS_MAX_NUM_ARGS; args_count++) {
                        if (child_proc[i].args[args_count][0] == '\0') {
                          break;
                        } else {
                          args_temp[args_count] = child_proc[i].args[args_count];
                        }
                    }
                    execvp(child_proc[i].name[0], args_temp);
                    int exec_error = errno;
                    LOC_LOGE("\nError! execvp() returned. Something is wrong. Could not start %s,"
                             "Error: %d [%s]\n",
                             child_proc[i].name[0], exec_error, strerror(exec_error));
                    exit(1);

                }
                //This block executed by parent process
                else {
                    LOC_LOGD("Child %s created with pid: %d\n",
                             child_proc[i].name[0], child_proc[i].proc_id);
                    child_proc[i].proc_status = RUNNING;
                }
            }
        }
        pthread_mutex_lock(&signal_handling_done_mutex);
        if (locProcLauncherMaskConsumed) {
            LOC_LOGD("Parent sleeping. Will wake up when one of the children mess up\n");
            // wait for cond signal from signal handler thread
            pthread_cond_wait(&signal_handling_done_cond, &signal_handling_done_mutex);

        }
        pthread_mutex_unlock(&signal_handling_done_mutex);
        LOC_LOGE("Parent woken up!\n");
    }
err:
    if(child_proc) {
        for(j=0; j<proc_list_length; j++)
        {
            if(child_proc[j].proc_id)
                kill(child_proc[j].proc_id, SIGHUP);
            if(child_proc[j].name[0]) {
                free(child_proc[j].name[0]);
            }
        }
        free(child_proc);
    }
    if (num_restarts_ptr) {
        free (num_restarts_ptr);
    }

err_signal_thread_fail:
    // close signal handling thread.
    thread_loop_continue = false;
    pthread_join(signal_handler_thread, NULL);

err_conf_file:
err_capset:
    LOC_LOGE("%s:%d]: Exiting main. ret: %d", __func__, __LINE__, ret);
    return ret;
}

// Create uniq process key name with process name and first argument
// processes like EP can have same name as engine-service. To avoid
// such name conflit use first argument as well.
std::string getProcessKey(int index) {
    std::string key(child_proc[index].name[0]);
    if ('\0' != child_proc[index].args[1][0]) {
        key += std::string(child_proc[index].args[1]);
    }
    return key;
}

bool enableProcessOnTrigger(int index, std::map<std::string,
        LocLaunchTriggerMask>& locProcessEventBringUpMap, LocLaunchTriggerMask bringupMask) {
    bool enableStatus = false;
    std::string key = getProcessKey(index);
    auto itr = locProcessEventBringUpMap.find(key);
    if (itr != locProcessEventBringUpMap.end()) {
        // DISABLED here signfies this process was ENABLED in izat config
        // OR this process exited with status 0 due to shutdown trigger

        if (DISABLED == child_proc[index].proc_status &&
                MAX_CHILD_PROC_RESTARTS > num_restarts_ptr[index]) {
            LOC_LOGd("Enabled Process: %s with trigger: 0x%x, key %s", child_proc[index].name[0],
                    child_proc[index].launch_trigger_mask, key.c_str());
            // Enable a process if its trigger matches
            // OR enable a process if LOC_ON_EMERGENCY is set in launch_trigger_mask
            // ignore other masks if device is in emergency
            LocLaunchTriggerMask maskwithoutEmergencyBit = (itr->second & (~LOC_ON_EMERGENCY));
            if ((maskwithoutEmergencyBit == (maskwithoutEmergencyBit & bringupMask))
                    || ((itr->second & bringupMask & LOC_ON_EMERGENCY) > 0)) {
                enableStatus = true;
                LOC_LOGd("Enabled Process: %s with trigger: 0x%x", child_proc[index].name[0],
                        child_proc[index].launch_trigger_mask);
            } else if (key.find("xtwifi-client") != std::string::npos) {
                //For GTP WiFi process, launch trigger condition is
                //Location enabled && (OptIn enabled || NLP session started)
                LOC_LOGd("Enabled Process: %s with trigger: 0x%x", child_proc[index].name[0],
                        child_proc[index].launch_trigger_mask);
                LocLaunchTriggerMask maskwithoutOptInBit =
                        (maskwithoutEmergencyBit & (~LOC_ON_OPT_IN));
                LocLaunchTriggerMask maskwithoutNlpSessionBit =
                        (maskwithoutEmergencyBit & (~LOC_ON_NLP_SESSION_START));
                if (((maskwithoutOptInBit & locProcessEventBringUpMask) == maskwithoutOptInBit) ||
                        ((maskwithoutNlpSessionBit & locProcessEventBringUpMask)
                    == maskwithoutNlpSessionBit)) {
                    enableStatus = true;
                    LOC_LOGd("Enabled Process: %s with trigger: 0x%x", child_proc[index].name[0],
                            child_proc[index].launch_trigger_mask);
                }
            }
        }
    }
    return enableStatus;
}

static void* signal_handler_thread_func(void* arg)
{
    int result = 1;
    sigset_t *signal_set = (sigset_t *)arg;
    int signal_num = 0;
    unsigned int i = 0;
    //To be passed in to waitpid
    int child_status;
    int child_exit_status = 0;

    //Recurring thread loop. This loop does the following things:
    //1. Check if SIGCHLD is received and re-enable/disable child processes
    //2. Check if SIGALRM is received and clear restart counter
    while (thread_loop_continue) {
        result = sigwait(signal_set, &signal_num);
        if (0 != result) {
            LOC_LOGE("%s:%d]: sigwait error : %d\n", __func__, __LINE__, result);
        }
        LOC_LOGD("%s:%d]: Got signal %s(%d)\n",  __func__, __LINE__,
                strsignal(signal_num), signal_num);

        //Check if any of the child processes have exited
        if (SIGCHLD == signal_num) {
            LOC_LOGE("%s:%d]: Received SIGCHLD\n", __func__, __LINE__);
            //Loop through the list of child processes and figure out which
            //process has exited
            for (i = 0; i < proc_list_length; i++) {
                child_status = 0;
                child_exit_status = 0;
                if (child_proc[i].proc_status == RUNNING) {
                    LOC_LOGD("%s:%d]: Child_id: %d; child name: %s\n",
                             __func__, __LINE__, child_proc[i].proc_id, child_proc[i].name[0]);
                    pthread_mutex_lock(&signal_handler_mutex);
                    if (waitpid(child_proc[i].proc_id, &child_status, WNOHANG) ==
                       child_proc[i].proc_id) {
                        if (WIFEXITED(child_status)) {
                            child_exit_status = WEXITSTATUS(child_status);
                            if (LOC_DYNAMIC_PROC_DISABLE_CODE == child_exit_status) {
                                LOC_LOGi("Received dynamic process disable exit status: %d for "
                                    "process: %s", child_exit_status, child_proc[i].name[0]);
                                child_proc[i].proc_status = DISABLED;
                                auto itr = locProcessEventBringUpMap.find(getProcessKey(i));
                                if (itr != locProcessEventBringUpMap.end()) {
                                    locProcessEventBringUpMap.erase(itr);
                                }
                            }
                            else if (child_exit_status) {
                                LOC_LOGE("%s:%d]: Exit status: %d. Restarting\n",
                                         __func__, __LINE__, child_exit_status);
                                if (num_restarts_ptr[i] == MAX_CHILD_PROC_RESTARTS) {
                                    child_proc[i].proc_status = DISABLED;
                                    LOC_LOGE("%s:%d]: %s Restarted too many times. Grounded!\n",
                                             __func__, __LINE__, child_proc[i].name[0]);
                                }
                                else {
                                    *(num_restarts_ptr + i) = num_restarts_ptr[i] + 1;
                                    LOC_LOGE("%s:%d]: Restarting %s. Number of restarts: %d\n",
                                             __func__, __LINE__, child_proc[i].name[0],
                                             num_restarts_ptr[i]);
                                    child_proc[i].proc_status = ENABLED;
                                }
                            }
                            else {
                                LOC_LOGE("%s:%d]: Exit status 0. Not Restarting\n",
                                         __func__, __LINE__);
                                child_proc[i].proc_status = DISABLED;
                            }
                        }
                        else if (WIFSIGNALED(child_status)) {
                            LOC_LOGE("%s:%d]: Child exited due to signal: %s\n",
                                     __func__, __LINE__, strsignal(WTERMSIG(child_status)));
                            if(num_restarts_ptr[i] == MAX_CHILD_PROC_RESTARTS) {
                                child_proc[i].proc_status = DISABLED;
                                LOC_LOGE("%s:%d]: %s Restarted too many times. Grounded!\n",
                                         __func__, __LINE__, child_proc[i].name[0]);
                            }
                            else {
                                *(num_restarts_ptr + i) = num_restarts_ptr[i] + 1;
                                LOC_LOGE("%s:%d]: Restarting %s. Number of restarts: %d\n",
                                         __func__, __LINE__, child_proc[i].name[0],
                                         num_restarts_ptr[i]);
                                child_proc[i].proc_status = ENABLED;
                            }
                        }
                        else {
                            LOC_LOGE("%s:%d]:No Reason found\n", __func__, __LINE__);
                        }
                    }
                    else {
                        LOC_LOGD("%s:%d]: Either the %s did not exit or waitpid returned error\n",
                                 __func__, __LINE__, child_proc[i].name[0]);
                    }
                    pthread_mutex_unlock(&signal_handler_mutex);
                }
            }
        } else if (SIGALRM == signal_num) {
            LOC_LOGD("%s:%d]:sigalrm_received\n",__func__, __LINE__);
            for (i = 0; i < proc_list_length; i++) {
                if ((child_proc[i].proc_status == RUNNING) ||
                    (child_proc[i].proc_status == ENABLED)) {
                    LOC_LOGD("%s:%d]: Child_id: %d; child name: %s; num_restarts: %d\n",
                             __func__, __LINE__, child_proc[i].proc_id, child_proc[i].name[0],
                             num_restarts_ptr[i]);
                    *(num_restarts_ptr+i) = 0;
                }
            }
            alarm(ONE_DAY_SECONDS);
        }
        // notify parent of signal handled.
        LOC_LOGD("Notifying parent of signal handling\n");
        pthread_mutex_lock(&signal_handling_done_mutex);
        pthread_cond_signal(&signal_handling_done_cond);
        pthread_mutex_unlock(&signal_handling_done_mutex);
    }
    LOC_LOGD("Exit signal handler thread\n");
    return NULL;
}

using namespace qc_loc_fw;

static int registerMqClient();
static int subscribeForSysStatusUpdate();

void loc_launcher_process_bringUp_update(LocLaunchTriggerMask inTriggerMask) {
    if (locProcessEventBringUpMask != inTriggerMask) {
        locProcessEventBringUpMaskMutex.lock();
        LOC_LOGd("Current Trigger mask: %d New Trigger Mask: %d", locProcessEventBringUpMask,
            inTriggerMask);
        locProcessEventBringUpMask = inTriggerMask;
        locProcessEventBringUpMaskMutex.unlock();
        pthread_mutex_lock(&signal_handling_done_mutex);
        locProcLauncherMaskConsumed = false;
        pthread_cond_signal(&signal_handling_done_cond);
        pthread_mutex_unlock(&signal_handling_done_mutex);
    }
}

class LocLauncherMqCallback: public MessageQueueServiceCallback
{
public:
    int newMsg(InMemoryStream* new_buffer) override;
    void processSystemStatusInfoEvent(InPostcard* card);
};

int LocLauncherMqCallback::newMsg(InMemoryStream* new_buffer) {
    LOC_LOGd("Incoming Postcard to Loc Launcher");

    if (NULL == new_buffer) {
        LOC_LOGe("Null card buffer");
        return -1;
    }

    InPostcard* card = InPostcard::createInstance();
    if (card == NULL) {
        LOC_LOGe("Null InPostcard card buffer");
        return -1;
    }

    card->init(new_buffer->getBuffer(), new_buffer->getSize());

    const char* from = NULL;
    card->getString("FROM", &from);
    LOC_LOGd("FROM: %s", from);

    const char* info = NULL;
    card->getString("INFO", &info);
    LOC_LOGd("INFO: %s", info);

    const char* client = NULL;
    card->getString("CLIENT", &client);
    LOC_LOGd("CLIENT: %s", client);

    // Re-subscribe when any new client registers
    if (NULL != info && 0 == strcmp(info, "REGISTER-EVENT") &&
            NULL != client && 0 != strcmp(client, MQ_CLIENT_LOC_LAUNCHER)) {
        int ret = -1;
        if (0 != (ret = subscribeForSysStatusUpdate())) {
            LOC_LOGe("Failed to subscribe, ret %d", ret);
        }
    } else if (NULL != info && (0 == strcmp(info, "OS-STATUS-UPDATE"))) {
        // process Info event
        processSystemStatusInfoEvent(card);
    }
    delete new_buffer;
    delete card;
    return 0;
}

void LocLauncherMqCallback::processSystemStatusInfoEvent(InPostcard* card) {
    LocLaunchTriggerMask inTriggerMask = locProcessEventBringUpMask;
    bool fieldStatus = false;
    InPostcard* innerCard = nullptr;

    // Look for user pref dataitem
    if ((0 == card->getCard("ENH", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_QUALCOMM_ENHANCED_PROVIDER_ENABLED", fieldStatus))) {
        LOC_LOGv("Recvd newUserPref: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_OPT_IN, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    if ((0 == card->getCard("IN_EMERGENCY_CALL", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_EMERGENCY", fieldStatus))) {
        // Look for emergency call dataitem
        LOC_LOGv("Recvd emergency event: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_EMERGENCY, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    // Look for precise location enabled dataitem
    if ((0 == card->getCard("PRECISE_LOCATION_ENABLED", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_PRECISE_LOCATION_ENABLED", fieldStatus))) {
        LOC_LOGv("Recvd Precise location: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_PRECISE_TRACKING_START, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    // Look for tracking started dataitem
    if ((0 == card->getCard("TRACKING_STARTED", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_TRACKING_STARTED", fieldStatus))) {
        LOC_LOGv("Recvd Tracking Started: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_TRACKING_START, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    // Look for Ntrip started dataitem
    if ((0 == card->getCard("NTRIP_STARTED", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_NTRIP_STARTED", fieldStatus))) {
        LOC_LOGv("Recvd Ntrip Started: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_NTRIP_START, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    // Look for gps state dataitem
    if ((0 == card->getCard("GPS_STATE", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_GPS_PROVIDER_ENABLED", fieldStatus))) {
        LOC_LOGv("Recvd gps state: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_LOCATION_ENABLE, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    // Look for NLP session started dataitem
    if ((0 == card->getCard("NLP_SESSION_STARTED", &innerCard)) && (innerCard != nullptr) &&
            (0 == innerCard->getBool("IS_NLP_SESSION_STARTED", fieldStatus))) {
        LOC_LOGv("Recvd NLP Session Started: %d", fieldStatus);
        updateBitInMask(inTriggerMask, LOC_ON_NLP_SESSION_START, fieldStatus);
        delete innerCard;
        innerCard = nullptr;
    }

    loc_launcher_process_bringUp_update(inTriggerMask);
}

static pthread_t mq_client_thread;
static MessageQueueClient* mq_client;

// Start listening for IPC messages
static void process_launcher_start_ipc_listener() {
    // Create a thread to handle MQ client messages
    int result = pthread_create(&mq_client_thread, NULL, mq_client_thread_fn, NULL);
    if (result != 0) {
        LOC_LOGE("pthread_create failed. Error = %d", result);
    }
    result = pthread_setname_np(mq_client_thread, (char *) "loc_mq_clnt");
    if (result != 0) {
        LOC_LOGE("pthread_setname_np failed, err %d", result);
    }
}

static void* mq_client_thread_fn(void* arg)
{
    LOC_LOGD("Loc Launcher MQ Client thread");

    int ret = -1;

    do {
        mq_client = MessageQueueClient::createInstance();
        if (NULL == mq_client) {
            LOC_LOGE("Failed to create MQ client instance.");
            break;
        }

        ret = mq_client->setServerNameDup(MQ_SERVER_SOCKET_NAME);
        if (0 != ret) {
            LOC_LOGE("setServerNameDup failed, ret %d", ret);
            break;
        }

        ret = mq_client->connect();
        if (0 != ret) {
            LOC_LOGE("MQ Connect failed, ret %d", ret);
            break;
        }

        LOC_LOGD("MQ connect successful");

        if (0 != (ret = registerMqClient())) {
            LOC_LOGE("Failed to register, ret %d", ret);
            break;
        }

        if (0 != (ret = subscribeForSysStatusUpdate())) {
            LOC_LOGE("Failed to subscribe, ret %d", ret);
            break;
        }

        LocLauncherMqCallback mqCallback;
        ret = mq_client->run_block(&mqCallback);
        if (0 != ret) {
            LOC_LOGE("Failed to run_block, ret %d", ret);
        }

    } while (0);

    return arg;
}

static int registerMqClient()
{
    LOC_LOGD("LOC_LAUNCHER > REGISTER");

    int ret = -1;

    // Send Register message
    OutPostcard * card = OutPostcard::createInstance();
    if (NULL != card && NULL != mq_client) {
      card->init();
      card->addString("TO", "SERVER");
      card->addString("FROM", MQ_CLIENT_LOC_LAUNCHER);
      card->addString("REQ", "REGISTER");
      card->finalize();
      ret = mq_client->send(card->getEncodedBuffer());
      delete card;
    }
    return ret;
}

static int subscribeForSysStatusUpdate() {

    LOC_LOGd("LOC_LAUNCHER > SUBSCRIBE OS-AGENT update");
    int ret = -1;
    // Send Subscribe message
    OutPostcard * card = OutPostcard::createInstance();
    if (NULL != card && NULL != mq_client) {
        card->init();
        card->addString("TO", "OS-Agent");
        card->addString("FROM", MQ_CLIENT_LOC_LAUNCHER);
        card->addString("REQ", "SUBSCRIBE");
        int32_t dataitems[] = {
                ENH_DATA_ITEM_ID,
                IN_EMERGENCY_CALL_DATA_ITEM_ID,
                GPSSTATE_DATA_ITEM_ID,
                PRECISE_LOCATION_ENABLED_DATA_ITEM_ID,
                TRACKING_STARTED_DATA_ITEM_ID,
                NTRIP_STARTED_DATA_ITEM_ID,
                NETWORK_POSITIONING_STARTED_DATA_ITEM_ID,
        };
        card->addArrayInt32("DATA-ITEMS", (sizeof(dataitems) / sizeof(int32_t)), dataitems);
        card->finalize();
        ret = mq_client->send(card->getEncodedBuffer());
        delete card;
    }
    return ret;
}
