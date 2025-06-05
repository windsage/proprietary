/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/hardware/debugutils/BnDebugUtils.h>


namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace debugutils {

class DebugUtils : public BnDebugUtils {
    ::ndk::ScopedAStatus collectBinderDebugInfoByPid(int64_t in_pid, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectBinderDebugInfoByProcessname(const std::string& in_processName, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectCPUInfo(bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectDependentProcessStackTrace(int64_t in_pid, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectFullBinderDebugInfo(bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectHprof(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectMemoryInfo(bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectPeriodicTraces(int64_t in_pid, int64_t in_duration, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectProcessCPUInfo(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectProcessMemoryInfo(bool in_isBlocking, int64_t in_pid, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectRamdump(const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectStackTraceByPid(int64_t in_pid, bool in_isJava, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectStackTraceByProcessName(const std::string& in_processName, bool in_isJava, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus collectUserspaceLogs(const std::string& in_logCmd, const std::string& in_debugTag, int64_t in_duration) override;
    ::ndk::ScopedAStatus executeDumpsysCommands(const std::string& in_command, bool in_isBlocking, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus setBreakPoint(int64_t in_pid, bool in_isProcess, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus setWatchPoint(int64_t in_pid, int64_t in_add, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus startPerfettoTracing(int64_t in_duration, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus startSimplePerfTracing(int64_t in_pid, int64_t in_duration, const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus stopPerfettoTracing(const std::string& in_debugTag) override;
    ::ndk::ScopedAStatus stopSimplePerfTracing(const std::string& in_debugTag) override;
};
}
}
}
}
}
