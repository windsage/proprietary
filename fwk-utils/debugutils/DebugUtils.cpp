/******************************************************************************
Copyright (c) 2022,2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include "DebugUtils.h"
#include <stdlib.h>
#include <dirent.h>
#include <sstream>
#include <stdio.h>
#include <sys/ptrace.h>
#include <elf.h>
#include <err.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <android-base/logging.h>
#include "android-base/properties.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace debugutils {

using std::chrono::system_clock;
using namespace std;
const char sContinueFromBreakpoint[] = "vendor.debug.breakpoint.continue";
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/* Default dir path */
string debugUtilsDir = "/data/local/tmp/";

struct breakpoint_arg_struct {
    bool isProcess;
    uint64_t pid;
    string in_debugTag;
};

struct watchpoint_arg_struct {
    uint64_t pid;
    uint64_t add;
    string in_debugTag;
};

struct periodic_trace_struct {
    uint64_t pid;
    uint64_t duration;
};

struct arg_struct {
arg_struct() : duration(0), syncNeeded(false){}
string command;
string dirName;
string fileName;
string in_debugTag;
uint64_t duration;
int status;
bool syncNeeded;
};

// Return process pid
int getProcIdByName(const std::string& process_name) {
    const std::string command = std::string("pidof -s ") + process_name;
    FILE* file = popen(command.c_str(), "r");
    char buffer[1024];
    if (file == nullptr) {
        LOG(ERROR) << "unable to open file for " << command;
        return 0;
    }
    if (fgets(buffer, 1024, file) == nullptr) {
        LOG(ERROR) << "Unable to open file for " << command;
        pclose(file);
        return 0;
    }
    pclose(file);
    return strtoul(buffer, nullptr, 10);
}

/* Return file path in string format:
   Default - '<debugUtilsDir>/<outputDir>/<fileName>_<in_debugTag>_<time>.txt'
   If outputDir dir cannot be created in '<debugUtilsDir>'
   then path will be set to '<debugUtilsDir>/<fileName>_<in_debugTag>_<time>.txt' */
string getFilePath(string outputDir, string fileName, string in_debugTag, bool syncNeeded=false) {
    string outputDirPath = debugUtilsDir + outputDir + "/";
    /* Create directory*/
    struct stat st;
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR |
                 S_IRGRP | S_IWGRP | S_IXGRP |
                 S_IROTH | S_IWOTH | S_IXOTH;

    if (syncNeeded) pthread_mutex_lock(&lock);
    if( stat( outputDirPath.c_str(), &st ) != 0 )
    {
      /* Directory does not exist */
      if( mkdir( outputDirPath.c_str(), mode ) != 0 )
      {
         LOG(INFO) << "Directory " << outputDir <<" could not be created in " << debugUtilsDir << errno;
         outputDirPath = debugUtilsDir ;
      }
      else
         LOG(INFO) << " Directory created " ;
    }
    else if( !S_ISDIR( st.st_mode ) )
    {
      errno = ENOTDIR;
      LOG(INFO) << " Given path is not a dir : " <<errno ;
      outputDirPath = debugUtilsDir ;
    }
    if (syncNeeded) pthread_mutex_unlock(&lock);

    /* Current time */
    system_clock::time_point p = system_clock::now();
    time_t t = system_clock::to_time_t(p);

    /* Output file path */
    string filePath = outputDirPath + fileName + "_" + in_debugTag + "_" + to_string(t) + ".txt" ;

    return filePath;
}

/*
   Executes command and save output in filePath
   command - Command to be executed in shell with absolute path to binary
             Exp: /system/bin/dumpsys window
   filePath - Absolute file path to save console output. Pass empty string ""
              if you don't want to save console output
   duration - read from pipe for this duration
              Default, 0 , means read till EOF
*/
int executeCommand(string command, string filePath, uint64_t duration=0) {
    /* Command to execute */
    string cmd = command + " 2>&1 ";
    char cmdArray [cmd.length()+1];
    strlcpy(cmdArray , cmd.c_str() , cmd.length()+1);

    char buffer[128];
    ofstream file_out;
    if (!filePath.empty()) {
      file_out.open (filePath, ofstream::out | ofstream::app);
      if (file_out.fail())
      {
        LOG(ERROR) << "Failed to open file :" <<errno;
        return -1;
      }
    }

    FILE* pipe = popen(cmdArray, "r");
    if (!pipe) {
      LOG(ERROR) << "popen failed!";
      return -1;
    }
    if (!filePath.empty()) {
      if (duration>0) // read for that particular duration and write to file
      {
        system_clock::time_point currentTime = system_clock::now();
        time_t endTime  = system_clock::to_time_t(currentTime) + duration + 10;
        while ( endTime >= system_clock::to_time_t(system_clock::now())) {
          if (fgets(buffer, 128, pipe) != NULL)
             file_out << buffer;
        }
      }
      else // read till end of process and write to file
      {
        while (!feof(pipe)) {
          if (fgets(buffer, 128, pipe) != NULL)
             file_out << buffer;
       }
      }
      file_out.close();
    } else {
      while (!feof(pipe)) {
          if (fgets(buffer, 128, pipe) != NULL)
             LOG(INFO) << buffer;
      }
    }

    return WEXITSTATUS(pclose(pipe));
}

void* executeCommandHelper(void *arguments) {
    struct arg_struct *args = (struct arg_struct *)arguments;
    string filePath = getFilePath(args->dirName, args->fileName, args->in_debugTag, args->syncNeeded);
    int retStatus = executeCommand(args->command, filePath, args->duration);
    if (retStatus == -1)
       LOG(INFO) << "*** Command could not be executed ***" ;
    else
       LOG(ERROR) << "*** Execution o/p at "<< filePath <<" ***" ;
    args->status = retStatus ;
    pthread_exit(NULL);
}

void* collect_periodic_trace(void *arguments) {
    struct periodic_trace_struct *args = (struct periodic_trace_struct *)arguments;
    uint64_t pid = args->pid;
    uint64_t duration = args->duration;
    while(true) {
        if(kill(pid, SIGQUIT)) {
            LOG(ERROR) << "SIGQUIT sent to process" << pid ;
        } else {
            LOG(ERROR) << " SIGQUIT not sent to process" << pid;
        }
        sleep(duration);
    }
}

void* init_breakpoint_set(void *arguments) {
    struct breakpoint_arg_struct *args = (struct breakpoint_arg_struct *)arguments;
    //LOG(ERROR) << args->pid <<  args->in_debugTag ;
    android::base::SetProperty(sContinueFromBreakpoint, "false");
    uint64_t pid = args->pid;
    if(!args->isProcess) {
        ptrace(PTRACE_ATTACH,pid, NULL, NULL);
        LOG(ERROR) <<"error value of ptrace attach " << strerror(errno);
        while (1) {
            const std::string contProcess = android::base::GetProperty(sContinueFromBreakpoint, "");
            LOG(ERROR) <<"value of property is " << contProcess;
            if (contProcess == "true") {
                ptrace(PTRACE_DETACH, pid, NULL, 35);
                break;
            }
        }
    } else {
        if(kill(pid, SIGSTOP)) {
            LOG(ERROR) << "sigstop sent to process" << pid ;
        } else {
            LOG(ERROR) << " sigstop not sent to process" << pid;
        }
        while (1) {
            const std::string contProcess = android::base::GetProperty(sContinueFromBreakpoint, "");
            LOG(ERROR) <<"value of property is " << contProcess;
            if (contProcess == "true") {
                kill(pid, SIGCONT);
                break;
            }
        }
    }
    return NULL;
}

static void set_watchpoint(pid_t child, uintptr_t address, size_t size) {
     LOG(ERROR) <<"pid in set_watchpoint" << child;
     LOG(ERROR) << " address in setwatchpoint is " << address;
#if defined(__arm__) || defined(__aarch64__)
    const unsigned byte_mask = (1 << size) - 1;
       const unsigned type = 2; // Write.
       const unsigned enable = 1;
       const unsigned control = byte_mask << 5 | type << 3 | enable;
#ifdef __arm__
       ptrace(PTRACE_SETHBPREGS, child, -1, &address);
       LOG(ERROR) <<"error value of ptrace addressis " << strerror(errno);
       ptrace(PTRACE_SETHBPREGS, child, -2, &control);
       LOG(ERROR) <<"error value of ptrace control " << strerror(errno);
#else // aarch64
       user_hwdebug_state dreg_state;
       memset(&dreg_state, 0, sizeof dreg_state);
       dreg_state.dbg_regs[0].addr = address;
       dreg_state.dbg_regs[0].ctrl = control;
       iovec iov;
       iov.iov_base = &dreg_state;
       iov.iov_len = offsetof(user_hwdebug_state, dbg_regs) + sizeof(dreg_state.dbg_regs[0]);
       ptrace(PTRACE_SETREGSET, child, NT_ARM_HW_WATCH, &iov);
       LOG(ERROR) <<"error value of ptrace aarch64 control " << strerror(errno);
#endif
#endif
}

void* init_watchpoint_set(void *arguments) {
    struct watchpoint_arg_struct *args = (struct watchpoint_arg_struct *)arguments;
    uint64_t pid = args->pid;
    uint64_t add = args->add;
    LOG(ERROR) << "pid is " << pid;
    LOG(ERROR) << "address is " << add;
    int status;
    ptrace(PTRACE_ATTACH,pid, NULL, NULL);
    LOG(ERROR) <<"error value of ptrace attach " << strerror(errno);
    set_watchpoint(pid,add, sizeof(void*));
    ptrace(PTRACE_CONT, pid, NULL, nullptr);
    LOG(ERROR) <<"error value of ptrace cont " << strerror(errno);
    int ret = TEMP_FAILURE_RETRY(waitpid(pid, &status, __WALL));
    LOG(ERROR) << "Waitpid return hit with status" << status << "waitpid retrun is " << ret << "value is " << strerror(errno);
    if (WIFSTOPPED(status)==true)
        LOG(ERROR) << " process stopped with signal" << WSTOPSIG(status);
    if(SIGTRAP==WSTOPSIG(status)) {
        LOG(ERROR) << "process hit with signal" << WSTOPSIG(status);
        ptrace(PTRACE_DETACH, pid, NULL, 35);
    }
return NULL;
}

int is_detached(string key) {
    string mPerfettoDetachedCmd = "/system/bin/perfetto --is_detached=";
    mPerfettoDetachedCmd.append(key);
    string mOutputDir = "perfettoDir";
    string mFileName = "PerfettoDetached";

    LOG(INFO) << "Perfetto Command to be executed " << mPerfettoDetachedCmd ;
    return executeCommand(mPerfettoDetachedCmd, "");
}


::ndk::ScopedAStatus DebugUtils::collectBinderDebugInfoByPid(int64_t in_pid, bool in_isBlocking, const std::string& in_debugTag) {

    DIR* dir = opendir("/sys/kernel/debug/binder/proc/");
    string command;
    if (dir) {
       command = " cat /sys/kernel/debug/binder/proc/";
    }else {
       command = "cat /dev/binderfs/binder_logs/proc/";
    }
    command=command+to_string(in_pid);
    string mOutputDir = "BinderDebugInfo";
    string mFileName = "BinderDebugInfoByPid_"+to_string(in_pid);
    struct arg_struct *args = new arg_struct();
    args->command = command;
    args->dirName = mOutputDir;
    args->fileName = mFileName;
    args->in_debugTag = in_debugTag.c_str();
    pthread_t BinderDebugInfoByPid_thread;
    if (pthread_create(&BinderDebugInfoByPid_thread, NULL, &executeCommandHelper, (void *)args) != 0) {

       LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    else
    {
       if (in_isBlocking)
         pthread_join(BinderDebugInfoByPid_thread, NULL);
        return ndk::ScopedAStatus::ok();
    }

}
::ndk::ScopedAStatus DebugUtils::collectBinderDebugInfoByProcessname(const std::string& in_processName, bool in_isBlocking, const std::string& in_debugTag) {
    int pid=getProcIdByName(in_processName);
    LOG(ERROR) <<"Pid returned from function:"<<pid;
    if(pid!=-1){
        LOG(ERROR) <<"status is  " << in_debugTag << in_processName;
        return collectBinderDebugInfoByPid(pid,in_isBlocking,in_debugTag);
    }
    else {
        LOG(ERROR) <<"No matching pid found corresponding to ProcessName :"<<in_processName;
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
}
::ndk::ScopedAStatus DebugUtils::collectCPUInfo(bool in_isBlocking, const std::string& in_debugTag) {
   string outputDirName = "CPUInfo";
   string cmd1 = "/system/bin/dumpsys cpuinfo";
   string fileName1 = "dumpsys_cpuinfo";

   /*
     Start top in batch mode with 1 iteration
     Show info of all threads
     Show fields : pid,tid,user,pr,ni,%cpu,time,%mem,s,pcy,cmd,name
     Sort by 6th field (based on %CPU)
   */
   string cmd2 = "top -b -n 1 -H -s 6 -o pid,tid,user,pr,ni,%cpu,time,%mem,s,pcy,cmd,name" ;
   string fileName2 = "topOutput";

   struct arg_struct *args1 = new arg_struct();
   struct arg_struct *args2 = new arg_struct();
   args1->command=cmd1;
   args1->dirName=outputDirName;
   args1->fileName=fileName1;
   args1->in_debugTag=in_debugTag.c_str();
   args1->syncNeeded=true;
   args2->command=cmd2;
   args2->dirName=outputDirName;
   args2->fileName=fileName2;
   args2->in_debugTag=in_debugTag.c_str();
   args2->syncNeeded=true;

   pthread_t cpuInfoThread[2];
   bool hasFailed[2] = {false,false};
   if (pthread_create(&cpuInfoThread[0], NULL, &executeCommandHelper, (void *)args1) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd1;
      hasFailed[0] = true; }

   if (pthread_create(&cpuInfoThread[1], NULL, &executeCommandHelper, (void *)args2) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd2;
      hasFailed[1] = true; }

   if (in_isBlocking)
     for (int i=0;i<2;i++)
        if (!hasFailed[i])
          pthread_join(cpuInfoThread[i], NULL);

   if (hasFailed[0]&&hasFailed[1])
      return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
   else
      return ndk::ScopedAStatus::ok();

}

::ndk::ScopedAStatus DebugUtils::collectDependentProcessStackTrace(int64_t in_pid, bool in_isBlocking, const std::string& in_debugTag) {

    vector<int> vec;
    ifstream input;
    string line;
    size_t pos;
    int i=0;
    DIR* dir = opendir("/sys/kernel/debug/binder/proc/");
    if (dir) {
       input.open("/sys/kernel/debug/binder/proc/" + to_string(in_pid));
       LOG (INFO) << "/sys/kernel/debug/binder/proc/" << to_string(in_pid);
    }
    else{
       input.open("/dev/binderfs/binder_logs/proc/" + to_string(in_pid));
       LOG (INFO) << "/dev/binderfs/binder_logs/proc/"<< to_string(in_pid);
    }
    if(input.is_open()) {
        while(getline(input,line)) {
            pos = line.find("outgoing transaction");
            if (pos!=string::npos) {
                size_t pos1 = line.find("to");
                string str2 =line.substr(pos1+3);
                stringstream check1(str2);
                int count=0;
                int num;
                string tokens;
                while (getline(check1, tokens,':')) {
                    if (count==1)
                        break;
                    int num=stoi(tokens);
                    vec.push_back(num);
                    count=1;
                }
            }
        }
    }
    else {
        LOG (ERROR) << " Unable to open proc pid file ";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    input.close();
    if (vec.empty()) {
        LOG (ERROR) << " No outgoing transaction for pid ";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    for(i=0;i<vec.size();i++) {
        int pid=vec[i];
        string command="/system/bin/debuggerd -b ";
        command=command+to_string(pid);
        string mOutputDir = "CollectDependentProcessStackTrace_Dir";
        string mFileName = "collectDependentProcessStackTrace_"+to_string(pid);

        struct arg_struct *args = new arg_struct();
        args->command = command;
        args->dirName = mOutputDir;
        args->fileName = mFileName;
        args->in_debugTag = in_debugTag.c_str();
        pthread_t collectDependentProcessStackTrace;
        if (pthread_create(&collectDependentProcessStackTrace, NULL, &executeCommandHelper, (void *)args)) {
            LOG(ERROR) <<" unable to create thread";
            return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
        }
        else {
            LOG(INFO) << in_debugTag.c_str() << " thread created for collectDependentProcessStackTrace";

        }   if (in_isBlocking) {
                pthread_join(collectDependentProcessStackTrace, NULL);
        }
    }
     return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DebugUtils::collectFullBinderDebugInfo(bool in_isBlocking, const std::string& in_debugTag) {

    // Collection of binder debug information.
    string outputDirName = "BinderDebugInfo";
    /*
      State
    */
    DIR* dir = opendir("/sys/kernel/debug/binder/proc/");
    string cmd1,cmd2,cmd3,cmd4,cmd5;
    if (dir) {
       cmd1 = "cat /sys/kernel/debug/binder/state";
       cmd2 = "cat /sys/kernel/debug/binder/stats" ;
       cmd3 = "cat /sys/kernel/debug/binder/transactions" ;
       cmd4 = "cat /sys/kernel/debug/binder/transaction_log" ;
       cmd5 = "cat /sys/kernel/debug/binder/failed_transaction_log" ;
    }else{
       cmd1 = "cat /dev/binderfs/binder_logs/state";
       cmd2 = "cat /dev/binderfs/binder_logs/stats" ;
       cmd3 = "cat /dev/binderfs/binder_logs/transactions" ;
       cmd4 = "cat /dev/binderfs/binder_logs/transaction_log" ;
       cmd5 = "cat /dev/binderfs/binder_logs/failed_transaction_log" ;
    }
    string fileName1 = "BinderDebugInfo_state";
    /*
      Stats
    */
    string fileName2 = "BinderDebugInfo_stats";
    /*
      Transactions
    */
    string fileName3 = "BinderDebugInfo_transactions";
    /*
      Transactions log
    */
    string fileName4 = "BinderDebugInfo_transaction_log";
    /*
      Failed transaction log
    */
    string fileName5 = "BinderDebugInfo_failed_transaction_log";


    struct arg_struct *args1 = new arg_struct();
    struct arg_struct *args2 = new arg_struct();
    struct arg_struct *args3 = new arg_struct();
    struct arg_struct *args4 = new arg_struct();
    struct arg_struct *args5 = new arg_struct();
    args1->command=cmd1;
    args1->dirName=outputDirName;
    args1->fileName=fileName1;
    args1->in_debugTag=in_debugTag.c_str();
    args2->command=cmd2;
    args2->dirName=outputDirName;
    args2->fileName=fileName2;
    args2->in_debugTag=in_debugTag.c_str();
    args3->command=cmd3;
    args3->dirName=outputDirName;
    args3->fileName=fileName3;
    args3->in_debugTag=in_debugTag.c_str();
    args4->command=cmd4;
    args4->dirName=outputDirName;
    args4->fileName=fileName4;
    args4->in_debugTag=in_debugTag.c_str();
    args5->command=cmd5;
    args5->dirName=outputDirName;
    args5->fileName=fileName5;
    args5->in_debugTag=in_debugTag.c_str();

    pthread_t BinderDebugInfoThread[5];
    bool hasFailed[5] = {false,false,false,false,false};

    if (pthread_create(&BinderDebugInfoThread[0], NULL, &executeCommandHelper, (void *)args1) != 0 ) {

       LOG(ERROR) <<" unable to create thread for " << cmd1;
       hasFailed[0] = true; }

    if (pthread_create(&BinderDebugInfoThread[1], NULL, &executeCommandHelper, (void *)args2) != 0 ) {

       LOG(ERROR) <<" unable to create thread for " << cmd2;
       hasFailed[1] = true; }

    if (pthread_create(&BinderDebugInfoThread[2], NULL, &executeCommandHelper, (void *)args3) != 0 ) {

       LOG(ERROR) <<" unable to create thread for " << cmd3;
       hasFailed[2] = true; }

    if (pthread_create(&BinderDebugInfoThread[3], NULL, &executeCommandHelper, (void *)args4) != 0 ) {

       LOG(ERROR) <<" unable to create thread for " << cmd4;
       hasFailed[3] = true; }

    if (pthread_create(&BinderDebugInfoThread[4], NULL, &executeCommandHelper, (void *)args5) != 0 ) {

       LOG(ERROR) <<" unable to create thread for " << cmd5;
       hasFailed[4] = true; }

    if (in_isBlocking)
      for (int i=0;i<5;i++)
         if (!hasFailed[i])
           pthread_join(BinderDebugInfoThread[i], NULL);

    if (hasFailed[0]&&hasFailed[1]&&hasFailed[2]&&hasFailed[3]&&hasFailed[4])
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    else
        return ndk::ScopedAStatus::ok();

}

::ndk::ScopedAStatus DebugUtils::collectHprof(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) {

    string command="init.fda.am.sh dumpheap ";
    command=command+to_string(in_pid);
    string mOutputDir = "collectHprof";
    string mFileName = "collectHprof_"+to_string(in_pid);
    struct arg_struct *args = new arg_struct();
    args->command = command;
    args->dirName = mOutputDir;
    args->fileName = mFileName;
    args->in_debugTag = in_debugTag.c_str();
    pthread_t collectHprof_thread;
    LOG(INFO) << "Inside collectHprof api call before executing command :" << command;
    if (pthread_create(&collectHprof_thread, NULL, &executeCommandHelper, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
       LOG(INFO) << in_debugTag.c_str() << " thread created for collectHprof";
       if (in_isBlocking)
          pthread_join(collectHprof_thread, NULL);
        return ndk::ScopedAStatus::ok();
      }

}

::ndk::ScopedAStatus DebugUtils::collectMemoryInfo(bool in_isBlocking, const std::string& in_debugTag) {
  string outputDirName = "MemInfo";
   string cmd1 = "/system/bin/dumpsys meminfo";
   string fileName1 = "dumpsys_meminfo";

   string cmd2 = "/system/bin/procrank" ;
   string fileName2 = "procrank";

   struct arg_struct *args1 = new arg_struct();
   struct arg_struct *args2 = new arg_struct();
   args1->command=cmd1;
   args1->dirName=outputDirName;
   args1->fileName=fileName1;
   args1->in_debugTag=in_debugTag.c_str();
   args1->syncNeeded=true;
   args2->command=cmd2;
   args2->dirName=outputDirName;
   args2->fileName=fileName2;
   args2->in_debugTag=in_debugTag.c_str();
   args2->syncNeeded=true;

   pthread_t memInfoThread[2];
   bool hasFailed[2] = {false,false};
   if (pthread_create(&memInfoThread[0], NULL, &executeCommandHelper, (void *)args1) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd1;
      hasFailed[0] = true; }

   if (pthread_create(&memInfoThread[1], NULL, &executeCommandHelper, (void *)args2) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd2;
      hasFailed[1] = true; }

   if (in_isBlocking)
     for (int i=0;i<2;i++)
        if (!hasFailed[i])
          pthread_join(memInfoThread[i], NULL);

   if (hasFailed[0]&&hasFailed[1])
      return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
   else
       return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DebugUtils::collectPeriodicTraces(int64_t in_pid, int64_t in_duration, const std::string& in_debugTag) {
    LOG(ERROR) << "in_debugTag for periodic trace is "<< in_debugTag;
    struct periodic_trace_struct *args = (struct periodic_trace_struct *)malloc(sizeof(struct periodic_trace_struct));
    if (args == NULL) {
        LOG(ERROR) <<"Unable to allocate memory";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    args->pid = in_pid;
    args->duration = in_duration;
    pthread_t mthread;
    if (pthread_create(&mthread, NULL, &collect_periodic_trace, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
         return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::collectProcessCPUInfo(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) {
   string outputDirName = "collectProcessCPUInfo"+to_string(in_pid);
    string cmd1 = "/system/bin/dumpsys cpuinfo |grep " +to_string(in_pid);
    string fileName1 = "dumpsys_cpuinfo"+to_string(in_pid);
    string cmd2 = "top -b -n 1 -H -s 6 -o pid,tid,user,pr,ni,%cpu,time,%mem,s,pcy,cmd,name |grep " +to_string(in_pid);
    string fileName2 = "topOutput"+to_string(in_pid);

   struct arg_struct *args1 = new arg_struct();
   struct arg_struct *args2 = new arg_struct();
   args1->command=cmd1;
   args1->dirName=outputDirName;
   args1->fileName=fileName1;
   args1->in_debugTag=in_debugTag.c_str();
   args1->syncNeeded=true;
   args2->command=cmd2;
   args2->dirName=outputDirName;
   args2->fileName=fileName2;
   args2->in_debugTag=in_debugTag.c_str();
   args2->syncNeeded=true;

   pthread_t cpuInfoThread[2];
   bool hasFailed[2] = {false,false};
   if (pthread_create(&cpuInfoThread[0], NULL, &executeCommandHelper, (void *)args1) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd1;
      hasFailed[0] = true; }

   if (pthread_create(&cpuInfoThread[1], NULL, &executeCommandHelper, (void *)args2) != 0 ) {
      LOG(ERROR) <<" unable to create thread for " << cmd2;
      hasFailed[1] = true; }

   if (in_isBlocking)
     for (int i=0;i<2;i++)
        if (!hasFailed[i])
          pthread_join(cpuInfoThread[i], NULL);

   if (hasFailed[0]&&hasFailed[1])
      return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
   else
       return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DebugUtils::collectProcessMemoryInfo(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) {

 string binPath = "/system/bin/" ;

    /* Command to execute */
    LOG(INFO) << in_isBlocking;
    string cmd = binPath + "procrank |grep " +to_string(in_pid);
    string outputDirName = "collectProcessMemoryInfoDir";
    string fileName = "collectProcessMemoryInfo"+to_string(in_pid);

    struct arg_struct *args = new arg_struct();
    args->command=cmd;
    args->dirName=outputDirName;
    args->fileName=fileName;
    args->in_debugTag=in_debugTag.c_str();

    pthread_t memInfoThread;
    if (pthread_create(&memInfoThread, NULL, &executeCommandHelper, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
        LOG(INFO) << in_debugTag.c_str() << " thread created for collectProcessMemoryInfo" ;
        if (in_isBlocking)
            pthread_join(memInfoThread, NULL);
         return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::collectRamdump(const std::string& in_debugTag) {
    std::ofstream myfile ("/proc/sysrq-trigger");
    if (myfile.is_open()){
        myfile << "c";
        myfile.close();
    }
    else {
        LOG(ERROR) << "Failed to open file :";
        LOG(ERROR) <<"collectRamdump status is  " << in_debugTag;
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    LOG(INFO) << in_debugTag << "***collectRamdump is successful***" ;
     return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DebugUtils::collectStackTraceByPid(int64_t in_pid, bool in_isJava, bool in_isBlocking, const std::string& in_debugTag)  {

    if (!in_isJava) {
        string command="/system/bin/debuggerd -b ";
        command=command+to_string(in_pid);
        string mOutputDir = "collectStackTraceByPid_Dir";
        string mFileName = "collectStackTraceByPid_"+to_string(in_pid);
        struct arg_struct *args = new arg_struct();
        args->command = command;
        args->dirName = mOutputDir;
        args->fileName = mFileName;
        args->in_debugTag = in_debugTag.c_str();

        pthread_t collectStackTraceByPid_thread;
        if (pthread_create(&collectStackTraceByPid_thread, NULL, &executeCommandHelper, (void *)args)!= 0) {
            LOG(ERROR) <<" unable to create thread";
            return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
        }
       else {
            LOG(INFO) << in_debugTag.c_str() << " thread created for collectStackTraceByPid";
            if (in_isBlocking)
                pthread_join(collectStackTraceByPid_thread, NULL);
             return ndk::ScopedAStatus::ok();
        }
    } else {
        if(kill(in_pid, SIGQUIT)) {
            LOG(ERROR) << "sigquit sent to process" << in_pid ;
            return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
        } else {
            LOG(ERROR) << " sigquit not sent to process" << in_pid;
             return ndk::ScopedAStatus::ok();
        }
    }
}

::ndk::ScopedAStatus DebugUtils::collectStackTraceByProcessName(const std::string& in_processName, bool in_isJava, bool in_isBlocking, const std::string& in_debugTag) {
    int pid=getProcIdByName(in_processName);
    LOG(ERROR) <<"Pid returned from function:"<<pid;
    if(pid!=-1){
        LOG(ERROR) <<"status is  " << in_debugTag<<in_processName<<in_isJava<<in_isBlocking;
        return collectStackTraceByPid(pid, in_isJava,in_isBlocking,in_debugTag);
    }
    else {
        LOG(ERROR) <<"No matching pid found corresponding to ProcessName :"<<in_processName;
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus DebugUtils::collectUserspaceLogs(const std::string& in_logCmd, const std::string& in_debugTag, int64_t in_duration) {
   //Logcat binary path
    string binPath = "/system/bin/" ;

    /* Command to execute */
    string cmd = binPath + in_logCmd.c_str();
    string outputDirName = "LogcatDir";
    string fileName = "logcat";

    struct arg_struct *args = new arg_struct();
    args->command=cmd;
    args->dirName=outputDirName;
    args->fileName=fileName;
    args->in_debugTag=in_debugTag.c_str();
    args->duration = in_duration;
    pthread_t logcatThread;
    if (pthread_create(&logcatThread, NULL, &executeCommandHelper, (void *)args) != 0) {
       LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    else
    {
       LOG(INFO) << in_debugTag.c_str() << " thread created for logcat" ;
        return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::executeDumpsysCommands(const std::string& in_command, bool in_isBlocking, const std::string& in_debugTag) {
    //Dumpsys binary path
    string binPath = "/system/bin/" ;

    /* Command to execute */
    string cmd = binPath + in_command.c_str();
    string outputDirName = "dumpsysDir";
    string fileName = "dumpsys";

    struct arg_struct *args = new arg_struct();
    args->command=cmd;
    args->dirName=outputDirName;
    args->fileName=fileName;
    args->in_debugTag=in_debugTag.c_str();

    pthread_t dumpsysThread;
    if (pthread_create(&dumpsysThread, NULL, &executeCommandHelper, (void *)args) != 0) {
       LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    else
    {
       LOG(INFO) << in_debugTag.c_str() << " thread created for dumpsys" ;
       if (in_isBlocking)
          pthread_join(dumpsysThread, NULL);
       return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::setBreakPoint(int64_t in_pid, bool in_isProcess, const std::string& in_debugTag) {
    LOG(ERROR) <<"in_debugTag for breakpont call is " << in_debugTag ;
    struct breakpoint_arg_struct *args = (struct breakpoint_arg_struct *)malloc(sizeof(struct breakpoint_arg_struct));
    if (args == NULL) {
        LOG(ERROR) <<"Unable to allocate memory";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    args->pid = in_pid;
    args->isProcess = in_isProcess;

    //args->in_debugTag = in_debugTag;
    pthread_t mthread;
    if (pthread_create(&mthread, NULL, &init_breakpoint_set, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
         return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::setWatchPoint(int64_t in_pid, int64_t in_add, const std::string& in_debugTag) {
    LOG(ERROR) <<"status is  " << in_debugTag;
    struct watchpoint_arg_struct *args = (struct watchpoint_arg_struct *)malloc(sizeof(struct watchpoint_arg_struct));
    if (args == NULL) {
    LOG(ERROR) <<"Unable to allocate memory";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    args->pid = in_pid;
    args->add = in_add;
    pthread_t mthread;
    if (pthread_create(&mthread, NULL, &init_watchpoint_set, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
         return ndk::ScopedAStatus::ok();
   }


}

::ndk::ScopedAStatus DebugUtils::startPerfettoTracing(int64_t in_duration, const std::string& in_debugTag) {
    LOG(ERROR) <<"in_debugTag for perfetto call is   " << in_debugTag;
    if(executeCommand("echo '0' > /sys/kernel/debug/tracing/tracing_on","")) {
         LOG(INFO) << "Tracing is not disabled by debugutils";
    } else {
         LOG(INFO) << "Tracing is disabled by debugutils";
    }
    string mPerfettoCmd = "/system/bin/perfetto --txt --config /vendor/etc/PerfettoConfig.cfg --out /data/misc/perfetto-traces/trace_";
    mPerfettoCmd.append(in_debugTag.c_str());
    if(in_duration > 0) {
        string mDetach = " --detach=";
        mPerfettoCmd.append(mDetach + in_debugTag.c_str());
    }
    string mOutputDir = "perfettoDir";
    string mFileName = "StartPerfetto";
    LOG(INFO) << "Perfetto Command to be executed " << mPerfettoCmd;

    struct arg_struct *args = new arg_struct();
    args->command = mPerfettoCmd;
    args->dirName = mOutputDir;
    args->fileName = mFileName;
    args->in_debugTag = in_debugTag.c_str();

    pthread_t mPerfettoThread;
    if (pthread_create(&mPerfettoThread, NULL, &executeCommandHelper, (void *)args) != 0) {
       LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    else
    {
       LOG(INFO) << in_debugTag.c_str() << " thread created for StartPerfetto" ;
       sleep(2);
        return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::startSimplePerfTracing(int64_t in_pid, int64_t in_duration, const std::string& in_debugTag) {
   std::string executable_path = "/system/bin/simpleperf";
    std::string options_1 = " record -g -p  ";
    system_clock::time_point p = system_clock::now();
    time_t t = system_clock::to_time_t(p);
    std::string trace_path  = std::string("/data/local/tmp/perf") + "_" + to_string(t) + in_debugTag.c_str() + ".data";
    std::string option_2 = " -g -o " + trace_path + " --duration " + to_string(in_duration);
    std::stringstream pid_str;
    pid_str << in_pid;
    std::string tracee_pid = pid_str.str();
    std::string full_commmand = executable_path + options_1 + tracee_pid + option_2;
    string outputDirName = "SimplePerf";
    string fileName = "simpleperf"+to_string(in_pid);

    struct arg_struct *args = new arg_struct();
    args->command=full_commmand;
    args->dirName=outputDirName;
    args->fileName=fileName;
    args->in_debugTag=in_debugTag.c_str();

    pthread_t simpleperfThread;
    if (pthread_create(&simpleperfThread, NULL, &executeCommandHelper, (void *)args) != 0) {
        LOG(ERROR) <<" unable to create thread";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    } else {
        LOG(INFO) << in_debugTag.c_str() << " thread created for SimplePerf" ;
         return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::stopPerfettoTracing(const std::string& in_debugTag) {
  int detached = is_detached(in_debugTag.c_str());
    if(detached == 1) {
        LOG(ERROR) << "is_detached failed due to general error, e.g., wrong cmdline, cannot reach the service ";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    if(detached == 2) {
        LOG(ERROR) << "No Detached Session with the given Key is found.";
        LOG(ERROR) << "Possibly calling this API twice (or) without startPerfetto";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    if( detached == -1) {
        LOG(ERROR) << "is_detached thread creation failed";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    string mPerfettoCmd = "/system/bin/perfetto --stop --attach=";
    mPerfettoCmd.append(in_debugTag.c_str());
    string mOutputDir = "perfettoDir";
    string mFileName = "StopPerfetto";

    LOG(INFO) << "Perfetto Command to be executed " << mPerfettoCmd;

    struct arg_struct *args = new arg_struct();
    args->command = mPerfettoCmd;
    args->dirName = mOutputDir;
    args->fileName = mFileName;
    args->in_debugTag = in_debugTag.c_str();

    pthread_t mPerfettoThread;
    if (pthread_create(&mPerfettoThread, NULL, &executeCommandHelper, (void *)args) != 0) {
       LOG(ERROR) <<" unable to create thread";
       return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    else
    {
       LOG(INFO) << in_debugTag.c_str() << " thread created for StopPerfetto" ;
        return ndk::ScopedAStatus::ok();
    }
}

::ndk::ScopedAStatus DebugUtils::stopSimplePerfTracing(const std::string& in_debugTag) {
    int total_length = 256;
    char line[total_length];
    FILE * command = popen("pidof -s simpleperf","r");
    if (command == NULL) {
        LOG(ERROR)<< "unable to get simpleperf pid";
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
    fgets(line,total_length,command);
    pid_t pid = strtoul(line,NULL,10);
    LOG(ERROR)<< "simpleperf pid is" << pid;
    pclose(command);

    if (kill(pid, SIGINT)) {
        LOG(INFO) << in_debugTag << "***SIGINT sent to simpleperfd***" ;
         return ndk::ScopedAStatus::ok();
    } else {
        LOG(INFO) << in_debugTag << "***SIGINT not sent to simpleperfd***" << errno;
        return ndk::ScopedAStatus::fromStatus(STATUS_INVALID_OPERATION);
    }
}

}
}
}
}
}


