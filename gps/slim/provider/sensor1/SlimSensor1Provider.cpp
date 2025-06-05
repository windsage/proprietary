/*
   Copyright (c) 2011, 2014-2015, 2020-2023 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   Copyright (c) 2012 Qualcomm Atheros, Inc. All Rights Reserved.
   Qualcomm Atheros Confidential and Proprietary.
   */
/**
  @file
  @brief GNSS / Sensor Interface Framework Support

  This file defines the implementation for sensor provider interface using
  Qualcomm Technologies Inc. proprietary DSPS SensorProtobuf API.

  @ingroup slim_SensorProtobufProvider
  */
#include <loc_cfg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "slim_client_types.h"
#include <iostream>
#include <cinttypes>
#include <unistd.h>
#include "SlimSscConnection.h"
#include "SlimSscUtils.h"
#include <vector>
#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"
#include "sns_client.pb.h"
#include "SlimSensor1Provider.h"

using namespace std;
//! @addtogroup slim_SensorProtobufProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* This unit logs as SensorProtobuf component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module constant for SensorProtobuf Provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_S1


//! @brief Supported services by SensorProtobuf
#define SLIM_SENSOR_PROTOBUF_SERVICE_MASK ((1 <<  eSLIM_SERVICE_SENSOR_ACCEL) |\
    (1 <<  eSLIM_SERVICE_SENSOR_ACCEL_TEMP) |\
    (1 <<  eSLIM_SERVICE_SENSOR_GYRO) |\
    (1 <<  eSLIM_SERVICE_SENSOR_GYRO_TEMP))

#define BATCH_PERIOD 100000  // 100 ms

#define SENSORPROTOBUF_SUCCESS 1
#define SENSORPROTOBUF_EWOULDBLOCK 2
using namespace slim;

Mutex SensorProtobufProvider::s_zInstanceMutex;
SensorProtobufProvider *SensorProtobufProvider::s_pzInstance;

/* Static function declarations */

/*=============================================================================
  Static Data
  ===========================================================================*/


static ssc_connection *sscConnection[SPB_MAX_SERV]={NULL};
static sensor_uid suidList[SPB_MAX_SERV];
static char sensorStrList[SPB_MAX_SERV][50] = {};

static int64_t qtimerOffsetFromBootTime = 0;
static slimAvailableServiceMaskT availableServiceMask;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
#define SENSORPROTOBUF_APSS_QTIMER_GAP_MAX_ITERATION    (1)
#define SENSORPROTOBUF_APSS_QTIMER_GAP_THRESHOLD_NS     (50000)
#define SENSORPROTOBUF_NS_IN_ONE_SECOND                 (1000000000LL)

uint64_t sensorProtobufQitmerToNs(uint64_t inQtimer)
{
  uint64_t qTimerCnt = 0, qTimerFreq = 0, qTimerSec = 0, qTimerNanoSec = 0;
  uint64_t outTime = 0;

  qTimerCnt = inQtimer;
  qTimerFreq = slim_GetQTimerFrequency();
  qTimerSec = (qTimerCnt / qTimerFreq);
  qTimerNanoSec = (qTimerCnt % qTimerFreq);
  qTimerNanoSec *= SENSORPROTOBUF_NS_IN_ONE_SECOND;
  qTimerNanoSec /= qTimerFreq;
  outTime = (qTimerSec * SENSORPROTOBUF_NS_IN_ONE_SECOND) + qTimerNanoSec;
  return outTime;
}

uint64_t sensorProtobufGetQtimerTimeNs(uint64_t &qTimerTicks)
{
  uint64_t qTimerCnt = 0;
  uint64_t outTime = 0;

  qTimerTicks = qTimerCnt = slim_GetQTimerTickCount();
  outTime = sensorProtobufQitmerToNs(qTimerCnt);
  return outTime;
}

uint64_t sensorProtobufGetClockBootTimeNs()
{
  uint64_t outTime = 0;
  struct timespec  bootTime;

  clock_gettime(CLOCK_BOOTTIME, &bootTime);
  outTime = (bootTime.tv_sec * SENSORPROTOBUF_NS_IN_ONE_SECOND) + bootTime.tv_nsec;
  return outTime;
}

int64_t sensorProtobufCalQtimerOffset()
{
  uint64_t  bootTimeNs = 0;
  uint64_t  bootTime_newNs = 0;
  uint64_t  qtimerTimeNs = 0;
  uint64_t  qtimerTicks = 0;
  int64_t   offsetNs = 0;
  int       ctr = 0;

  // Find delta between qtimer and system time
  do {
    bootTimeNs = sensorProtobufGetClockBootTimeNs();
    qtimerTimeNs = sensorProtobufGetQtimerTimeNs(qtimerTicks);
    bootTime_newNs = sensorProtobufGetClockBootTimeNs();

    offsetNs = (int64_t) (qtimerTimeNs - bootTimeNs);
    ctr++;
  } while (((bootTime_newNs - bootTimeNs) > SENSORPROTOBUF_APSS_QTIMER_GAP_THRESHOLD_NS) &&
      (ctr < SENSORPROTOBUF_APSS_QTIMER_GAP_MAX_ITERATION));

  qtimerOffsetFromBootTime = offsetNs;
  SLIM_LOGV("Current qtimerTicks= %" PRId64 " Ticks, qtimerTimeNs = % "
      PRId64 " Ns, QtimerOffsetFromBootTime = % " PRId64 " Ns",
      qtimerTicks, qtimerTimeNs, qtimerOffsetFromBootTime);
  SLIM_LOGV("bootTime = % " PRId64 " Ns, bootTime_new = % " PRId64" Ns, Tries:%d",
      bootTimeNs, bootTime_newNs, ctr);
  return qtimerOffsetFromBootTime;
}

inline uint64_t ConvertQtimerToAPSS(int64_t qtimerTicks)
{
  // Convert input Qtimer to APSS time
  uint64_t qtimerNs = sensorProtobufQitmerToNs(qtimerTicks);
  uint64_t apssTime = qtimerNs - qtimerOffsetFromBootTime;

  SLIM_LOGV("SampleQtimerTicks = % " PRId64 " SampleQtimerNS = % " PRId64
    " SampleAPSStimeNS = % " PRId64 " OffsetNs %" PRId64, qtimerTicks, qtimerNs, apssTime,
    qtimerOffsetFromBootTime);
  return apssTime;
}

void slim::SensorProtobufProvider::SensorProtobufProcessSensorData(const uint8_t *data,
                                               size_t size)

{
  sns_client_event_msg pb_event_msg;
  sensor_uid suid;
  int servType = eSLIM_SENSOR_TYPE_NONE;
  pb_event_msg.ParseFromArray(data, size);
  SLIM_LOGV("Received QMI indication with length %zu events_size:%d", size,
                    pb_event_msg.events_size());
  const sns_std_suid &stdSuid = pb_event_msg.suid();
  suid.low = stdSuid.suid_low();
  suid.high = stdSuid.suid_high();

  if (suidList[0].high == suid.high && suidList[0].low == suid.low) {
    servType = eSLIM_SENSOR_TYPE_ACCEL;
    m_zAccSensorData.sensorType = (slimSensorTypeEnumT)eSLIM_SENSOR_TYPE_ACCEL;
    m_zAccSensorData.flags = 0;
    m_zAccSensorData.provider = eSLIM_SERVICE_PROVIDER_SSC;
    m_zAccSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  } else if (suidList[1].high == suid.high && suidList[1].low == suid.low) {
    servType = eSLIM_SENSOR_TYPE_GYRO;
    m_zGyroSensorData.sensorType = (slimSensorTypeEnumT)eSLIM_SENSOR_TYPE_GYRO;
    m_zGyroSensorData.flags = 0;
    m_zGyroSensorData.provider = eSLIM_SERVICE_PROVIDER_SSC;
    m_zGyroSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  } else if (suidList[2].high == suid.high && suidList[2].low == suid.low) {
    servType = eSLIM_SENSOR_TYPE_ACCEL_TEMP;
    m_zTempSensorData.sensorType = (slimSensorTypeEnumT)eSLIM_SENSOR_TYPE_ACCEL_TEMP;
    m_zTempSensorData.flags = 0;
    m_zTempSensorData.provider = eSLIM_SERVICE_PROVIDER_SSC;
    m_zTempSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  }

  for (int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    SLIM_LOGV("Event[%i] msg_id=%i, ts=%llu", i, pb_event.msg_id(),
        (unsigned long long)pb_event.timestamp());

    if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id())
    {
      sns_std_error_event error;
      error.ParseFromString(pb_event.payload());

      SLIM_LOGE("Received Error Event  %i from SUID %" PRIx64 "%" PRIx64 "",
          error.error(), suid.high, suid.low);
    }
    else if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_physical_config_event config;
      config.ParseFromString(pb_event.payload());

      SLIM_LOGV("Received Config Event with sample rate %f from SUID %" PRIx64 "%" PRIx64 "",
        config.sample_rate(), suid.high, suid.low);
    }
    else if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_event event;
      event.ParseFromString(pb_event.payload());
      if (SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != event.status())
      {
        if (eSLIM_SENSOR_TYPE_ACCEL_TEMP == servType)
        {
          SLIM_LOGV("Received Sensor Event[%d] from SUID %" PRIx64 "%" PRIx64
                " servType:%d Received sample <%f, 0, 0> QTimer:%" PRId64
                " Ticks status:%d", i, suid.high, suid.low,
                servType, event.data(0), pb_event.timestamp(), event.status());

           uint64_t sensorTimeNsec = ConvertQtimerToAPSS(pb_event.timestamp());

          if (0 == m_zTempSensorData.samples_len) {
            // Set timebase for first sample
            m_zTempSensorData.timeBase = sensorTimeNsec / 1000000; //NanoSec TO milliSec
            m_zTempSensorData.samples[m_zTempSensorData.samples_len].sampleTimeOffsetMs = 0;
          } else {
            // Calculate offset for second sample onwards
            m_zTempSensorData.samples[m_zTempSensorData.samples_len].sampleTimeOffsetMs =
                                  (sensorTimeNsec / 1000000) - m_zTempSensorData.timeBase;
          }
          /* sub milli offset with Nsec resolution */
          m_zTempSensorData.samples[m_zTempSensorData.samples_len].sampleTimeOffsetSubMilliNs =
                                    sensorTimeNsec % 1000000;

          m_zTempSensorData.samples[m_zTempSensorData.samples_len].sample[0] = event.data(0);
          m_zTempSensorData.samples[m_zTempSensorData.samples_len].sample[1] = 0;
          m_zTempSensorData.samples[m_zTempSensorData.samples_len].sample[2] = 0;
          m_zTempSensorData.samples_len++;

          if (i + 1 >= pb_event_msg.events_size() ||
                 m_zTempSensorData.samples_len + 1 >= SLIM_SENSOR_MAX_SAMPLE_SETS) {
            routeIndication(eSLIM_SERVICE_SENSOR_ACCEL_TEMP, m_zTempSensorData);
            m_zTempSensorData.samples_len = 0;
          }
        }
        else if (eSLIM_SENSOR_TYPE_GYRO == servType)
        {
          SLIM_LOGV("Received Sensor Event[%d] from SUID %" PRIx64 "%" PRIx64
                " servType:%d Received sample <%f, %f, %f> QTimer:%" PRId64
                " Ticks status:%d", i, suid.high, suid.low, servType,
                event.data(0), event.data(1), event.data(2), pb_event.timestamp(), event.status());

          uint64_t sensorTimeNsec = ConvertQtimerToAPSS(pb_event.timestamp());

          if (0 == m_zGyroSensorData.samples_len) {
            // Set timebase for first sample
            m_zGyroSensorData.timeBase = sensorTimeNsec / 1000000; //NanoSec TO milliSec
            m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sampleTimeOffsetMs = 0;
          } else {
            // Calculate offset for second sample onwards
            m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sampleTimeOffsetMs =
                                  (sensorTimeNsec / 1000000) - m_zGyroSensorData.timeBase;
          }
          /* sub milli offset with Nsec resolution */
          m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sampleTimeOffsetSubMilliNs =
                                  sensorTimeNsec % 1000000;

          m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[0] = event.data(0);
          m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[1] = event.data(1);
          m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[2] = event.data(2);
          m_zGyroSensorData.samples_len++;

          if (i + 1 >= pb_event_msg.events_size() ||
                  m_zGyroSensorData.samples_len + 1 >= SLIM_SENSOR_MAX_SAMPLE_SETS) {
            routeIndication(eSLIM_SERVICE_SENSOR_GYRO, m_zGyroSensorData);
            m_zGyroSensorData.samples_len = 0;
          }
        }
        else if (eSLIM_SENSOR_TYPE_ACCEL == servType)
        {
          SLIM_LOGV("Received Sensor Event[%d] from SUID %" PRIx64 "%" PRIx64
                " servType:%d Received sample <%f, %f, %f> QTimer:%" PRId64
                " Ticks status:%d", i, suid.high, suid.low, servType,
                event.data(0), event.data(1), event.data(2), pb_event.timestamp(), event.status());

          uint64_t sensorTimeNsec = ConvertQtimerToAPSS(pb_event.timestamp());

          if (0 == m_zAccSensorData.samples_len) {
            // Set timebase for first sample
            m_zAccSensorData.timeBase = sensorTimeNsec / 1000000; //NanoSec TO milliSec
            m_zAccSensorData.samples[m_zAccSensorData.samples_len].sampleTimeOffsetMs = 0;
          } else {
            // Calculate offset for second sample onwards
            m_zAccSensorData.samples[m_zAccSensorData.samples_len].sampleTimeOffsetMs =
                                 (sensorTimeNsec / 1000000) - m_zAccSensorData.timeBase;
          }
          /* sub milli offset with Nsec resolution */
          m_zAccSensorData.samples[m_zAccSensorData.samples_len].sampleTimeOffsetSubMilliNs =
                                    sensorTimeNsec % 1000000;

          m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[0] = event.data(0);
          m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[1] = event.data(1);
          m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[2] = event.data(2);
          m_zAccSensorData.samples_len++;

          if (i + 1 >= pb_event_msg.events_size() ||
                m_zAccSensorData.samples_len + 1 >= SLIM_SENSOR_MAX_SAMPLE_SETS) {
            routeIndication(eSLIM_SERVICE_SENSOR_ACCEL, m_zAccSensorData);
            m_zAccSensorData.samples_len = 0;
          }
        }
        else
        {
          SLIM_LOGE("Received Sensor Event from SUID %" PRIx64 "%" PRIx64
                " Sensor Event %d --Not Handled !!!!!!", suid.high, suid.low, servType);
        }
      }
      else
      {
        SLIM_LOGE("Received Sensor Event from SUID %" PRIx64 "%" PRIx64
                " Sensor Event:%d status:%d --Not Handled !!!!!!", suid.high, suid.low,
                servType, event.status());
      }
    }
    else if (SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == pb_event.msg_id())
    {
      /* flush event received, check accel and route data to client */
      if (eSLIM_SENSOR_TYPE_ACCEL == servType || m_zAccSensorData.samples_len > 0) {
        routeIndication(eSLIM_SERVICE_SENSOR_ACCEL, m_zAccSensorData);
        m_zAccSensorData.samples_len = 0;
      }

      /* flush event received, check gyro and route data to client */
      if (eSLIM_SENSOR_TYPE_GYRO == servType || m_zGyroSensorData.samples_len > 0) {
        routeIndication(eSLIM_SERVICE_SENSOR_GYRO, m_zGyroSensorData);
        m_zGyroSensorData.samples_len = 0;
      }

      /* flush event received, check temp and route data to client */
      if (eSLIM_SENSOR_TYPE_ACCEL_TEMP == servType || m_zTempSensorData.samples_len > 0) {
        routeIndication(eSLIM_SERVICE_SENSOR_ACCEL_TEMP, m_zTempSensorData);
        m_zTempSensorData.samples_len = 0;
      }
    }
    else
    {
      /* no valid event received */
      SLIM_LOGE("Received Unknown Event from SUID %" PRIx64 "%" PRIx64 "",
          suid.high, suid.low);
    }
  } /*end of for loop */
  return;
}

/**
 * Event callback function, as registered with ssc_connection.
 */
static void sensorProtobufEvent_cb(const uint8_t *data, size_t size)
{
  ProtoBufEvent *pzProtoEvent = (ProtoBufEvent*) malloc(sizeof(ProtoBufEvent));
  SLIM_LOGV("Received QMI indication with length %zu", size);
  // Copy Data
  if (NULL != pzProtoEvent)
  {
    memset(pzProtoEvent, 0, sizeof(ProtoBufEvent));
    pzProtoEvent->data = (uint8_t*) malloc(size);
    if (NULL == pzProtoEvent->data)
    {
      SLIM_LOGE("Error memalloc Fail Protobuff Data !!!!");
      free(pzProtoEvent);
      return;
    }
    memset(pzProtoEvent->data, 0, size);
    slim_Memscpy(pzProtoEvent->data, size, data, size);
    pzProtoEvent->size = size;
    // Post it for processing in a thread
    if (!slim_IpcSendData(THREAD_ID_SENSOR_PROTOBUF, ePROVIDER_IPC_MSG_NEW_SENSORPROTOBUF_EVENT,
               &pzProtoEvent, size))
    {
      SLIM_LOGE("Error sending Protobuff Event IPC message to event processing loop !!!!");
    }
  }
  else
    SLIM_LOGE("Error memalloc Fail Protobuff Event !!!!");
}

/**
 * Send a SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG to SUID
 */
static void send_config_req(ssc_connection *conn, sensor_uid const *suid,
                                 uint32_t sampling_rate, bool uResampling)
{
  string pb_req_msg_encoded;
  string config_encoded;
  sns_client_request_msg pb_req_msg;
  sns_std_sensor_config config;

  SLIM_LOGD("Send config request with sample rate %i", sampling_rate);
  SLIM_LOGD(" SUID %" PRIx64 "%" PRIx64 " req Send", suid->high, suid->low);

  config.set_sample_rate(sampling_rate);
  config.SerializeToString(&config_encoded);

  if (true == uResampling )
  {
    SLIM_LOGD("resampler is used, set resampler config");
    // Requested resampled rate is fixed
    pb_req_msg.mutable_resampler_config()->set_rate_type(SNS_RESAMPLER_RATE_FIXED);
    pb_req_msg.mutable_resampler_config()->set_filter(true);
  }

  pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
  pb_req_msg.mutable_request()->set_payload(config_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->
    set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
    set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
  pb_req_msg.mutable_request()->mutable_batching()->
    set_batch_period(BATCH_PERIOD);
  pb_req_msg.SerializeToString(&pb_req_msg_encoded);
  conn->send_request(pb_req_msg_encoded);
}

/**
 * SUID callback as registered with suid_lookup.
 */
static void suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids)
{
  int srvCnt;
  static bool isFirstSUIDRec = false;

  if (false == isFirstSUIDRec)
  {
    loc_boot_kpi_marker("L - SSC SUID Event Received");
    isFirstSUIDRec = true;
  }

  SLIM_LOGD("Received SUID event with length %zu", suids.size());
  if (suids.size() > 0)
  {
    for (srvCnt = 0; srvCnt < SPB_MAX_SERV; srvCnt++)
    {
      if (0 == strcmp(sensorStrList[srvCnt], datatype.c_str()))
      {
        suidList[srvCnt] = suids.at(0);
        SLIM_LOGD("srvCnt:%d Received SUID %" PRIx64 "%" PRIx64 " for '%s'", srvCnt,
               suidList[srvCnt].high, suidList[srvCnt].low, datatype.c_str());

        /* update availableServiceMask based on sensorStrList */
        if (eSPB_SensorACCEL == srvCnt) {
          /* sensorStrList[eSPB_SensorACCEL] = accel */
          SLIM_MASK_SET(availableServiceMask, eSLIM_SERVICE_SENSOR_ACCEL);
        } else if (eSPB_SensorGYRO == srvCnt ) {
          /* sensorStrList[eSPB_SensorGYRO] = gear */
          SLIM_MASK_SET(availableServiceMask, eSLIM_SERVICE_SENSOR_GYRO);
        } else if (eSPB_SensorTEMP == srvCnt ) {
          /* sensorStrList[eSPB_SensorTEMP] = temp */
          SLIM_MASK_SET(availableServiceMask, eSLIM_SERVICE_SENSOR_ACCEL_TEMP);
          SLIM_MASK_SET(availableServiceMask, eSLIM_SERVICE_SENSOR_GYRO_TEMP);
        }
      }
    }
  }

  /* suids of all supported services are received, update route_config_change */
  if (SLIM_SENSOR_PROTOBUF_SERVICE_MASK == availableServiceMask) {
    /*IPC to runevent */
    SLIM_LOGV("All services suid_cb received, sending IPC !!!!");
    if (!slim_IpcSendEmpty(THREAD_ID_SENSOR_PROTOBUF, ePROVIDER_IPC_MSG_ROUTE_CONFIG_CHANGE))
    {
      SLIM_LOGE("Error sending route config change IPC message to event processing loop !!!!");
    }
  }

  return;
}

/**
 * @brief Thread main function
 *
 * @param[in] pData User data pointer.
 */
void SensorProtobufProvider::threadMain
(
 void *pData
 )
{
  SensorProtobufProvider *pzProvider = reinterpret_cast<SensorProtobufProvider*>(pData);
  slim_IpcStart(THREAD_ID_SENSOR_PROTOBUF);
  /* Service support is enabled */
  pzProvider->m_qServiceMask = 0;
  pzProvider->runEventLoop();
}

/**
  @brief Constructs object

  Function performs partial initialization of object state.
  */
SensorProtobufProvider::SensorProtobufProvider() :
    MultiplexingProvider(SLIM_PROVIDER_SENSOR_PROTOBUF, true, 0),
    m_zThread(),
    m_zMutex(),
    m_zGyroState(),
    m_zAccelState(),
    m_zGyroTempState(),
    m_zAccelTempState(),
    m_eOpenResult(SENSORPROTOBUF_SUCCESS),
    m_uBatchMode(false),
    mQMIConnectionPending(true)
{
  memset(sensorStrList, 0, sizeof(sensorStrList));
  memscpy(sensorStrList[eSPB_SensorACCEL], sizeof(sensorStrList[eSPB_SensorACCEL]),
          SPB_SENSOR_ACCEL_STR, sizeof(SPB_SENSOR_ACCEL_STR));
  memscpy(sensorStrList[eSPB_SensorGYRO], sizeof(sensorStrList[eSPB_SensorGYRO]),
          SPB_SENSOR_GYRO_STR, sizeof(SPB_SENSOR_GYRO_STR));
  memscpy(sensorStrList[eSPB_SensorTEMP], sizeof(sensorStrList[eSPB_SensorTEMP]),
          SPB_SENSOR_TEMPERATURE_STR, sizeof(SPB_SENSOR_TEMPERATURE_STR));
  lookUpSuid = NULL;
}

/**
  @brief Releases object data
  */
SensorProtobufProvider::~SensorProtobufProvider()
{
}

/* Call back for SSC Connection Loss indication */
static void ssc_conn_error_cb(ssc_error_type e)
{
  // Reset connection if error
  SLIM_LOGE("Connection Error %d", e);
  // Post it for processing in a thread
  if (!slim_IpcSendEmpty(THREAD_ID_SENSOR_PROTOBUF, ePROVIDER_IPC_MSG_ID_ENABLE_SERVICE))
  {
    SLIM_LOGE("Error sending Re-Enable Service IPC message to processing loop !!!!");
  }
  else
  {
    SLIM_LOGV("Sent Re-Enable Service IPC message to processing loop ");
  }
}

/* In case of SSR, Restore ServiceStates */
void SensorProtobufProvider::restoreServiceStates()
{

  for (int eService = 0; eService <= eSLIM_SERVICE_LAST; eService++)
  {
    if (SLIM_MASK_IS_SET(m_qServiceMask, eService))
    {
      switch (eService)
      {
        case eSLIM_SERVICE_SENSOR_ACCEL:
          if (m_zAccelState.running)
          {
            m_zAccelState.running = false;
            doUpdateSensorStatus(
              true,
              m_zAccelState.reporting_rate,
              m_zAccelState.batching_rate,
              eSLIM_SERVICE_SENSOR_ACCEL
            );
          }
        break;
        case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
          if (m_zAccelTempState.running)
          {
            m_zAccelTempState.running = false;
            doUpdateSensorStatus(
              true,
              m_zAccelTempState.reporting_rate,
              m_zAccelTempState.batching_rate,
              eSLIM_SERVICE_SENSOR_ACCEL_TEMP
            );
          }
        break;
        case eSLIM_SERVICE_SENSOR_GYRO:
          if (m_zGyroState.running)
          {
            m_zGyroState.running = false;
            doUpdateSensorStatus(
              true,
              m_zGyroState.reporting_rate,
              m_zGyroState.batching_rate,
              eSLIM_SERVICE_SENSOR_GYRO
            );
          }
        break;
        default:
          SLIM_LOGE("UnHandled Service:%d", eService);
        break;
      }
    }
  }
}

/**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using SensorProtobuf Buffering
  API.

  @todo The sampling frequency is a mere suggestion to the sensor protobuf daemon.
  Sensor 1 will stream at the requested minimum sampling frequency requested
  by all AP clients combined. So we do see cases where buffering API does not
  help and we get single sensor data for every indication. In that case should
  SLIM AP do the batching?

  @param[in] uEnable       Flag that indicates if the service shall be enabled
  or disabled.
  @param[in] wReportRateHz Requested reporting rate in Hz.
  @param[in] wSampleCount  Requested sample count in one batch.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
slimErrorEnumT SensorProtobufProvider::doUpdateSensorStatus
(
 bool uEnable,
 uint16_t wReportRateHz,
 uint16_t wSampleCount,
 slimServiceEnumT eService
 )
{
  SensorState* sensor_state;
  uint32_t sampling_rate;
  bool uMessagesSent = true;
  ssc_connection *connection;
  const char* sensor_str;
  sensor_uid suid;
  int srvCnt;

  MutexLock _l(m_zMutex);

  sampling_rate = wSampleCount * wReportRateHz;
  switch (eService)
  {
    case eSLIM_SERVICE_SENSOR_ACCEL:
      srvCnt = 0;
      sensor_state = &m_zAccelState;
      sensor_str = sensorStrList[srvCnt];//"accel";
      suid = suidList[srvCnt];
      connection = sscConnection[srvCnt];
      m_acc100HzBatchCount = wSampleCount;
      break;

    case eSLIM_SERVICE_SENSOR_GYRO:
      srvCnt = 1;
      sensor_state = &m_zGyroState;
      sensor_str = sensorStrList[srvCnt];//"gyro";
      suid = suidList[srvCnt];
      connection = sscConnection[srvCnt];
      m_gyro100HzBatchCount = wSampleCount;
      break;

    case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      srvCnt = 2;
      sensor_state = &m_zAccelTempState;
      sensor_str = sensorStrList[srvCnt];//"Temp";
      suid = suidList[srvCnt];
      connection = sscConnection[srvCnt];
      m_temp100HzBatchCount = wSampleCount;
      break;

    default:
      SLIM_LOGE("Unknown Sensor Type %d", eService);
      return eSLIM_ERROR_BAD_ARGUMENT;
  }

  /* No sensor state to use. */
  if (0 == sensor_state)
  {
    SLIM_LOGE("No %s sensor state provided to start/stop", sensor_str);
    return eSLIM_ERROR_BAD_ARGUMENT;
  }

  /* No State Change */
  if (!uEnable && !sensor_state->running)
  {
    /* Sensors stay Off */
    return eSLIM_SUCCESS;
  }
  /* Sensors stay On but no change in sensor sampling rate */
  else if (uEnable &&
           sensor_state->running &&
           sampling_rate == (sensor_state->reporting_rate * sensor_state->batching_rate))
  {
    return eSLIM_SUCCESS;
  }

  if (uEnable)
  {
    /* Turn on sensor */
    if (!sensor_state->running)
    {
      SLIM_LOGD(
        "Turning on %s, reporting-rate %" PRIu16 " sample-count%" PRIu16 "sampling-rate %" PRIu32,
        sensor_str, wReportRateHz, wSampleCount, sampling_rate);
      connection = sscConnection[srvCnt] = new ssc_connection(sensorProtobufEvent_cb);
      connection->register_error_cb(ssc_conn_error_cb);
    }
    /* Sensor sampling rate change */
    else if (sensor_state->running)
    {
      SLIM_LOGD("Changing sampling rate on %s reporting", sensor_str);
    }

    if (NULL != connection)
    {
      send_config_req(connection, &suid, sampling_rate, true);
      uMessagesSent = true;
    }
    else
      SLIM_LOGE("NULL connection %s Sampling rate Change request...!!!", sensor_str);
  }
  /* Turn off sensor */
  else if (!uEnable && sensor_state->running)
  {
    SLIM_LOGD("Turning off %s reporting", sensor_str);
    if (NULL != connection)
    {
      delete connection;
      connection = NULL;
      uMessagesSent = true;
    }
    else
      SLIM_LOGE("NULL connection %s Close connection request...!!!", sensor_str);
  }

  if (uMessagesSent)
  {
    mQMIConnectionPending =  false;
    sensor_state->running = uEnable;
    sensor_state->batching_rate = wSampleCount;
    sensor_state->reporting_rate = wReportRateHz;
  }

  return eSLIM_SUCCESS;
}

/**
  @brief Handler for messages targeted for provider.

  Function handles messages sent via SLIM core to provider.

  @param[in] qMessageId      Message id.
  @param[in] qMsgPayloadSize Message size.
  @param[in] pMsgPayload     Pointer to the message payload.
  */
void SensorProtobufProvider::handleProviderMessage
(
 uint32_t qMessageId,
 uint32_t qMsgPayloadSize,
 const void *pMsgPayload
 )
{
  SLIM_LOGD("qMessageId: %d qMsgPayloadSize: %d ", qMessageId, qMsgPayloadSize);
  switch (qMessageId)
  {
    default:
      MultiplexingProvider::handleProviderMessage(qMessageId, qMsgPayloadSize, pMsgPayload);
      break;
  }
}
/**
  @brief Opens SensorProtobuf connection.

  Opens connection to SensorProtobuf API. The result can be one of the following:
  - Success
  - Partial success
  - Error

  @return true if result is success or partial success; false otherwise.
  */
bool SensorProtobufProvider::open()
{
  bool b_Success = false;
  int srvCnt=0;
  /* create new suid_lookup instance */
  lookUpSuid = new suid_lookup(suid_cb);

  /* Request All required SUIDs */
  for (srvCnt =0; srvCnt < SPB_MAX_SERV;srvCnt++)
  {
    SLIM_LOGD("SensorProtobuf lookup: %d srvCnt: %d ", m_eOpenResult, srvCnt);
    SLIM_LOGD("Requesting SUID for :%s", sensorStrList[srvCnt]);
    if (NULL != lookUpSuid) {
      lookUpSuid->request_suid(sensorStrList[srvCnt]);
    } else {
      SLIM_LOGE("NULL Error in requesting SUID for :%s", sensorStrList[srvCnt]);
    }
  }
  /* register timer with IPC */
  bool timerStart = slim_TimerStart(mSuidCbExpiryTimer,
                                    SLIMPB_SUID_CB_TIMEOUT_MSEC,
                                    eTIMER_SUID_CB_EXPIRY_TIMER);
  if (timerStart) {
    SLIM_LOGV("SUID CB timer started...");
  } else {
    SLIM_LOGE("SUID CB timer start fail...");
  }
  /* open result is not success since sensorprotobuf connection is pending */
  m_eOpenResult = SENSORPROTOBUF_EWOULDBLOCK;

  SLIM_LOGD("SensorProtobuf open: %d", m_eOpenResult);

  m_qServiceMask = 0;
  switch (m_eOpenResult)
  {
    case SENSORPROTOBUF_SUCCESS:
      SLIM_LOGD("SensorProtobuf connection opened successfully!");
      b_Success = true;
      m_qServiceMask = SLIM_SENSOR_PROTOBUF_SERVICE_MASK;
      break;

    case SENSORPROTOBUF_EWOULDBLOCK:
      SLIM_LOGE("Pending SensorProtobuf connection opening!");
      b_Success = true;
      break;

    default:
      SLIM_LOGE("Fall back to Android NDK as Sensor Core is not available!");
      break;
  }

  return b_Success;
}

/**
  @brief Closes SensorProtobuf connection.

  Closes connection to SensorProtobuf API.

  @return true if operation is successful; false otherwise.
  */
bool SensorProtobufProvider::close()
{
  SLIM_LOGD("close SensorProtobuf");
  m_qServiceMask = 0;
  m_eOpenResult = SENSORPROTOBUF_SUCCESS;
  memset(&m_zGyroState, 0, sizeof(m_zGyroState));
  memset(&m_zAccelState, 0, sizeof(m_zAccelState));
  memset(&m_zGyroTempState, 0, sizeof(m_zGyroTempState));
  memset(&m_zAccelTempState, 0, sizeof(m_zAccelTempState));
  return true;
}


/**
  @brief Initializes internal structures for sensor provider.

  The method prepares provider for operation.
  @return true if operation is successful; false otherwise.
  */
bool SensorProtobufProvider::init()
{
  SLIM_LOGD("Initializing SensorProtobuf");
  /* Retrive Batching configuration */
  setBatchingConfig();

  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "SensorProtobuf"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_SENSOR_PROTOBUF);
  mQtimerOffsetTimer = slim_TimerCreate(eTIMER_QTIMER_OFFSET_CAL_TIMER,
                  THREAD_ID_SENSOR_PROTOBUF);
  mQMIReconnectTimer = slim_TimerCreate(eTIMER_QTIMER_QMI_RECONNECT_TIMER,
                  THREAD_ID_SENSOR_PROTOBUF);
  mSuidCbExpiryTimer = slim_TimerCreate(eTIMER_SUID_CB_EXPIRY_TIMER,
                  THREAD_ID_SENSOR_PROTOBUF);

  if (slim_IpcSendEmpty(m_zThread.q_IpcId, (uint32_t)ePROVIDER_IPC_MSG_ID_REOPEN))
  {
    return true;
  }
  return false;
}

bool SensorProtobufProvider::startQtimerOffsetCalTimer()
{
  return slim_TimerStart(mQtimerOffsetTimer,
      SLIMPB_QTIMER_BOOTTIME_OFFSET_CAL_TIMER_MSEC,
      eTIMER_QTIMER_OFFSET_CAL_TIMER);
}

bool SensorProtobufProvider::startQMIReconnectTimer()
{
  return slim_TimerStart(mQMIReconnectTimer,
       SLIMPB_QMI_RECONNECT_TIMER_MSEC,
       eTIMER_QTIMER_QMI_RECONNECT_TIMER);
}

void SensorProtobufProvider::runEventLoop()
{
  SLIM_LOGD("Starting event loop");
  slim_TaskReadyAck();
  /* Start timer and initilize Qtimer offset */
  sensorProtobufCalQtimerOffset();
  startQtimerOffsetCalTimer();

  while (1)
  {
    slim_IpcMsgT *pz_Msg = NULL;
    while (NULL == (pz_Msg = slim_IpcReceive()));

    SLIM_LOGV("IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
        pz_Msg->q_MsgId,
        pz_Msg->q_SrcThreadId);
    switch (pz_Msg->q_MsgId)
    {
      case eIPC_MSG_TIMER:
        {
          os_TimerExpiryType z_TimerMsg;
          slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data,
              pz_Msg->q_Size);
          SLIM_LOGV("q_TimerId:%d\n", z_TimerMsg.q_TimerId);
          switch (z_TimerMsg.q_TimerId)
          {
          case eTIMER_QTIMER_OFFSET_CAL_TIMER:
            sensorProtobufCalQtimerOffset();
            startQtimerOffsetCalTimer();
          break;
          case eTIMER_QTIMER_QMI_RECONNECT_TIMER:
            if (true == mQMIConnectionPending)
            {
              SLIM_LOGV("Service Restore is being attempted by timer.");
              restoreServiceStates();
            }
            else
              SLIM_LOGV("Service Restore is completed no attempt required by timer.");
          break;
        // On timer expiry, Send out the updated available servicemask
        case eTIMER_SUID_CB_EXPIRY_TIMER:
          SLIM_LOGD("Sending route configuration change on timer expiry AvailableMask: %x\n",
            availableServiceMask);
          m_qServiceMask = availableServiceMask;
          /* delete suid_lookup instance */
          if (lookUpSuid) {
            delete lookUpSuid;
            lookUpSuid = NULL;
          }
          /* stop the timer and send route config change */
          slim_TimerStop(mSuidCbExpiryTimer);
          routeConfigurationChange(availableServiceMask);
          break;
        }
        break;
      }
      case ePROVIDER_IPC_MSG_ID_ENABLE_SERVICE:
      {
        mQMIConnectionPending = true;
        // Release of memory for Connction objects
        for (int i=0; i < SPB_MAX_SERV; i++)
        {
          if (sscConnection[i])
          {
            delete sscConnection[i];
            sscConnection[i] = NULL;
          }
        }
        SLIM_LOGV("Service Restore is being attempted.");
        startQMIReconnectTimer();
        break;
      }
      case ePROVIDER_IPC_MSG_ID_REOPEN:
      {
        {
          MutexLock _l(m_zMutex);
          open();
        }
      break;
      }
      case ePROVIDER_IPC_MSG_ROUTE_CONFIG_CHANGE:
      {
        SLIM_LOGV("Sending route configuration change.AvailableMask: %x\n", availableServiceMask);
        m_qServiceMask = availableServiceMask;
        m_eOpenResult = SENSORPROTOBUF_SUCCESS;
        /* delete suid_lookup instance */
        if (lookUpSuid) {
          delete lookUpSuid;
          lookUpSuid = NULL;
        }
        /* stop the timer and send route config change */
        slim_TimerStop(mSuidCbExpiryTimer);
        routeConfigurationChange(availableServiceMask);
        break;
      }
      case ePROVIDER_IPC_MSG_NEW_SENSORPROTOBUF_EVENT:
      {
        ProtoBufEvent *pzNewEvent = NULL;
        MutexLock _l(m_zMutex);
        SLIM_LOGV("ePROVIDER_IPC_MSG_NEW_SENSORPROTOBUF_EVENT sizeof(pzNewEvent):%" PRIuPTR ""
                  "qsize:%d", sizeof(pzNewEvent), pz_Msg->q_Size);
        slim_Memscpy(&pzNewEvent, sizeof(pzNewEvent),
                  pz_Msg->p_Data, pz_Msg->q_Size);
        if (NULL != pzNewEvent)
        {
          SensorProtobufProcessSensorData(pzNewEvent->data, pzNewEvent->size);
          if (NULL != pzNewEvent->data)
          {
            free( pzNewEvent->data );
          }
          free( pzNewEvent );
        }
        else
        {
          SLIM_LOGE("pzNewEvent is null");
        }
      break;
      }
      default:
        SLIM_LOGE("Unknown Sensor Event.");
        break;
    }
    slim_IpcDelete(pz_Msg);
  }
}

/**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
slimErrorEnumT SensorProtobufProvider::getTimeUpdate
(
 int32_t lTxnId
 )
{
  slimErrorEnumT retVal = eSLIM_SUCCESS;
  SLIM_LOGD("Requesting time update: txn=%" PRId32, lTxnId);
  return retVal;
}

/**
  @brief Returns SLIM provider interface

  Method constructs provider instance, initializes it, and returns SLIM
  provider interface.

  @return SLIM provider interface of 0 on error.
  */
slim_ServiceProviderInterfaceType *SensorProtobufProvider::getServiceProvider()
{
  SensorProtobufProvider *pzInstance = getInstance();
  if (0 != pzInstance)
  {
    return pzInstance->getSlimInterface();
  }
  else
  {
    return 0;
  }
}

/**
  @brief Returns SLIM interface

  The method initializes singleton instance and returns SLIM provider
  interface.

  @return SLIM interface pointer
  */
SensorProtobufProvider *SensorProtobufProvider::getInstance()
{
  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new SensorProtobufProvider;
    if (!s_pzInstance->init())
    {
      SLIM_LOGE("SensorProtobuf provider initialization failed");
      delete s_pzInstance;
      s_pzInstance = 0;
    }
  }
  return s_pzInstance;
}

/**
  @brief Configures batch mode support

  @param[in] uFlag Flag to enable (true) or disable batch mode.
  */
void SensorProtobufProvider::setBatchMode
(
 bool uFlag
 )
{
  if (!uFlag)
  {
    //! @todo Add support for non-batching mode
    SLIM_LOGW("Can not disable batch mode for SensorProtobuf provider");
    return;
  }
  m_uBatchMode = uFlag;
}

void SensorProtobufProvider::setBatchingConfig(void)
{
  loc_param_s_type z_sapParameterTable[] =
  {
    { "COUNT_BASED_BATCHING", &m_countBasedBatchingCfg, NULL, 'n' },
    { "SYNC_ONCE", &m_syncOnceCfg, NULL, 'n' },
  };

  /* Read Batching configuration and Time Sync Needed */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
  SLIM_LOGD("m_countBasedBatchingCfg = %d and m_syncOnceCfg = %d \n",
      m_countBasedBatchingCfg,
      m_syncOnceCfg);
}

//! @}
