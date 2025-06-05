/**
 * Copyright (c) 2011-2016, 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 *Copyright (c) 2012 Qualcomm Atheros, Inc.
 *All Rights Reserved.
 *Qualcomm Atheros Confidential and Proprietary.
**/

/**
@file
@brief Sensor Interface Framework Daemon

This file creates slim daemon that will interface with sensor clients and
Sensor Provider (DSPS or Android API). It routes sensor data to SLIM CORE
using slim IPC mechanism.

@ingroup slim_APSS
*/

/* Platform library headers */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
#if defined(__ANDROID__)
    #include <sys/capability.h>
    #include <cutils/android_filesystem_config.h>
#else
    #include <cutils/sys/capability.h>
#endif
#include <sys/prctl.h>
#if !defined(CAP_BLOCK_SUSPEND)
#define CAP_BLOCK_SUSPEND (4 + 32)
#endif /* !defined(CAP_BLOCK_SUSPEND) */
#endif
#include <sys/prctl.h>

/* Utility headers */
#include <loc_cfg.h>
#include <comdef.h>
#include <diag_lsm.h>

/* SLIM daemon */
#include <SlimDaemonManager.h>
#include <SlimProviderConf.h>
#ifdef FEATURE_LOCTECH_SLIM_QMILOC
#include <QLClientListener.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_NDK
#include <SlimNDKProvider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_DSPS
#include <SlimSensor1Provider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_VNS
#include <VNSProvider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL
#include <SlimSensorHalProvider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
#include <SlimSensorHalDaemonProvider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_REMOTE_PROVIDER
#include <SlimRemoteProvider.h>
#endif

#ifdef FEATURE_LOCTECH_SLIM_INT_CLIENT
#include <SlimIntClient.h>
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_PLAYBACK
#include <SlimSensorPlayBackProvider.h>
#endif

/* SLIM headers */
#include <slim_task.h>
#include <qmislim_task_api.h>
#include <qmislim_qmi_dm_log_api.h>

#include <loc_pla.h>
#include <gps_extended_c.h>
#include <mq_client/mq_client.h>
#include <base_util/postcard.h>
#include <DataItemId.h>
#include "shutdown_timer.h"
#include <pthread.h>
#include <sys/prctl.h>

//! @addtogroup slim_APSS
//! @{

#undef SLIM_LOG_LOCAL_MODULE
//! @brief Log messages as MAIN module
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_MAIN
using namespace qc_loc_fw;

namespace slim
{
const SlimDaemonManager::ProviderInfoCfg SlimDaemonManager::c_zProviderConfigs[] =
{
  {SLIM_PROVIDER_QMISLIM_MPSS, "qmi-slim-mp", "Modem interface over QMI-SLIM"},
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
  {SLIM_PROVIDER_QMISLIM_TEST, "qmi-slim-test", "Test interface over QMI-SLIM"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_DSPS
  {SLIM_PROVIDER_SENSOR_PROTOBUF, "SensorProtobuf", "Sensor interface over Protobuf"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_NDK
  {SLIM_PROVIDER_NDK, "ndk", "Android interface over Android Sensor API"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_VNW
  {SLIM_PROVIDER_VNW, "vnw", "Vehicle Network (STUB) provider"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL
  {SLIM_PROVIDER_SENSOR_HAL, "SensorHal", "Sensor Interface for Sensor HAL provider"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  {SLIM_PROVIDER_SENSOR_HAL_DAEMON, "SensorHalDaemon", "Sensor Interface for Sensor\
    HAL Daemon provider"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_PLAYBACK
  {SLIM_PROVIDER_SENSOR_PLAYBACK, "SensorPlayback", "Sensor Interface for Sensor\
    Playback provider"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_VNS
  {SLIM_PROVIDER_VNS, "vns", "Vehicle Network Service provider"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_REMOTE_PROVIDER
  {SLIM_PROVIDER_RP, "remoteProvider", "Remote sensor interface provider"},
#endif

};
const SlimDaemonManager::ClientInfoCfg SlimDaemonManager::c_zClientConfigs[] =
{
  {SLIM_CLIENT_QMISLIM_MP, "qmi-slim-mp", "Modem client over QMI-SLIM"},
  {SLIM_CLIENT_QMISLIM_AP, "qmi-slim-ap", "Local client over QMI-SLIM"},
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
  {SLIM_CLIENT_QMISLIM_TEST, "qmi-slim-test", "Test client over QMI-SLIM"},
#endif
#ifdef FEATURE_LOCTECH_SLIM_QMILOC
  {SLIM_CLIENT_MPSS, "qmi-loc", "Modem client over QMI-LOC"},
#endif
};

/** Izat mappings configuration */
const SlimDaemonManager::IzatInfoCfg SlimDaemonManager::c_zIzatModeConfigs[] =
{
  {eIZAT_MODE_DISABLED, "disabled", "Feature is disabled"},
  {eIZAT_MODE_BASIC, "basic", "Feature is enabled in basic mode"},
  {eIZAT_MODE_PREMIUM, "premium", "Feature is enabled in premium mode"},
};

/**
 * Sets up signal mask and initializes service facilities.
 */
SlimDaemonManager::SlimDaemonManager() :
    m_eProviderSetting(SLIM_PROVIDER_SETTING_NONE),
    m_qSensorUsage(0),
    m_uEnableQmiLocInterface(FALSE),
    m_uSensorPlayBackUsageCfg(FALSE),
#if  defined(FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON) || defined(FEATURE_LOCTECH_SLIM_SENSOR_HAL)
    m_uSensorHalDeamonProviderEnabled(FALSE),
#endif
    m_qQSClientMask(0),
    m_qProviderMask(0),
    m_pzQLClientListener(0),
    m_eBatchMode(eBATCH_MODE_DEFAULT),
    m_ePipMode(eIZAT_MODE_DISABLED),
    m_launchTriggerMask(0),
#ifndef FEATURE_LOCTECH_SLIM_NDK
    m_eSapMode(eIZAT_MODE_DISABLED)
#else
    m_eSapMode(eIZAT_MODE_DISABLED),
    m_eNdkTimeSource(NDKProvider::eTIME_SOURCE_UNKNOWN)
#endif
{
}
/**
 * Restores signal mask and terminates services.
 */
SlimDaemonManager::~SlimDaemonManager()
{
  slim_MainThreadDeinit();
}
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
/**
 * @brief Fork child process and exit parent.
 */
#if defined(__ANDROID__)
void SlimDaemonManager::forkChild()
{
  SLIM_LOGD("Creating child process");

  signal(SIGCHLD, SIG_IGN);
  pid_t qPid = fork();
  if (qPid < 0)
  {
    SLIM_LOGE("Failed to fork child process: %d %s",
        errno, strerror(errno));
    exit(1);
  }
  else if (qPid > 0)
  {
    SLIM_LOGD("Created process: %ld", (long)qPid);
    exit(0);
  }

  // restore previous action
  signal(SIGCHLD, SIG_DFL);
  SLIM_LOGD("Creating new process group");
  qPid = setsid();
  if (-1 == qPid)
  {
    SLIM_LOGE("Failed to create new process group: %d %s",
        errno, strerror(errno));
    exit(1);
  }
  SLIM_LOGD("Changed process group: %ld", (long)qPid);

  int lFd = open("/dev/null", O_RDWR);
  if (lFd >= 0)
  {
    dup2(lFd, STDIN_FILENO);
    dup2(lFd, STDOUT_FILENO);
    dup2(lFd, STDERR_FILENO);
    if (lFd > 2)
    {
      close(lFd);
    }
  }
}
#endif /* __ANDROID__ */

/**
 * @brief Configures process capabilities.
 *
 * Process starts with root privileges and configures capabilities and
 * permissions before execution.
 *
 * @retval true on success
 * @retval false on failure
 */
bool SlimDaemonManager::setupCapabilities()
{
#if !defined(__ANDROID__)
    // For LE, use the groupd id values defined in base.inc
    #define AID_ROOT    0
    #define AID_GPS     1021
    #define AID_SYSTEM  1000
    #define AID_QCOM_DIAG 53 // AID_QCOM_DIAG not available in LE. So use diag
    #define AID_SENSORS 3011
    #define AID_INPUT   19
    #define AID_PLUGDEV   46
    //AID_NET_RAW not available in LE.
#endif

  //! Required process group UIDs
  static const gid_t zNewGroups[] =
  {
      AID_GPS,            // Primary group; required for QMI-LOC and QMI-SLIM
      AID_SYSTEM,         // Other service access
#ifdef AID_QCOM_DIAG
      AID_QCOM_DIAG,      // Required for DIAG interface
#endif
#ifdef AID_SENSORS
      AID_SENSORS,        // Required for SensorProtobuf interface
#endif
#ifdef AID_PLUGDEV
      AID_PLUGDEV,        // Required for Sensor interface in LE
#endif
#if !defined(__ANDROID__)
      AID_INPUT,          // Required for buffer data read
#endif
#ifdef AID_NET_RAW
      AID_NET_RAW,        // Registration of QMI-SLIM service
#endif
  };
  struct __user_cap_header_struct zCapHeader;
  struct __user_cap_data_struct zCapData[2];
  uint64_t tCapabilitiesAll;
  uint32_t qCapabilitiesLow;
  uint32_t qCapabilitiesHigh;

  if (getuid() != AID_ROOT)
  {
    SLIM_LOGD("Process started with non-root privileges");
    return true;
  }

  /* Setting GID and groups */
  if (setgroups(ARR_SIZE(zNewGroups), zNewGroups))
  {
    SLIM_LOGE("setgroups failed %s", strerror(errno));
    goto err;
  }
  if (setgid(zNewGroups[0]) < 0)
  {
    SLIM_LOGE("setgid failed %s", strerror(errno));
    goto err;
  }

  /* CAPs needed
     CAP_BLOCK_SUSPEND : required for Wakelock
     CAP_SETUID        : required for dropping to GPS
   */
  tCapabilitiesAll = 0;
  tCapabilitiesAll |= ((uint64_t)1) << CAP_BLOCK_SUSPEND;
  tCapabilitiesAll |= ((uint64_t)1) << CAP_SETUID;
#if !defined(__ANDROID__)
  // This is needed only in case of LE. Due to non-avail of AID_NET_RAW.
  tCapabilitiesAll |= ((uint64_t)1) << CAP_NET_BIND_SERVICE;
#endif
  qCapabilitiesLow = (uint32_t)tCapabilitiesAll;
  qCapabilitiesHigh = (uint32_t)(tCapabilitiesAll >> 32);

  memset(&zCapHeader, 0, sizeof(zCapHeader));
  memset(&zCapData, 0, sizeof(zCapData));
  zCapHeader.version = _LINUX_CAPABILITY_VERSION_3;
  zCapHeader.pid = getpid();

  zCapData[0].inheritable = 0;
  zCapData[1].inheritable = 0;
  zCapData[0].permitted = qCapabilitiesLow;
  zCapData[0].effective = qCapabilitiesLow;
  zCapData[1].permitted = qCapabilitiesHigh;
  zCapData[1].effective = qCapabilitiesHigh;

  //set required capabilities
  if (capset(&zCapHeader, zCapData) < 0)
  {
    SLIM_LOGE("capset error: %s", strerror(errno));
    goto err;
  }

  //capabilities should remain after dropping to GPS user
  if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) != 0)
  {
    SLIM_LOGE("PR_SET_KEEPCAPS error: %s", strerror(errno));
    goto err;
  }

  //now drop privileges to become GPS user
  if (setuid(AID_GPS) < 0)
  {
    SLIM_LOGE("setuid error: %s", strerror(errno));
    goto err;
  }
  //set effective capabilties
  if (capset(&zCapHeader, zCapData) < 0)
  {
    SLIM_LOGE("capset error: %s", strerror(errno));
    goto err;
  }

  //Enable dumps after setuid
  if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) != 0)
  {
    SLIM_LOGE("PR_SET_DUMPABLE error: %s", strerror(errno));
  }

  goto report;

report:
  return true;
err:
  return false;
}

/**
 * @brief Prints process capabilities to log.
 */
void SlimDaemonManager::printCapabilities()
{
  struct __user_cap_header_struct zCapHeader;
  struct __user_cap_data_struct zCapData[2];
  gid_t lGroups[32];
  int lNGroups;

  memset(&zCapHeader, 0, sizeof(zCapHeader));
  memset(&zCapData, 0, sizeof(zCapData));
  zCapHeader.version = _LINUX_CAPABILITY_VERSION_3;
  zCapHeader.pid = getpid();

  lNGroups = getgroups(ARR_SIZE(lGroups), lGroups);
  if (lNGroups < 0)
  {
    SLIM_LOGE("getgroups error: %s", strerror(errno));
  }
  else
  {
    for (int i = 0; i < lNGroups; ++i)
    {
      SLIM_LOGD("group[%d]=%ld", i, (long )lGroups[i]);
    }
  }

  if (capget(&zCapHeader, zCapData) < 0)
  {
    SLIM_LOGE("capget failed: %s", strerror(errno));
  }
  else
  {
    SLIM_LOGD("capabilities: effective=%08" PRIx32 ".%08" PRIx32
              "; permitted=%08" PRIx32 ".%08" PRIx32,
             zCapData[1].effective, zCapData[0].effective,
             zCapData[1].permitted, zCapData[0].permitted);
  }
}
#endif /* FEATURE_LOCTECH_SLIM_DEBUG */

/**
 * @brief Load SAP and GPS configuration files
 */
void SlimDaemonManager::loadConfiguration()
{
  uint32_t qSensorProviderCfg = SLIM_PROVIDER_SETTING_NONE;
  uint32_t qSensorUsageCfg = 0;
  uint32_t qNdkProviderTimeSourceCfg = eAPP_CFG_NDK_TIMESRC_UNKNOWN;
  uint32_t qSensorPlayBackUsageCfg = 0;
  uint32_t qSensorDiagLevelCfg = 0;
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  uint32_t qslimShdProvierUsageCfg = 0;
#endif

  loc_param_s_type z_sapParameterTable[] =
  {
    { "SENSOR_PROVIDER", &qSensorProviderCfg, NULL, 'n' },
    { "SENSOR_USAGE", &qSensorUsageCfg, NULL, 'n' },
    { "NDK_PROVIDER_TIME_SOURCE", &qNdkProviderTimeSourceCfg, NULL, 'n' },
    { "NATIVE_RECORD_FILE", &qSensorPlayBackUsageCfg, NULL, 'n' },
    { "SENSOR_DIAG_LEVEL", &qSensorDiagLevelCfg, NULL, 'n' },
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
    { "LOCTECH_SLIM_ENABLE_SENSORHALDAEMON_PROVIDER", &qslimShdProvierUsageCfg, NULL, 'n' },
#endif
  };
  /* Read logging configuration and sensor provider */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
  UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);

  /* If a bad sensor provider is given then revert to the default. */
  switch (qSensorProviderCfg)
  {
  case eAPP_CFG_PROVIDER_SENSOR_PROTOBUF:
    m_eProviderSetting = SLIM_PROVIDER_SETTING_SSC;
    break;
  case eAPP_CFG_PROVIDER_NDK:
    m_eProviderSetting = SLIM_PROVIDER_SETTING_NATIVE;
    break;
  default:
    m_eProviderSetting = SLIM_PROVIDER_SETTING_NONE;
    break;
  }

  m_qSensorUsage = qSensorUsageCfg;

  /* If a bad Sensor PlayBack Usage is given then revert to the default. */
  switch (qSensorPlayBackUsageCfg)
  {
  case 0:
    SLIM_LOGD(" qSensorPlayBackUsageCfg is 0");
    m_uSensorPlayBackUsageCfg = FALSE;
    break;
  case 1:
    SLIM_LOGD(" qSensorPlayBackUsageCfg is 1");
    m_uSensorPlayBackUsageCfg = TRUE;
    break;
  default:
    SLIM_LOGD(" qSensorPlayBackUsageCfg is default");
    m_uSensorPlayBackUsageCfg = FALSE;
    break;
  }

#ifdef FEATURE_LOCTECH_SLIM_NDK
  switch (qNdkProviderTimeSourceCfg)
  {
  case eAPP_CFG_NDK_TIMESRC_UNKNOWN:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_UNKNOWN;
    break;
  case eAPP_CFG_NDK_TIMESRC_BOOTIME:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_BOOTTIME;
    break;
  case eAPP_CFG_NDK_TIMESRC_ELAPSED:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_ELAPSED;
    break;
  case eAPP_CFG_NDK_TIMESRC_MONOTONIC:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_MONOTONIC;
    break;
  case eAPP_CFG_NDK_TIMESRC_REALTIME:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_REALTIME;
    break;
  default:
    m_eNdkTimeSource = NDKProvider::eTIME_SOURCE_UNKNOWN;
    break;
  }
#endif

#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  /* If a incorrect Sensor provider choice is provided then revert to the default. */
  switch (qslimShdProvierUsageCfg)
  {
  case 0:
    SLIM_LOGD(" SensorHalDeamonProviderEnabled is 0");
    m_uSensorHalDeamonProviderEnabled = FALSE;
    break;
  case 1:
    SLIM_LOGD(" SensorHalDeamonProviderEnabled is 1");
    m_uSensorHalDeamonProviderEnabled = TRUE;
    break;
  default:
    SLIM_LOGD(" SensorHalDeamonProviderEnabled is default");
    m_uSensorHalDeamonProviderEnabled = FALSE;
    break;
  }
#endif

  slimSetSensorDiagLevel(qSensorDiagLevelCfg);
}

/**
 * @brief Handles main application event loop.
 *
 * Main application event loop initializes SLIM facilities, external
 * interfaces and processes SLIM core and application events.
 *
 * The following events are handled:
 * - SLIM task messages
 * - User signals (including Ctrl-C handling)
 * - Timer signals
 * - QMI CSI select results
 */
void SlimDaemonManager::run()
{
  boolean uRunEventLoop = TRUE;
  SLIM_LOGD("Initializing SLIM Task; Timesync: %" PRIu64,
      (uint64_t)slim_TimeTickGetMilliseconds());

  SLIM_LOGD("Provider: QMI-SLIM MP %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_QMISLIM_MPSS) ? "On" : "Off");
  SLIM_LOGD("Provider: QMI-SLIM Test %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_QMISLIM_TEST) ? "On" : "Off");
  SLIM_LOGD("Provider: NDK %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_NDK) ? "On" : "Off");
  SLIM_LOGD("Provider: SensorProtobuf %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_SENSOR_PROTOBUF) ? "On" : "Off");
  SLIM_LOGD("Provider: VNW %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_VNW) ? "On" : "Off");
  SLIM_LOGD("Provider: SensorHal %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_SENSOR_HAL) ? "On" : "Off");
  SLIM_LOGD("Provider: SensorHalDaemon %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_SENSOR_HAL_DAEMON) ? "On" : "Off");
  SLIM_LOGD("Provider: VNS %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_VNS) ? "On" : "Off");
  SLIM_LOGD("Provider: remoteProv %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_RP) ? "On" : "Off");
  SLIM_LOGD("Provider: SensorPlayback %s",
      SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_SENSOR_PLAYBACK) ? "On" : "Off");
  SLIM_LOGD("Client: QMI-SLIM AP %s",
      SLIM_MASK_IS_SET(m_qQSClientMask, eSLIM_QS_NODE_AP) ? "On" : "Off");
  SLIM_LOGD("Client: QMI-SLIM MP %s",
      SLIM_MASK_IS_SET(m_qQSClientMask, eSLIM_QS_NODE_MP) ? "On" : "Off");
  SLIM_LOGD("Client: QMI-SLIM Test %s",
      SLIM_MASK_IS_SET(m_qQSClientMask, eSLIM_QS_NODE_TEST) ? "On" : "Off");

  SLIM_LOGD("Client: QMI-LOC %s", m_uEnableQmiLocInterface ? "On" : "Off");

  SLIM_LOGD("Sensor provider: setting=%" PRIu32 " usage=%" PRIu32,
      (uint32_t)m_eProviderSetting,
      (uint32_t)m_qSensorUsage);

  SLIM_LOGD("Batch mode: %d", m_eBatchMode);
  SLIM_LOGD("IZAT features: pip=%d sap=%d", m_ePipMode, m_eSapMode);
#ifdef FEATURE_LOCTECH_SLIM_NDK
  SLIM_LOGD("NDK Provider Time Source: %d", m_eNdkTimeSource);
#endif
  /* Set provider configuration before SLIM initialization. */
  slim_ProviderConfSetConfig(m_eProviderSetting, m_qProviderMask);

  /* Obtain provider instances if present */
#ifdef FEATURE_LOCTECH_SLIM_NDK
  android::sp<NDKProvider> pzNDKProvider = NDKProvider::getInstance();
#endif
#ifdef FEATURE_LOCTECH_SLIM_DSPS
  SensorProtobufProvider *pzS1Provider = NULL;
  if (SLIM_MASK_IS_SET(m_qProviderMask, SLIM_PROVIDER_SENSOR_PROTOBUF)) {
    pzS1Provider = SensorProtobufProvider::getInstance();
  }
#endif

  switch (m_eBatchMode)
  {
  case eBATCH_MODE_ON:
  {
#ifdef FEATURE_LOCTECH_SLIM_NDK
    if (0 != pzNDKProvider.get())
    {
      // Batch support in NDK is removed
      // pzNDKProvider->setBatchMode(true);
    }
#endif
#ifdef FEATURE_LOCTECH_SLIM_DSPS
    if (0 != pzS1Provider)
    {
      pzS1Provider->setBatchMode(true);
    }
#endif
    break;
  }
  case eBATCH_MODE_OFF:
  {
    #ifdef FEATURE_LOCTECH_SLIM_NDK
    // Batch support in NDK is removed
    // NDKProvider::getInstance()->setBatchMode(false);
#endif
#ifdef FEATURE_LOCTECH_SLIM_DSPS
    if (0 != pzS1Provider)
    {
      pzS1Provider->setBatchMode(false);
    }
#endif
    break;
  }
  case eBATCH_MODE_DEFAULT:
    break;
  }
#ifdef FEATURE_LOCTECH_SLIM_NDK
  if (0 != pzNDKProvider.get())
  {
    pzNDKProvider->setTimeSource(m_eNdkTimeSource);
  }
#endif
  slim_TaskInit();
  slim_TaskStart();
  /* Configure SLIM client interfaces */
  if (initClientInterfaces() < 0)
  {
    SLIM_LOGF("Unable to initialize SLIM_DAEMON!");
    exit(1);
  }

  /* Exit slim_daemon when parent process  dies. */
  if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0)
  {
    SLIM_LOGV("%s:%d]: prctl failed! (%s)\n", __func__, __LINE__, strerror(errno));
  }

  /* Infinite loop until cancellation request */
  while (uRunEventLoop)
  {
    slim_IpcMsgT *pz_Msg;

    while (NULL == (pz_Msg = slim_IpcReceive()));

    SLIM_LOGV("Got IPC: %" PRIu32 " from %" PRIu32,
        (uint32_t)pz_Msg->q_MsgId,
        (uint32_t)pz_Msg->q_SrcThreadId);

    switch (pz_Msg->q_MsgId)
    {
    case M_IPC_MSG_SIGINT:
      SLIM_LOGD("Processing stop request");
      uRunEventLoop = FALSE;
      break;

    case M_IPC_MSG_TIMER:
      os_TimerExpiryType z_TimerMsg;
      slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data,
          pz_Msg->q_Size);

      if (IPC_SLIM_TIMER_ID_MIN <= z_TimerMsg.q_TimerId &&
          IPC_SLIM_TIMER_ID_MAX >= z_TimerMsg.q_TimerId)
      {
        slim_TaskTimerHandler(z_TimerMsg.q_TimerId, z_TimerMsg.q_Param);
      }
      else
      {
        SLIM_LOGW("Unknown timer event: timer=%" PRIu32 " param=%" PRIu32,
            (uint32_t)z_TimerMsg.q_TimerId,
            (uint32_t)z_TimerMsg.q_Param);
      }
      break;

    default:
      if (IPC_SLIM_TO_TASK_MIN <= pz_Msg->q_MsgId &&
          IPC_SLIM_TO_TASK_MAX >= pz_Msg->q_MsgId)
      {
        slim_TaskIpcHandler(pz_Msg);
      }
      else
      {
        SLIM_LOGW("Unknown message: message=%" PRIu32 " from=%" PRIu32,
            (uint32_t)pz_Msg->q_MsgId,
            (uint32_t)pz_Msg->q_SrcThreadId);
      }
      break;
    }

    slim_IpcDelete(pz_Msg);
  }
}

/**
 * @brief Print application usage help.
 * @param[in] pz_Name Application name.
 */
void SlimDaemonManager::printHelp(const char *pz_Name)
{
  printf("SLIM Daemon.\n");
  printf("usage: %s [--help | -h] [--log-console] [--log-diag]\n"
         "          [--enable-provider <provider>] [--disable-provider <provider>]\n"
         "          [--enable-client <client> --disable-client <client>]\n"
         "          [--pip <mode>] [--sap <mode>]",
         (NULL == pz_Name) ? "slim_ap_daemon" : pz_Name);
  printf("\nLogging options:\n");
  printf("     --log-console                 Use console for logging output.\n");
  printf("     --log-diag                    Use DIAG for logging output.\n");
  printf("\nInterface options:\n");
  printf("     --enable-provider <provider>  Force provider enabled.\n");
  printf("     --disable-provider <provider> Force provider disabled.\n");
  printf("     --enable-client <client>      Enable client support.\n");
  printf("     --disable-client <client>     Disable client support.\n");
  printf("\nFeature options:\n");
  printf("     --pip <mode>                  Control PIP mode.\n");
  printf("     --sap <mode>                  Control SAP mode.\n");
  printf("\nOther:\n");
  printf("     --fork                        Fork child daemon.\n");
  printf("     --batch                       Enable batch mode.\n");
  printf("     --no-batch                    Disable batch mode.\n");
  printf(" -h, --help                        Print this help and exit.\n");
  printf("\nProviders:\n");
  for (size_t i = 0; i < ARR_SIZE(c_zProviderConfigs); ++i)
  {
    printf("  %-23s %s\n", c_zProviderConfigs[i].pzArgName,
        c_zProviderConfigs[i].pzHelp);
  }
  printf("\nClients:\n");
  for (size_t i = 0; i < ARR_SIZE(c_zClientConfigs); ++i)
  {
    printf("  %-23s %s\n", c_zClientConfigs[i].pzArgName,
        c_zClientConfigs[i].pzHelp);
  }
  printf("\nIZat feature modes:\n");
  for (size_t i = 0; i < ARR_SIZE(c_zIzatModeConfigs); ++i)
  {
    printf("  %-23s %s\n", c_zIzatModeConfigs[i].pzArgName,
        c_zIzatModeConfigs[i].pzHelp);
  }

  printf("\nProvides SLIM services on application processor for interfacing with other\n"
         "processors and applications.\n");
}

/**
 * @brief Convert provider argument into enumeration.
 * @param[in]  pzName       Command line parameter.
 * @param[out] eProviderOut Conversion result
 * @retval true  on success
 * @retval false on failure
 */
bool SlimDaemonManager::parseProviderArg
(
  const char *pzName,
  slim_ProviderEnumType &eProviderOut
)
{
  if (NULL != pzName)
  {
    for (size_t i = 0; i < ARR_SIZE(c_zProviderConfigs); ++i)
    {
      if (0 == strcasecmp(c_zProviderConfigs[i].pzArgName, pzName))
      {
        eProviderOut = c_zProviderConfigs[i].eProvider;
        return true;
      }
    }
  }

  return false;
}

/**
 * @brief Convert client argument into enumeration.
 * @param[in]  pzName     Command line parameter.
 * @param[out] eClientOut Conversion result
 * @retval true  on success
 * @retval false on failure
 */
bool SlimDaemonManager::parseClientArg
(
  const char *pzName,
  slim_ClientEnumType &eClientOut
)
{
  if (NULL != pzName)
  {
    for (size_t i = 0; i < ARR_SIZE(c_zClientConfigs); ++i)
    {
      if (0 == strcasecmp(c_zClientConfigs[i].pzArgName, pzName))
      {
        eClientOut = c_zClientConfigs[i].eClient;
        return true;
      }
    }
  }

  return false;
}

/**
 * @brief Convert IZAT mode argument into enumeration.
 * @param[in]  pzName   Command line parameter.
 * @param[out] eModeOut Conversion result
 * @retval true  on success
 * @retval false on failure
 */
bool SlimDaemonManager::parseIzatModeArg
(
  const char *pzName,
  IZatModeEnum &eModeOut
)
{
  if (NULL != pzName)
  {
    for (size_t i = 0; i < ARR_SIZE(c_zIzatModeConfigs); ++i)
    {
      if (0 == strcasecmp(c_zIzatModeConfigs[i].pzArgName, pzName))
      {
        eModeOut = c_zIzatModeConfigs[i].eMode;
        return true;
      }
    }
  }

  return false;
}

/**
 * @brief Initialize application object
 *
 * Function performs command-line argument parsing and configuration loading.
 *
 * @param argc Count of command line arguments
 * @param argv Command line arguments as string array
 */
void SlimDaemonManager::initialize
(
  int argc,
  char *argv[]
)
{
  // Set logging tag and load default configuration.
  slim_LogSetAndroidTag("slim_daemon");
  int lPrintUsageHelp = 0;
  /**< Print application usage */
  int lLogConsole = 0;
  /**< Log to console; 1 - enable */
  int lLogDiag = 0;
  /**< Log to DIAG F3; 1 - enable
   * DIAG logging includes:
   * - F3 Message logging
   * - DIAG packet logging.
   *
   * Unfortunately, DIAG buffers are quickly getting full on AP when there are
   * lots of messages processed. This leads to DIAG write errors and loss of
   * data. Because of that DIAG logging is disabled by default.
   */
  int lBatch = m_eBatchMode;
  /**< Use NDK batch mode */

  uint32_t qCmdClientConfig = 0;
  /**< Mask for supported client interfaces */
  uint32_t qCmdProviderConfig = 0;
  /**< Mask for supported providers */

  #ifdef FEATURE_LOCTECH_SLIM_DEBUG
  bool uFork = false;
  /**< Fork child process */
  #endif /* FEATURE_LOCTECH_SLIM_DEBUG */

  /*
  Set default client mask:
  - QMI-SLIM MP Client
  - QMI-SLIM AP Client (PIP/ULP:libslimclient)
  - QMI-LOC MP (if built)
  */
  qCmdClientConfig =  0;
  SLIM_MASK_SET(qCmdClientConfig, SLIM_CLIENT_QMISLIM_MP);
  SLIM_MASK_SET(qCmdClientConfig, SLIM_CLIENT_QMISLIM_AP);
#ifdef FEATURE_LOCTECH_SLIM_QMILOC
  SLIM_MASK_SET(qCmdClientConfig, SLIM_CLIENT_MPSS);
#endif
  /*
  Set default provider mask:
  - NDK Provider if built
  - QMI-SLIM MP
  - SENSOR_PROTOBUF if built
  - VNW if built
  - Remote Prov if built,
  - SensorHALDaemon Prov if built,
  - if SensorHALDaemon in conf file, Disable sensorHAL provider, both are mutual exclusive
  - if Remote Prov enabled in conf file, Disable Other Sensors
  */
  qCmdProviderConfig =  0;

  /* This is run time config option to disable providers*/
  uint32_t isRemoteProviderEnabled = 0;  //RP Disabled by default
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  uint32_t isSensorHalDeamonProviderEnabled = 0; //SensorHALDaemon is Disabled by default
#endif
  loc_param_s_type z_slimParameterTable[] =
  {
    { "LOCTECH_SLIM_ENABLE_REMOTE_PROVIDER", &isRemoteProviderEnabled, NULL, 'n' },
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
    { "LOCTECH_SLIM_ENABLE_SENSORHALDAEMON_PROVIDER", &isSensorHalDeamonProviderEnabled,
       NULL, 'n' },
#endif
  };
  /* Read logging configuration and sensor provider */
  UTIL_READ_CONF(LOC_PATH_SLIM_CONF, z_slimParameterTable);

#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  SLIM_LOGD("isSensorHALDaemon:%d ", isSensorHalDeamonProviderEnabled);
#endif
  loadConfiguration();

  if (m_uSensorPlayBackUsageCfg)
  {
    SLIM_LOGD("m_uSensorPlayBackUsageCfg is TRUE");
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_PLAYBACK
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_SENSOR_PLAYBACK);
    SLIM_LOGD("Set SLIM_PROVIDER_SENSOR_PLAYBACK, qCmdProviderConfig = %d", qCmdProviderConfig);
#endif
  }
  else
  {
    SLIM_LOGD("m_uSensorPlayBackUsageCfg is FALSE");
#ifdef FEATURE_LOCTECH_SLIM_NDK
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_NDK);
#endif
#ifdef FEATURE_LOCTECH_SLIM_DSPS
  SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_SENSOR_PROTOBUF);
#endif
  }

  SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_QMISLIM_MPSS);
#ifdef FEATURE_LOCTECH_SLIM_VNW
  if (!isRemoteProviderEnabled)
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_VNW);
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL
  if (!isRemoteProviderEnabled && !m_uSensorPlayBackUsageCfg && !m_uSensorHalDeamonProviderEnabled)
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_SENSOR_HAL);
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL_DAEMON
  if (!isRemoteProviderEnabled && !m_uSensorPlayBackUsageCfg && m_uSensorHalDeamonProviderEnabled)
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_SENSOR_HAL_DAEMON);
#endif
#ifdef FEATURE_LOCTECH_SLIM_VNS
  SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_VNS);
#endif
#ifdef FEATURE_LOCTECH_SLIM_REMOTE_PROVIDER
  if (isRemoteProviderEnabled)
    SLIM_MASK_SET(qCmdProviderConfig, SLIM_PROVIDER_RP);
#endif

  const struct option zLongOptions[] = {
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
     {"help", 0, &lPrintUsageHelp, 1},
     {"log-console", 0, &lLogConsole, 1},
     {"log-diag", 0, &lLogDiag, 1},
     {"disable-client", 1, 0, eAPP_CMD_LONG_DISABLE_CLIENT},
     {"enable-client", 1, 0, eAPP_CMD_LONG_ENABLE_CLIENT},
     {"disable-provider", 1, 0, eAPP_CMD_LONG_DISABLE_PROVIDER},
     {"enable-provider", 1, 0, eAPP_CMD_LONG_ENABLE_PROVIDER},
     {"batch", 0, &lBatch, eBATCH_MODE_ON},
     {"no-batch", 0, &lBatch, eBATCH_MODE_OFF},
     {"fork", 0, 0, eAPP_CMD_LONG_FORK},
#endif /* FEATURE_LOCTECH_SLIM_DEBUG */
     {"pip", 1, 0, eAPP_CMD_LONG_PIP},
     {"sap", 1, 0, eAPP_CMD_LONG_SAP},
     {LOC_FEATURE_LAUNCH_TRIGGER_MASK, 1, 0, eAPP_CMD_LONG_TRIGGER_MASK},
     {0, 0, 0, 0},
  };
  /**< Supported command-line options */
  while (1)
  {
    int lOptionIndex = 0;
    int lCh = 0;
    #ifdef FEATURE_LOCTECH_SLIM_DEBUG
        slim_ClientEnumType eClient = (slim_ClientEnumType)0;
        slim_ProviderEnumType eProvider = SLIM_PROVIDER_NONE;
    #endif /* FEATURE_LOCTECH_SLIM_DEBUG */
    IZatModeEnum eIzatMode;

    lCh = getopt_long(argc, argv, "h", zLongOptions, &lOptionIndex);
    if (-1 == lCh)
      break;

    switch (lCh)
    {
    case eAPP_CMD_DEFAULT:
      /* Long option parsed */
      break;
#ifdef FEATURE_LOCTECH_SLIM_DEBUG
    case eAPP_CMD_LONG_DISABLE_CLIENT:
      if (parseClientArg(optarg, eClient))
      {
        SLIM_MASK_CLEAR(qCmdClientConfig, eClient);
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse client argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse client argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_ENABLE_CLIENT:
      if (parseClientArg(optarg, eClient))
      {
        SLIM_MASK_SET(qCmdClientConfig, eClient);
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse client argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse client argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_DISABLE_PROVIDER:
      if (parseProviderArg(optarg, eProvider))
      {
        SLIM_MASK_CLEAR(qCmdProviderConfig, eProvider);
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse provider argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse provider argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_ENABLE_PROVIDER:
      if (parseProviderArg(optarg, eProvider))
      {
        SLIM_MASK_SET(qCmdProviderConfig, eProvider);
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse provider argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse provider argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_FORK:
      uFork = true;
      break;
    case eAPP_CMD_SHORT_HELP:
      lPrintUsageHelp = 1;
      break;
#endif /* FEATURE_LOCTECH_SLIM_DEBUG */
    case eAPP_CMD_LONG_PIP:
      if (parseIzatModeArg(optarg, eIzatMode))
      {
        m_ePipMode = eIzatMode;
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse mode argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse mode argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_SAP:
      if (parseIzatModeArg(optarg, eIzatMode))
      {
        m_eSapMode = eIzatMode;
      }
      else
      {
        SLIM_LOGF("ERROR: can't parse mode argument: %s", optarg);
        fprintf(stderr, "ERROR: can't parse mode argument: %s\n", optarg);
        exit(1);
      }
      break;
    case eAPP_CMD_LONG_TRIGGER_MASK:
      {
        m_launchTriggerMask = std::stoul(optarg, nullptr, 16);
        SLIM_LOGD("launch Trigger mask: %d", m_launchTriggerMask);
      }
      break;
    default:
      SLIM_LOGF("ERROR: getopt returned character code 0x%04x\n", lCh);
      fprintf(stderr, "ERROR: getopt returned character code 0x%04x\n", lCh);
      exit(1);
    }
  }

/* This is not allowing log control in LE PL's */
#if __ANDROID__
  slim_LogSetConsoleLevel(lLogConsole ? SLIM_LOG_VERBOSE : SLIM_LOG_NONE);
#endif
  m_uEnableQmiLocInterface = SLIM_MASK_IS_SET(qCmdClientConfig, SLIM_CLIENT_MPSS);

  m_qQSClientMask = 0;
  if (0 != SLIM_MASK_IS_SET(qCmdClientConfig, SLIM_CLIENT_QMISLIM_MP))
  {
    SLIM_MASK_SET(m_qQSClientMask, eSLIM_QS_NODE_MP);
  }
  if (0 != SLIM_MASK_IS_SET(qCmdClientConfig, SLIM_CLIENT_QMISLIM_AP))
  {
    SLIM_MASK_SET(m_qQSClientMask, eSLIM_QS_NODE_AP);
  }
  if (0 != SLIM_MASK_IS_SET(qCmdClientConfig, SLIM_CLIENT_QMISLIM_TEST))
  {
    SLIM_MASK_SET(m_qQSClientMask, eSLIM_QS_NODE_TEST);
  }

  m_qProviderMask = qCmdProviderConfig;
  m_eBatchMode = (BatchModeEnum)lBatch;

  if (0 != lPrintUsageHelp)
  {
    printHelp(argc > 0 ? argv[0] : NULL);
    exit(0);
  }

  // Initialize thread IPC and logging.
  if (!slim_MainThreadInit("main", THREAD_ID_MAIN))
  {
    SLIM_LOGF("Failed to initialize thread");
    exit(1);
  }
  if (0 != slimGetSensorDiagLevel() || 0 != lLogDiag) {
    // Initialize DIAG stack.
    if (!Diag_LSM_Init(0))
    {
      SLIM_LOGF("Failed to initialize DIAG.");
    }
  }
  if (0 != lLogDiag)
  {
    // Initialize DIAG levels.
    slim_LogSetDiagLevel(SLIM_LOG_DEBUG);
    slim_QSLogConfigure(TRUE, TRUE, TRUE, TRUE);
  }
  else
  {
    // Disable all DIAG output: logging and packet logging.
    slim_LogSetDiagLevel(SLIM_LOG_NONE);
    slim_LogSetEnabled(FALSE);
    slim_QSLogConfigure(FALSE, FALSE, FALSE, FALSE);
  }

#ifdef FEATURE_LOCTECH_SLIM_DEBUG
#if defined(__ANDROID__)
  if (uFork)
  {
    forkChild();
  }
#endif /* __ANDROID__ */
  if (!setupCapabilities())
  {
    SLIM_LOGE("Failed to setup process capabilities");
    exit(1);
  }
  printCapabilities();
#endif /* FEATURE_LOCTECH_SLIM_DEBUG */
}

/**
 * @brief Initializes external client interfaces.
 *
 * The method initializes additional external client interfaces in addition
 * to QMI-SLIM service interface.
 *
 * Currently the following interfaces are supported:
 * - QMI-LOC interface for legacy GNSS builds.
 * - IPC-SLIM interface for AP clients (including PIP).
 * - QMI-SLIM AP/MP/Test interfaces with QMI-SLIM providers.
 *
 * @retval 0 on success
 */
int SlimDaemonManager::initClientInterfaces(void)
{
  slimServiceProviderEnumT eDefaultProvider = eSLIM_SERVICE_PROVIDER_DEFAULT;

  switch (m_eProviderSetting)
  {
  case SLIM_PROVIDER_SETTING_NONE:
    eDefaultProvider = eSLIM_SERVICE_PROVIDER_DEFAULT;
    break;
  case SLIM_PROVIDER_SETTING_SSC:
    eDefaultProvider = eSLIM_SERVICE_PROVIDER_SSC;
    break;
  case SLIM_PROVIDER_SETTING_NATIVE:
    eDefaultProvider = eSLIM_SERVICE_PROVIDER_NATIVE;
    break;
  default:
    break;
  }

  /* Configure QMI-SLIM client support. */
  slim_QSQmiSvcTask_SetPeerMask(&slimz_QSQmiSvcTask, m_qQSClientMask);

  //Initialize client listeners
#ifdef FEATURE_LOCTECH_SLIM_QMILOC
  if (m_uEnableQmiLocInterface)
  {
    m_pzQLClientListener = QLClientListener::get(eDefaultProvider);
  }
#endif

#ifdef FEATURE_LOCTECH_SLIM_INT_CLIENT
  m_pzSlimIntClient = SlimIntClient::getInstance();
#endif

  return 0;
}

// SLIM MQ client
class SlimMqClient: public MessageQueueServiceCallback {
private:
    std::string mqClientName;
    pthread_t mqClientThread;
    SlimDaemonManager* mMgr;
    ProcessShutdownTimer mShutdownTimer;
    MessageQueueClient* mqClient;
    bool mTrackingStarted;
public:
    inline SlimMqClient(SlimDaemonManager* mgr) : mTrackingStarted(false), mqClientName("SLIM"),
        mqClientThread(NULL), mMgr(mgr), mqClient(NULL),
        mShutdownTimer("SLIM", [this]() { this->shutdownProcess(); }) {
            mSlimMqClient = this;
            mShutdownTimer.setLaunchTriggerMask(mMgr->getLaunchTriggerMask());
            SLIM_LOGD("Process shutdown is %s ",
                    (mMgr->getLaunchTriggerMask() > 0) ? "Enabled" : "Disable");
    }
    inline ~SlimMqClient() {
        if (mqClient) { delete mqClient; }
        mSlimMqClient = NULL;
        mMgr = NULL;
    }
    int newMsg(InMemoryStream* new_buffer) override;
    int subOrUnSubOsStatusUpdate(bool unsubscribe = false);
    int registerMqClient();
    void startSlimOsAgentUpdateListener();
    void shutdownProcess();
    static void* slimMsgHandlerThread(void* arg);
    static SlimMqClient* mSlimMqClient;
};

SlimMqClient* SlimMqClient::mSlimMqClient = NULL;

int SlimMqClient::newMsg(InMemoryStream* new_buffer) {
    const char* info = NULL;
    SLIM_LOGD("Incoming Postcard to %s", mqClientName.c_str());
    if (NULL == new_buffer) {
        SLIM_LOGE("Null card buffer");
        return -1;
    }
    InPostcard* card = InPostcard::createInstance();
    card->init(new_buffer->getBuffer(), new_buffer->getSize());
    card->getString("INFO", &info);
    if (NULL != info && (0 == strncmp("OS-STATUS-UPDATE", info, sizeof("OS-STATUS-UPDATE")))) {
        InPostcard* innerCard = NULL;
        SLIM_LOGD("INFO: %s", info);
        card->getCard("TRACKING_STARTED", &innerCard);
        if (innerCard != NULL) {
            innerCard->getBool("IS_TRACKING_STARTED", mTrackingStarted);
            SLIM_LOGD("Tracking started: %s", mTrackingStarted ? "true" : "false");
            if (!mTrackingStarted) {
                if (mShutdownTimer.isActive()) {
                    mShutdownTimer.restart();
                } else {
                    mShutdownTimer.start();
                }
            } else {
                if (mShutdownTimer.isActive()) {mShutdownTimer.stop();}
            }
            delete innerCard;
        }
    }
    delete new_buffer;
    delete card;
    return 0;
}

int SlimMqClient::subOrUnSubOsStatusUpdate(bool unsubscribe) {
    SLIM_LOGD("SLIM > SUBSCRIBE(TRACKING_STARTED_DATA_ITEM_ID)");
    int ret = -1;
    // Send Subscribe message
    OutPostcard * card = OutPostcard::createInstance();
    if (NULL != card && NULL != mqClient) {
        card->init();
        card->addString("TO", "OS-Agent");
        card->addString("FROM", mqClientName.c_str());
        if (unsubscribe) {
          card->addString("REQ", "UNSUBSCRIBE");
        } else {
          card->addString("REQ", "SUBSCRIBE");
        }
        int32_t dataitems[] = {TRACKING_STARTED_DATA_ITEM_ID};
        card->addArrayInt32("DATA-ITEMS", 1, dataitems);
        card->finalize();
        ret = mqClient->send(card->getEncodedBuffer());
        delete card;
    }
    return ret;
}

int SlimMqClient:: registerMqClient() {
    SLIM_LOGD("SLIM > REGISTER");
    int ret = -1;
    // Send Register message
    OutPostcard * card = OutPostcard::createInstance();
    if (NULL != card && NULL != mqClient) {
      card->init();
      card->addString("TO", "SERVER");
      card->addString("FROM", mqClientName.c_str());
      card->addString("REQ", "REGISTER");
      card->finalize();
      ret = mqClient->send(card->getEncodedBuffer());
      delete card;
    }
    return ret;
}

void* SlimMqClient::slimMsgHandlerThread(void* arg)
{
    SLIM_LOGD("Slim MQ Client thread");
    int ret = -1;
    do {
        mSlimMqClient->mqClient = MessageQueueClient::createInstance();
        if (NULL == mSlimMqClient->mqClient) {
            SLIM_LOGE("Failed to create MQ client instance.");
            break;
        }
        ret = mSlimMqClient->mqClient->setServerNameDup(MQ_SERVER_SOCKET_NAME);
        if (0 != ret) {
            SLIM_LOGE("setServerNameDup failed, ret %d", ret);
            break;
        }
        ret = mSlimMqClient->mqClient->connect();
        if (0 != ret) {
            SLIM_LOGE("MQ Connect failed, ret %d", ret);
            break;
        }
        SLIM_LOGD("MQ connect successful");
        if (0 != (ret = mSlimMqClient->registerMqClient())) {
            SLIM_LOGE("Failed to register, ret %d", ret);
            break;
        }
        if (0 != (ret = mSlimMqClient->subOrUnSubOsStatusUpdate())) {
            SLIM_LOGE("Failed to subscribe, ret %d", ret);
            break;
        }
        ret = mSlimMqClient->mqClient->run_block(mSlimMqClient);
        if (0 != ret) {
            SLIM_LOGE("Failed to run_block, ret %d", ret);
        }
    } while (0);
    return arg;
}

void SlimMqClient::startSlimOsAgentUpdateListener() {
    // Create a thread to handle MQ client messages
    int result = pthread_create(&mqClientThread, NULL, &(SlimMqClient::slimMsgHandlerThread), NULL);
    if (result != 0) {
        SLIM_LOGE("pthread_create failed. Error = %d", result);
    }
    result = pthread_setname_np(mqClientThread, (char *) "SlimMqClient");
    if (result != 0) {
        SLIM_LOGE("pthread_setname_np failed, err %d", result);
    }
}

void SlimMqClient::shutdownProcess() {
    SLIM_LOGD("Shuttin down process");
    //unsubscribe dataitems here
    subOrUnSubOsStatusUpdate(true);
    //shutdown the reciver of SlimMqClient
    mqClient->shutdown();
    //send msg to zApplication to exit from run function
    if (!slim_IpcSendEmpty(THREAD_ID_MAIN, M_IPC_MSG_SIGINT)) {
      SLIM_LOGE("Error sending M_IPC_MSG_SIGINT to main thread!!!");
    }
}

} /* namespace slim */

/**
@brief Application entry function

Startup of daemon process. Simply initializes daemon and starts up data
thread to process message queue requests. Then waits until data thread
exits (which should be never).

@param[in] argc Argument count.
@param[in] argv Argument string array.

@return Application exit code.
*/
int main(int argc, char *argv[])
{
  slim::SlimDaemonManager zApplication;
  zApplication.initialize(argc, argv);
  slim::SlimMqClient mqClient(&zApplication);
  if (0 != zApplication.getLaunchTriggerMask()) {
      mqClient.startSlimOsAgentUpdateListener();
  }
  zApplication.run();
  SLIM_LOGD("bye bye");
  return 0;
}

//! @}
