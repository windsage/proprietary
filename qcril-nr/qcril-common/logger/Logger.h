/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include <memory>
#include <map>

extern "C" {
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>
}

// For MDM targets use RLOGV from mocks directory
#ifdef RIL_FOR_MDM_LE
#include <utils/Log2.h>
#endif

#ifndef QCRIL_NOTUSED
#define QCRIL_NOTUSED(x) ((void)(x))
#endif

namespace qti {
namespace ril {
namespace logger {

static inline long my_gettid() {
    return syscall(SYS_gettid);
}

const char *qcril_get_thread_name();

#define QCRIL_LOG_FMT "[%s: %d] [%s(%ld,%ld)] %s: "
#define QCRIL_LOG_DEFAULT_ARGS \
    basename((char *)__BASE_FILE__), \
    __LINE__, \
    ::qti::ril::logger::qcril_get_thread_name(), \
    (long)getpid(), \
    (long)::qti::ril::logger::my_gettid(), \
    __func__

#define QCRIL_HAL_LOG_VERBOSE(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_VERBOSE, TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define   QCRIL_HAL_LOG_DEBUG(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_DEBUG  , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define    QCRIL_HAL_LOG_INFO(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_INFO   , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define    QCRIL_HAL_LOG_WARN(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_WARN   , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define    QCRIL_HAL_LOG_HIGH(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_HIGH   , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define   QCRIL_HAL_LOG_FATAL(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_FATAL  , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)
#define   QCRIL_HAL_LOG_ADDITIONAL(fmt, ...) ::qti::ril::logger::Logger::log( \
        ::qti::ril::logger::LVL_ADDITIONAL  , TAG, QCRIL_LOG_FMT fmt, QCRIL_LOG_DEFAULT_ARGS, ##__VA_ARGS__)

#define   QCRIL_HAL_LOG_ENTER(fmt, ...) QCRIL_HAL_LOG_VERBOSE("> %s: " fmt, __func__, ##__VA_ARGS__)
#define   QCRIL_HAL_LOG_LEAVE(fmt, ...) QCRIL_HAL_LOG_VERBOSE("< %s: " fmt, __func__, ##__VA_ARGS__)

#ifdef DEBUG_LOGGER_STDERR
#define DEBUG_LOGGER(fmt, ...) ({\
    if (::qti::ril::logger::Logger::debugLogger) { \
    fprintf(stderr, "        LOGGER_DEBUG %s: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
    } \
})
#else
#define DEBUG_LOGGER RLOGV
#endif

enum LogLevel {
    LVL_ESSENTIAL,
    LVL_VERBOSE,
    LVL_DEBUG,
    LVL_INFO,
    LVL_WARN,
    LVL_HIGH,
    LVL_FATAL,
    LVL_ADDITIONAL,
    LVL_MAX /* Nothing will be logged if this is selected as level */
};

class LoggerImpl: public std::enable_shared_from_this<LoggerImpl> {
    public:
    class Param;

    virtual void vlog(LogLevel lvl, const char *tag, const char *msg) = 0;
    LoggerImpl(const std::string &name);
    virtual ~LoggerImpl() {}
    void setDebugLogger(bool debug) { debugLogger = debug; }
    bool getDebugLogger() { return debugLogger; }

    protected:
    int registerParam(const std::string &name,
                      const std::string &description);
    bool        debugLogger;

    private:
    std::string name;
};

class LoggerFactory {
    public:
    virtual std::shared_ptr<LoggerImpl> getLoggerImpl() = 0;
    static LoggerFactory *getLoggerFactory(const std::string &name);
    static std::shared_ptr<LoggerImpl> getLogger(const std::string &name);
    virtual ~LoggerFactory();

    protected:
    LoggerFactory(const std::string &name);
    static std::map< std::string, LoggerFactory * > &loggerFactories();

    private:
    LoggerFactory(const LoggerFactory &);
    LoggerFactory(const LoggerFactory &&) = delete;
    LoggerFactory(LoggerFactory &) = delete;
    LoggerFactory(LoggerFactory &&) = delete;
    LoggerFactory &operator= (const LoggerFactory &) = delete;
    LoggerFactory &operator= (LoggerFactory &&) = delete;
    std::string name;
};

class StdoutLogger;
class StdoutLoggerFactory: virtual public LoggerFactory {
    public:
    StdoutLoggerFactory(): LoggerFactory("stdout") {};
    virtual std::shared_ptr<LoggerImpl> getLoggerImpl();
    virtual ~StdoutLoggerFactory() {}

    protected:

    private:
    static StdoutLogger theLogger;
};

class StdoutLogger: virtual public LoggerImpl {
    friend class StdoutLoggerFactory;
    public:
    virtual void vlog(LogLevel lvl, const char *tag, const char *msg);
    StdoutLogger();
};

class Logger {
    friend class LoggerFactory;
    public:
    static void log(LogLevel lvl, const char *tag, const char *fmt, ...);
    static void setParams(const std::string &name, ...);
    static void setPrefix(const std::string &pref) { prefix = pref; }
    static bool setDefault(const std::string &name);
    static void setDebugLogger(bool debug) { debugLogger = debug; }
    static bool getDebugLogger() { return debugLogger; }
    static void setAdditionalLogOn(bool on) { isAdditionalLogOn = on; }
    static bool debugLogger;
    static bool isAdditionalLogOn;
    Logger() { init(); }

    private:
    static std::shared_ptr<LoggerImpl> defaultLogger;
    thread_local static char log_buf[1024];
    static std::string prefix;
    void init();
};
} // logger
} // ril
} // qti

#endif
