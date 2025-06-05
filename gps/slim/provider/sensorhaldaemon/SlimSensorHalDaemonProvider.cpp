/**
 * Copyright (c) 2016 - 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* @file
 * @brief Sensor HAL Daemon provider implementation file.
 * This file defines Sensor HAL Deamon provider object implementation.
 */

#include <inttypes.h>
#include <slim_os_log_api.h>
#include <slim_os_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <loc_cfg.h>
#include <loc_pla.h>
#include <cmath>
#include <fstream>
#include <dirent.h>
#include <SensorClientApi.h>
#include <SlimSensorHalDaemonProvider.h>
#include <functional>
#include <string>

//! @addtogroup slim_SensorHALDaemonProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module for SensorHALDaemon provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_HAL_DAEMON
#define VARIABLE_COUNT_BASED_BATCHING_DAEMON 2

using namespace slim;
using namespace sensor_client;

const uint32_t SensorHalDaemonProvider::s_qThreadId = THREAD_ID_SENSOR_HAL_DAEMON;
SensorHalDaemonProvider *SensorHalDaemonProvider::s_pzInstance;
Mutex SensorHalDaemonProvider::s_zInstanceMutex;
const slimAvailableServiceMaskT SensorHalDaemonProvider::s_qSupportedServices =
(uint32_t(1) << eSLIM_SERVICE_SENSOR_ACCEL)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_GYRO)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_MAG_UNCALIB)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_ACCEL_TEMP)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_GYRO_TEMP);

void SensorHalDaemonProvider::threadMain(void *pData)
{
  SLIM_LOGD("SensorHalDaemonProvider::threadMain(0x%"PRIxPTR")",
      (uintptr_t)pData);
  SensorHalDaemonProvider *pzSensorHalProvider =
      reinterpret_cast<SensorHalDaemonProvider*>(pData);
  slim_IpcStart(s_qThreadId);
  if (nullptr != pzSensorHalProvider)
  {
    if (false == pzSensorHalProvider->runEventLoop())
    {
      SLIM_LOGE("SensorHalDaemonProvider::threadMain Exiting (0x%"PRIxPTR")",
        (uintptr_t)pData);
    }
  }
}

/*!
 * @brief Returns provider instance.
 *
 * Method provides access to provider instance. If necessary, the instance
 * is created and initialized.
 *
 * @return Provider instance.
 * @retval 0 On error.
 */
slim_ServiceProviderInterfaceType *SensorHalDaemonProvider::getProvider()
{
  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new SensorHalDaemonProvider;
    // Provider initialization is delayed until
    // SHD Start indication is received in callback onCapabilitiesCb
    memset(&s_pzInstance->m_zThread, 0, sizeof(s_pzInstance->m_zThread));
    if (!slim_ThreadCreate(&s_pzInstance->m_zThread,
        threadMain, s_pzInstance, "SensorHalDaemonProviderThread"))
    {
      SLIM_LOGE("Slim Thread create fail!!!");
      return nullptr;
    }
  }
  SLIM_LOGD("Sensor HAL Daemon provider initialization success ");
  return s_pzInstance->getSlimInterface();
}

SensorHalDaemonProvider::SensorHalDaemonProvider()
    : MultiplexingProvider(SLIM_PROVIDER_SENSOR_HAL_DAEMON, true, 0),
    m_zThread(), m_zMutex(), mBufferTxStatus(KERNEL_BUFFER_DATA_INIT),
    mAccServiceStatus(false), mGyroServiceStatus(false), m_Enable(true),
    mCountBasedBatchingCfg(VARIABLE_COUNT_BASED_BATCHING_DAEMON)
{
  mtAccelDispatchTimestamp = 0ull;
  mtGyroDispatchTimestamp = 0ull;
  mSensorList = NULL;
  mSensorCount = 0;
  isBufferDataTriggered = false;
  memset(&m_zThread, 0, sizeof(m_zThread));
}

/*!
 * @brief Closes temperature files.
 */
SensorHalDaemonProvider::~SensorHalDaemonProvider()
{
  if (nullptr != sensorInterface)
    delete sensorInterface;
}

/**
 * @brief set buffer data state.
 *
 * @param[in] state.
 *
 * @return void.
 */
void SensorHalDaemonProvider::setBufferState(BufferTxStatus state)
{
  mBufferTxStatus = state;
}

/**
 * @brief Intialization for Buffer data Handling.
 *
 * Function Initializes buffer handler states and structurtes
 *
 * @param[in] void.
 *
 * @return true on success.
 */
bool SensorHalDaemonProvider::memsBufferDataInit()
{

  memset(&m_zBuffThread, 0, sizeof(m_zBuffThread));
  m_zAccBuffSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
  m_zAccBuffSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  m_zAccBuffSensorData.flags = 0;
  m_zAccBuffSensorData.sensorType = eSLIM_SENSOR_TYPE_ACCEL;

  m_zGyroBuffSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
  m_zGyroBuffSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  m_zGyroBuffSensorData.flags = 0;
  m_zGyroBuffSensorData.sensorType = eSLIM_SENSOR_TYPE_GYRO;

  return true;
}

/**
 * @brief buffer state set to complete by this function.
 *
 * @param[in] void
 *
 * @return void.
 */
void SensorHalDaemonProvider::memsBufferDataDeInit()
{
  setBufferState(COMPLETED_BUFFER_DATA);
}

#define DEBUG_SLIM_SENSOR_HAL_DAEMON_DATA 0
/**
 * @brief converts event to core format.
 *
 * @param[in] e - event info
 *            samples - data
 *
 * @return void.
 */
void SensorHalDaemonProvider::convert_event(const struct sensors_event_t *sensorEvent,
                                            slimSensorSampleStructT *samples)
{
  switch (sensorEvent->type) {
    case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
      samples->sample[0] = sensorEvent->uncalibrated_accelerometer.x_uncalib;
      samples->sample[1] = sensorEvent->uncalibrated_accelerometer.y_uncalib;
      samples->sample[2] = sensorEvent->uncalibrated_accelerometer.z_uncalib;
      samples->sampleTimeOffsetMs = sensorEvent->timestamp / 1000000;
      samples->sampleTimeOffsetSubMilliNs = sensorEvent->timestamp % 1000000;
      break;
    case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
      samples->sample[0] = sensorEvent->uncalibrated_gyro.x_uncalib;
      samples->sample[1] = sensorEvent->uncalibrated_gyro.y_uncalib;
      samples->sample[2] = sensorEvent->uncalibrated_gyro.z_uncalib;
      samples->sampleTimeOffsetMs = sensorEvent->timestamp / 1000000;
      samples->sampleTimeOffsetSubMilliNs = sensorEvent->timestamp % 1000000;
      break;
    default:
      SLIM_LOGD("Unknown sensor_id events %d\n", sensorEvent->type);
  }
#if DEBUG_SLIM_SENSOR_HAL_DAEMON_DATA
  SLIM_LOGV("sensor:%s, time:%dms:%dns, current SLIMTime:%" PRIu64 "",
      getSensorName(sensorEvent->type),
      samples->sampleTimeOffsetMs,
      samples->sampleTimeOffsetSubMilliNs,
      slim_TimeTickGetMilliseconds());
  SLIM_LOGV("sensor:%s, value=<%f,%f,%f>\n",
      getSensorName(sensorEvent->type),
      samples->sample[0],
      samples->sample[1],
      samples->sample[2]);
#endif
}

/**
 * @brief api shared by SLIM to SENSOR once buffer data is read to delete it.
 *
 * @param[in] events = event info
 *            count  = num events
 *
 * @return void.
 */
void SensorHalDaemonProvider::bufferReadCallBack_delete(const sensors_event_t *events,
                                                        uint32_t count)
{
  SLIM_LOGD("Buffer callback count = %d, events[0].type:%d mBufferTxStatus:%d",
                                     count, events[0].type, mBufferTxStatus);
}

/**
 * @brief Buffered data processing task.
 *
 * Function for processing buffered data from sysfs interface
 * and send it for formatting and routing through slim core.
 *
 * @return void.
 */
void SensorHalDaemonProvider::bufferReadCallBack(const sensors_event_t *events, uint32_t count)
{
  bool accelBuffDataTxProgress = false;
  bool gyroBuffDataTxProgress = false;
  int buffSamplesReceived[2] = {0, 0};
  // Buffer for reading raw events from the queue
  // The events are sorted by timestamp by event queue facility
  slimSensorSampleStructT samples[2];
  ssize_t accNumRead = 0, gyroNumRead = 0;

  if (nullptr == events || !count)
  {
    SLIM_LOGE("Buffer callback error: nullptr - %u, count = %d", (nullptr == events), count);
    return;
  }

  SLIM_LOGD("Buffer callback count = %d, events[0].type:%d mBufferTxStatus:%d",
                                                         count, events[0].type, mBufferTxStatus);
  if (1 == count && 0xFFFFFFFF == events[0].timestamp
      && (SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED == events[0].type ||
        SENSOR_TYPE_ACCELEROMETER == events[0].type))
  {
    slimSensorSampleStructT zSensorEvent;

    /* send isamples from seconday buffer data*/
    setBufferState(SLIM_BUFFER_DATA_IN_PROGRESS);
    //Send secondary buffer data
    while ( mBufferTxStatus && (!mSecAccBufferList.empty() || !mSecGyroBufferList.empty()) )
    {
      //process acc data samples from secondary buffer
      if (!mSecAccBufferList.empty()){
        zSensorEvent = mSecAccBufferList.front();
        mSecAccBufferList.pop_front();
        accNumRead += 1;

        if (zSensorEvent.sampleTimeOffsetMs < samples[0].sampleTimeOffsetMs ) {
          SLIM_LOGD("ACCEL Ignore this duplicate sample, time stamp %d",
              zSensorEvent.sampleTimeOffsetMs);
          accNumRead -= 1;
        } else if (1 == accNumRead) {
          m_zAccBuffSensorData.flags = 0;
          m_zAccBuffSensorData.timeBase = zSensorEvent.sampleTimeOffsetMs;
          m_zAccBuffSensorData.samples[0] = zSensorEvent;
          m_zAccBuffSensorData.samples[0].sampleTimeOffsetMs = 0;
        } else {
          m_zAccBuffSensorData.samples[accNumRead - 1] = zSensorEvent;
          m_zAccBuffSensorData.samples[accNumRead - 1].sampleTimeOffsetMs =
            zSensorEvent.sampleTimeOffsetMs - m_zAccBuffSensorData.timeBase;
        }
      }
      //process gyro data samples from secondary buffer
      if (!mSecGyroBufferList.empty()) {
        zSensorEvent = mSecGyroBufferList.front();
        mSecGyroBufferList.pop_front();
        gyroNumRead += 1;

        if (zSensorEvent.sampleTimeOffsetMs < samples[1].sampleTimeOffsetMs ) {
          SLIM_LOGD("GYRO Ignore this duplicate sample, time stamp %d",
              zSensorEvent.sampleTimeOffsetMs);
          gyroNumRead -= 1;
        } else if (1 == gyroNumRead) {
          m_zGyroBuffSensorData.flags = 0;
          m_zGyroBuffSensorData.timeBase = zSensorEvent.sampleTimeOffsetMs;
          m_zGyroBuffSensorData.samples[0] = zSensorEvent;
          m_zGyroBuffSensorData.samples[0].sampleTimeOffsetMs = 0;
        } else {
          m_zGyroBuffSensorData.samples[gyroNumRead - 1] = zSensorEvent;
          m_zGyroBuffSensorData.samples[gyroNumRead - 1].sampleTimeOffsetMs =
            zSensorEvent.sampleTimeOffsetMs - m_zGyroBuffSensorData.timeBase;
        }
      }
      /* once count becomes 50, send secondary buffer data to clients*/
      if (50 == accNumRead) {
        m_zAccBuffSensorData.samples_len = accNumRead;
        routeIndication(eSLIM_SERVICE_SENSOR_ACCEL, m_zAccBuffSensorData);
        accNumRead = 0;
      }
      if (50 == gyroNumRead) {
        m_zGyroBuffSensorData.samples_len = gyroNumRead;
        routeIndication(eSLIM_SERVICE_SENSOR_GYRO, m_zGyroBuffSensorData);
        gyroNumRead = 0;
      }
    }
    /* send remainig samples from secondary buffer */
    if ( mBufferTxStatus && accNumRead > 0) {
      m_zAccBuffSensorData.samples_len = accNumRead;
      routeIndication(eSLIM_SERVICE_SENSOR_ACCEL, m_zAccBuffSensorData);
    }
    if ( mBufferTxStatus && gyroNumRead > 0) {
      m_zGyroBuffSensorData.samples_len = gyroNumRead;
      routeIndication(eSLIM_SERVICE_SENSOR_GYRO, m_zGyroBuffSensorData);
    }

    setBufferState(COMPLETED_BUFFER_DATA);
    /* Enabling the Batching */
    if (mAccReportRateHz > 1)
    {
      SLIM_LOGD("enableBatching eService = %d", eSLIM_SERVICE_SENSOR_ACCEL);
      enableBatching(eSLIM_SERVICE_SENSOR_ACCEL);
    }
    if (mGyroReportRateHz > 1)
    {
      SLIM_LOGD("enableBatching eService = %d", eSLIM_SERVICE_SENSOR_GYRO);
      enableBatching(eSLIM_SERVICE_SENSOR_GYRO);
    }
    SLIM_LOGD("Buffer callback Finished sending all packets count = %d, events[0].type:%d",
                                                         count, events[0].type);
    if (true == isBufferDataTriggered)
    {
      sendIndication(DELETE_BUFFER_DATA);
    }
    return;
  }
  else  {
    setBufferState(KERNEL_BUFFER_DATA_IN_PROGRESS);

    SLIM_LOGD("processing sensor data eService = %d ", events[0].type);
    for (auto i = 0; i < count; i++) {
      const sensors_event_t& data = events[i];

      if (SENSOR_TYPE_ACCELEROMETER == data.type ||
          SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED == data.type) {
        convert_event(&events[i], &samples[0]);
        accNumRead += 1;
        buffSamplesReceived[0]++;
        if (accNumRead == 1) {
          accelBuffDataTxProgress = true;
          m_zAccBuffSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
          m_zAccBuffSensorData.flags = SLIM_FLAGS_MASK_SENSOR_BUFFER_DATA;
          m_zAccBuffSensorData.timeBase = samples[0].sampleTimeOffsetMs;
          m_zAccBuffSensorData.samples[0] = samples[0];
          m_zAccBuffSensorData.samples[0].sampleTimeOffsetMs = 0;
          m_zAccBuffSensorData.samples[0].sampleTimeOffsetSubMilliNs =
            samples[0].sampleTimeOffsetSubMilliNs;
        } else if (accNumRead <= 50){
          m_zAccBuffSensorData.samples[accNumRead - 1] = samples[0];
          m_zAccBuffSensorData.samples[accNumRead - 1].sampleTimeOffsetMs =
              samples[0].sampleTimeOffsetMs - m_zAccBuffSensorData.timeBase;
          m_zAccBuffSensorData.samples[accNumRead - 1].sampleTimeOffsetSubMilliNs =
            samples[0].sampleTimeOffsetSubMilliNs;
        }
      }
      /* Fill the gyro buffered data into from kernel buffer */
      else if (SENSOR_TYPE_GYROSCOPE_UNCALIBRATED == data.type ||
               SENSOR_TYPE_GYROSCOPE == data.type ) {
        convert_event(&events[i], &samples[1]);
        gyroNumRead += 1;
        buffSamplesReceived[1]++;
        if (gyroNumRead == 1) {
          gyroBuffDataTxProgress = true;
          m_zGyroBuffSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
          m_zGyroBuffSensorData.flags = SLIM_FLAGS_MASK_SENSOR_BUFFER_DATA;
          m_zGyroBuffSensorData.timeBase = samples[1].sampleTimeOffsetMs;
          m_zGyroBuffSensorData.samples[0] = samples[1];
          m_zGyroBuffSensorData.samples[0].sampleTimeOffsetMs = 0;
          m_zGyroBuffSensorData.samples[0].sampleTimeOffsetSubMilliNs =
            samples[1].sampleTimeOffsetSubMilliNs;
        } else if (gyroNumRead <= 50){
          m_zGyroBuffSensorData.samples[gyroNumRead - 1] = samples[1];
          m_zGyroBuffSensorData.samples[gyroNumRead - 1].sampleTimeOffsetMs =
            samples[1].sampleTimeOffsetMs - m_zGyroBuffSensorData.timeBase;
          m_zGyroBuffSensorData.samples[gyroNumRead - 1].sampleTimeOffsetSubMilliNs =
            samples[1].sampleTimeOffsetSubMilliNs;
        }
      }
      /* once count become 50, send buffer data to clients*/
      if (accelBuffDataTxProgress && (accNumRead == 50)) {
        SLIM_LOGD("Send processing sensor data accel eService = %d ", data.type);
        m_zAccBuffSensorData.samples_len = 50;
        routeIndication(SlimSensorTypeToService(m_zAccBuffSensorData.sensorType),
                        m_zAccBuffSensorData);
        accNumRead = 0;
      }
      if (gyroBuffDataTxProgress && (gyroNumRead == 50)) {
        SLIM_LOGD("Send processing sensor data gyro eService = %d ", data.type);
        m_zGyroBuffSensorData.samples_len = 50;
        routeIndication(SlimSensorTypeToService(m_zGyroBuffSensorData.sensorType),
                        m_zGyroBuffSensorData);
        gyroNumRead = 0;
      }
    }

    /* send remaining samples from buffered data*/
    if ( mBufferTxStatus && accNumRead > 0) {
      SLIM_LOGD("Send with sleep remaining sensor data accel eService = %d accNumRead:%d",
                                                         events[0].type, accNumRead);
      m_zAccBuffSensorData.samples_len = accNumRead;
      routeIndication(SlimSensorTypeToService(m_zAccBuffSensorData.sensorType),
                      m_zAccBuffSensorData);
      accNumRead = 0;
    }
    if ( mBufferTxStatus && gyroNumRead > 0) {
      SLIM_LOGD("Send with sleep remaining sensor data gyro eService = %d gyroNumRead:%d",
                                                       events[1].type, gyroNumRead);
      m_zGyroBuffSensorData.samples_len = gyroNumRead;
      routeIndication(SlimSensorTypeToService(m_zGyroBuffSensorData.sensorType),
                                              m_zGyroBuffSensorData);
      gyroNumRead = 0;
    }
  }
  SLIM_LOGD("processing sensor buffer data call back complete = %d mBufferTxStatus:%d",
                                                         events[0].type, mBufferTxStatus);
}


/**
 * @brief get available sensors mask from list.
 *
 * Function to convert sensor list to sensor available mask
 *
 * @param[in] senList.
 *
 * @return slimAvailableServiceMaskT.
 */
slimAvailableServiceMaskT SensorHalDaemonProvider::getAvailableSensorsMask(sensor_list *senList)
{
  slimAvailableServiceMaskT senMask = 0;

  //convert sensor list to sensor available mask
  (void)senList;
  return senMask;
}


/**
 * @brief Start Temp sensor callback .
 *
 * Function for handling Temp sensor data
 *
 * @param[in] temperature .
 *
 * @return void.
 */
void SensorHalDaemonProvider::temperatureCallback(float temperature)
{
  m_zSensorData.samples[0].sample[0] = temperature;
  m_zSensorData.samples[0].sampleTimeOffsetMs = 0;
  m_zSensorData.samples[0].sampleTimeOffsetSubMilliNs = 0;
  m_zSensorData.samples_len = 1;
  m_zSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
  m_zSensorData.timeBase = slim_TimeTickGetMilliseconds() ; //Convert it to msec;
  m_zSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
  m_zSensorData.flags = 0;
  m_zSensorData.sensorType = eSLIM_SENSOR_TYPE_GYRO_TEMP;
  routeIndication(eSLIM_SERVICE_SENSOR_GYRO_TEMP, m_zSensorData);
  m_zSensorData.sensorType = eSLIM_SENSOR_TYPE_ACCEL_TEMP;
  routeIndication(eSLIM_SERVICE_SENSOR_ACCEL_TEMP, m_zSensorData);
}

/**
 * @brief Temp Sensor data processing task.
 *
 * Function for processing buffered data from sysfs interface
 * and send it for formatting and routing through slim core.
 *
 * @return void.
 */
void SensorHalDaemonProvider::tempSensorDataPollTask()
{
  int readReturn = 0;
  mTempuS = mTempuS<=0?1e6:mTempuS;
  while ( 0 == readReturn ) {
    SLIM_LOGD("Polling Temp Sensor ..\n");
    pthread_mutex_lock(&mHalTempMutex);
    if (sensorInterface) {
      readReturn = sensorInterface->sensor_read_temperature(
                       std::bind(&SensorHalDaemonProvider::temperatureCallback,
                        this, std::placeholders::_1));
    }
    else {
      readReturn = -1;
      pthread_mutex_unlock(&mHalTempMutex);
      break;
    }
    pthread_mutex_unlock(&mHalTempMutex);
    usleep(mTempuS);
  }
  SLIM_LOGE("Temp thread exit. readReturn:%d Error - %s", readReturn, strerror(errno));
}

/**
 * @brief Start Temp sensor task.
 *
 * Function for Start Temp sensor task
 *
 * @param[in] pData .
 *
 * @return void.
 */
void SensorHalDaemonProvider::tempSensorDataHandler(void *pData)
{
  SensorHalDaemonProvider *pzHALProvider = reinterpret_cast<SensorHalDaemonProvider*>(pData);
  SLIM_LOGD("tempSensorDataHandler Thread..\n");
  pzHALProvider->tempSensorDataPollTask();
}

/**
 * @brief Temperature data member init
 *
 * @param[in].
 *
 * @return void.
 */
bool SensorHalDaemonProvider::tempSensorDataInit()
{
  memset(&m_zTempSensorThread, 0, sizeof(m_zTempSensorThread));

  SLIM_LOGD("Initializing Temp Sensor..\n");
  /* Create Thread for reading Temperature sensor data */
  pthread_mutex_init(&mHalTempMutex, NULL);
  pthread_mutex_lock(&mHalTempMutex);
  mTempuS = 1e6;
  mTempServiceStatus = false;
  if (!slim_ThreadCreate(&m_zTempSensorThread, tempSensorDataHandler, this, "HALTempData"))
  {
    SLIM_LOGE("tempSensorDataInit: Slim Thread create fail");
    return false;
  }
  SLIM_LOGD("Initializing Temp Sensor.. Done\n");
  return true;
}

void SensorHalDaemonProvider::onCapabilitiesCb(SensorCapabilitiesMask mask) {
  switch (mask) {
    case SHD_READY:
      sendIndication(INITIALIZE_SHD_PROVIDER);
      SLIM_LOGD("Sensor Hal daemon is Ready to commnunicate");
     break;
    case SHD_RESTARTED:
      SLIM_LOGD("Sensor Hal daemon is Restarted");
      break;
    case SHD_NOT_RUNNING:
    SLIM_LOGD("Sensor Hal daemon is Not Running");
    break;
    case DEVICE_SUSPEND:
      SLIM_LOGD("Sensor Hal daemon is Suspended");
      break;
    case DEVICE_RESUME:
      SLIM_LOGD("Sensor Hal daemon Resume");
      break;
    case DEVICE_SHUTDOWN:
      SLIM_LOGD("Sensor Hal daemon is Shutdown");
      break;
    default:
      SLIM_LOGE("Sensor Hal daemon is Unknown Event:%d\n", mask);
      break;
  }
}
/**
* @brief Find nearest supported ODR.
*
* @param[in] sensor   Sensor number
* @param[in] inODR    Input ODR requested
* @param[out] outODR  Output ODR supported by SHD
*
* @bool true if nearest found else false if not found in list
*/
bool SensorHalDaemonProvider::getNearestODR(int sensor, int inODR, int *outODR)
{
  bool retVal = false;
  int outODRIndex = 0;
  int cnt =0;
  //Check if this service is ready from SHD
  for (cnt = 0; cnt < mSensorCount; cnt++)
  {
    if (mSensorList[cnt].type == sensor)
    {
      //Service is ready Find nearest ODR
      for (outODRIndex = 0; outODRIndex < MAX_ODR; outODRIndex++)
      {
        if ((outODRIndex+1 >= MAX_ODR) ||
            (0 == mSensorList[cnt].odr[outODRIndex+1] ))
        {
          *outODR = mSensorList[cnt].odr[outODRIndex];  //This is max supported ODR for service

          retVal = true;
          break;
        }
        else if (inODR <= mSensorList[cnt].odr[outODRIndex+1])
        {
          float middleVal  =
            (mSensorList[cnt].odr[outODRIndex] + mSensorList[cnt].odr[outODRIndex + 1]) / (float)2;
          if (inODR <= middleVal)
            *outODR = mSensorList[cnt].odr[outODRIndex];
          else
            *outODR = mSensorList[cnt].odr[outODRIndex+1];

          retVal = true;
          break;
        }
      }

      if (true == retVal)
        break;
    }
  }
  return retVal;
}

/**
 * @brief initialize hal daemon provider object.
 *
 * @param[in]
 *
 * @return void.
 */
bool SensorHalDaemonProvider::initialize()
{
  memset( m_zSensorInfoList, sizeof(m_zSensorInfoList), 0);

  /* Retrive Batching configuration */
  setBatchingConfig();
  // If sensor count is found to be zero , need to delay this API call until enable sensor
  if (NULL != sensorInterface)
    sensorInterface->get_sensor_list(&mSensorList, &mSensorCount);
  //pzSensorHalProvider->m_qServiceMask = getAvailableSensorsMask(mSensorList); // TBD
  SLIM_LOGV("initialize..... sensor_count:%d sensorInterface:%x \n",
      mSensorCount, sensorInterface);

  for (int i = 0; i < mSensorCount; i++)
  {
    uint8_t VendorName[100] ={0};

    slimSensorTypeEnumT slimSensor = SensorTypeToSlimSensorType(mSensorList[i].type);
    slimServiceEnumT slimSeervice  = SlimSensorTypeToService(slimSensor);
    if (eSLIM_SERVICE_NONE == slimSeervice){
      continue;
    }
    strlcat((char *)&VendorName[0], mSensorList[i].vendor, sizeof(VendorName));
    strlcat((char *)&VendorName[0], mSensorList[i].name, sizeof(VendorName));

    // Get Vendor , Need not to copy every time it is common for all sensors
    slim_Memscpy(&m_zSensorInfoList[slimSeervice].vendorName,
        sizeof(m_zSensorInfoList[slimSeervice].vendorName),
        &mSensorList[i].vendor, sizeof(mSensorList[i].vendor));

    //Need to update this to current ODR
    m_zSensorInfoList[slimSeervice].samplingRate = mSensorList[i].maxSamplingRate;
    m_zSensorInfoList[slimSeervice].fullScaleSetting = mSensorList[i].range;
    SLIM_LOGD("Sensor Name:%s MaxSamplingRate:%d Range:%d\n",
        mSensorList[i].name, mSensorList[i].maxSamplingRate, mSensorList[i].range);
    SLIM_LOGD("slimSensor:%d FullScaleSetting[slimSeervice:%d]:%d ODR: %d Vendor Name: %s\n",
        slimSensor,
        slimSeervice,
        m_zSensorInfoList[slimSeervice].fullScaleSetting,
        m_zSensorInfoList[slimSeervice].samplingRate,
        m_zSensorInfoList[slimSeervice].vendorName
        );
    for (int j = 0; j < MAX_ODR; j++)
    {
      SLIM_LOGV("sensor ODR[%d]:%f\n", j, mSensorList[i].odr[j]);
    }
  }

  m_qServiceMask = s_qSupportedServices;

  // call Initialize buffere data
  if (memsBufferDataInit()) {
    SLIM_LOGD("Buffer data initialize.....Done \n");
  } else {
    memsBufferDataDeInit();
  }

  /* Initialize Temp Sensor */
  tempSensorDataInit();

  /* send service information as threads in ready state */
  routeConfigurationChange(s_qSupportedServices, m_zSensorInfoList);

  SLIM_LOGD("initialization complete ..\n");

  return true;
}

/**
 * @brief get sensor name from type.
 *
 * @param[in] type = type of the sensor , enum
 *
 * @return  char const* = type of sensor string.
 */
char const* SensorHalDaemonProvider::getSensorName(int type) {
  switch (type) {
    case SENSOR_TYPE_ACCELEROMETER:
      return "Acc";
    case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
      return "AcU";
    case SENSOR_TYPE_MAGNETIC_FIELD:
      return "Mag";
    case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
      return "MaU";
    case SENSOR_TYPE_ORIENTATION:
      return "Ori";
    case SENSOR_TYPE_GYROSCOPE:
      return "Gyr";
    case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
      return "GyU";
    case SENSOR_TYPE_LIGHT:
      return "Lux";
    case SENSOR_TYPE_PRESSURE:
      return "Bar";
    case SENSOR_TYPE_TEMPERATURE:
      return "Tmp";
    case SENSOR_TYPE_PROXIMITY:
      return "Prx";
    case SENSOR_TYPE_GRAVITY:
      return "Grv";
    case SENSOR_TYPE_LINEAR_ACCELERATION:
      return "Lac";
    case SENSOR_TYPE_ROTATION_VECTOR:
      return "Rot";
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
      return "Hum";
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
      return "Tam";
  }
  return "ukn";
}

/**
 * @brief get SLIM enumfor type of sensor data corresponding to SENSOR.
 *
 * @param[in] sensorType =  type in SENSOR format.
 *
 * @return type in SLIM format
 */
slimSensorTypeEnumT SensorHalDaemonProvider::SensorTypeToSlimSensorType(int sensorType)
{
  switch (sensorType)
  {
    case SENSOR_TYPE_ACCELEROMETER:
    case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
      return eSLIM_SENSOR_TYPE_ACCEL;

    case SENSOR_TYPE_MAGNETIC_FIELD:
      return eSLIM_SENSOR_TYPE_MAGNETOMETER;

    case SENSOR_TYPE_GYROSCOPE:
    case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
      return eSLIM_SENSOR_TYPE_GYRO;

    case SENSOR_TYPE_PRESSURE:
      return eSLIM_SENSOR_TYPE_BAROMETER;

    case SENSOR_TYPE_GRAVITY:
    case SENSOR_TYPE_LINEAR_ACCELERATION:
    case SENSOR_TYPE_ROTATION_VECTOR:
    case SENSOR_TYPE_LIGHT:
    case SENSOR_TYPE_ORIENTATION:
    case SENSOR_TYPE_TEMPERATURE:
    case SENSOR_TYPE_PROXIMITY:
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
    default:
      return eSLIM_SENSOR_TYPE_MAX;
  }
}

/**
 * @brief get service type from sensor type.
 *
 * @param[in] slimSensorType= SLIM sensor type
 *
 * @return service type
 */
slimServiceEnumT SensorHalDaemonProvider::SlimSensorTypeToService(
                                             slimSensorTypeEnumT slimSensorType)
{
  switch (slimSensorType)
  {
    case eSLIM_SENSOR_TYPE_ACCEL:
      return eSLIM_SERVICE_SENSOR_ACCEL;
    case eSLIM_SENSOR_TYPE_GYRO:
      return eSLIM_SERVICE_SENSOR_GYRO;
    case eSLIM_SENSOR_TYPE_ACCEL_TEMP:
      return eSLIM_SERVICE_SENSOR_ACCEL_TEMP;
    case eSLIM_SENSOR_TYPE_GYRO_TEMP:
      return eSLIM_SERVICE_SENSOR_GYRO_TEMP;
    case eSLIM_SENSOR_TYPE_MAGNETOMETER:
      return eSLIM_SERVICE_SENSOR_MAG_UNCALIB;
    case eSLIM_SENSOR_TYPE_BAROMETER:
      return eSLIM_SERVICE_SENSOR_BARO;
    case eSLIM_SENSOR_TYPE_MAX:
    default:
      return eSLIM_SERVICE_NONE;
  }
}

/**
 * @brief live data callback.
 *
 * @param[in] sensor_id = type
 *            events = data
 *            count = num data
 *
 * @return void.
 */
void SensorHalDaemonProvider::eventCallback(int sensor_id,
                                            const sensors_event_t *events,
                                            uint32_t count)
{

  SLIM_LOGD("Handle Sensor Event");
  m_zAccSensorData.samples_len = 0;
  m_zGyroSensorData.samples_len = 0;
  m_bIsAccelPPSTimeNotReached = true;
  m_bIsGyroPPSTimeNotReached = true;
  slim_IpcMsgT* pz_Msg = NULL;

  if (nullptr == events || !count)
  {
    SLIM_LOGE("Handle Sensor Event NULL count:%d !!!!!", count);
    return;
  }

  SLIM_LOGD("read %d events mBufferTxStatus:%d\n", count, mBufferTxStatus);
  for (int i=0 ; i<count ; i++)
  {
    slimSensorSampleStructT sample;

    switch (events[i].type) {
      case SENSOR_TYPE_ACCELEROMETER:
      case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
        mAcc100HzBatchCount = count;
        /* Convert sensor event to SLIM format */
        convert_event(&events[i], &sample);
        /* If buffer data is in progress, keep these samples in secondary buffer */
        if (KERNEL_BUFFER_DATA_IN_PROGRESS == mBufferTxStatus ||
            SLIM_BUFFER_DATA_IN_PROGRESS == mBufferTxStatus)
        {
          m_zSensorData.samples[0] = sample;
          m_zSensorData.samples_len = 1;
          m_zSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
          m_zSensorData.timeBase = sample.sampleTimeOffsetMs;
          m_zSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
          m_zSensorData.flags = 0;
          m_zSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
          SLIM_LOGD("Kernel/SLIM buffer data in progress,"
              " store this ACC sample in secondary buffer \n");
          m_zSensorData.samples[0].sampleTimeOffsetMs = m_zSensorData.timeBase;
          //Push to queue
          mSecAccBufferList.push_back(m_zSensorData.samples[0]);
        }
        else
        {
          SLIM_LOGD("Status Buffer Read-%d, Accel Service Status:%d\n",
                                                         mBufferTxStatus, mAccServiceStatus);
          if (!mBufferTxStatus)
          {
            uint64_t sensorTimeNsec = events[i].timestamp;
            slimSensorDataStructT zSensorData;
            zSensorData.samples[0] = sample;
            zSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
            zSensorData.timeBase = sensorTimeNsec / 1000000; /* Convert to msec */
            zSensorData.samples[0].sampleTimeOffsetSubMilliNs = sample.sampleTimeOffsetSubMilliNs;
            if (isBatchReady(eSLIM_SERVICE_SENSOR_ACCEL, m_zAccSensorData,
                  zSensorData,
                  sensorTimeNsec,
                  mtAccelDispatchTimestamp,
                  m_bIsAccelPPSTimeNotReached,
                  mAcc100HzBatchCount,
                  mCountBasedBatchingCfg,
                  mSyncOnceCfg))
            {
              SLIM_LOGD("Sending Accel Data Len:%d \n",
                                                         m_zAccSensorData.samples_len);
              routeIndication(SlimSensorTypeToService(m_zAccSensorData.sensorType),
                  m_zAccSensorData);
              m_zAccSensorData.samples_len = 0;
            }
          }
        }
        break;
      case SENSOR_TYPE_GYROSCOPE:
      case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
        mGyro100HzBatchCount = count;
        /* Convert sensor event to SLIM format */
        convert_event(&events[i], &sample);
        /* If buffer data is in progress, keep these samples in secondary buffer */
        if (mBufferTxStatus == KERNEL_BUFFER_DATA_IN_PROGRESS ||
            mBufferTxStatus == SLIM_BUFFER_DATA_IN_PROGRESS)
        {
          m_zSensorData.samples[0] = sample;
          m_zSensorData.samples[0].sampleTimeOffsetSubMilliNs = sample.sampleTimeOffsetSubMilliNs;
          m_zSensorData.samples_len = 1;
          m_zSensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
          m_zSensorData.timeBase = sample.sampleTimeOffsetMs;
          m_zSensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
          m_zSensorData.flags = 0;
          m_zSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
          SLIM_LOGD("Kernel/SLIM buffer data in progress,"
              " store this GYRO sample in secondary buffer \n");
          m_zSensorData.samples[0].sampleTimeOffsetMs = m_zSensorData.timeBase;
          //Push to queue
          mSecGyroBufferList.push_back(m_zSensorData.samples[0]);
        }
        else
        {
          SLIM_LOGD("Status Buffer Read-%d, Gyro  Service Status: %d\n",
                                                         mBufferTxStatus, mGyroServiceStatus);
          if (!mBufferTxStatus)
          {
            uint64_t sensorTimeNsec = events[i].timestamp;
            slimSensorDataStructT zSensorData;
            zSensorData.samples[0] = sample;
            zSensorData.sensorType = SensorTypeToSlimSensorType(events[i].type);
            zSensorData.timeBase = sensorTimeNsec / 1000000;
            zSensorData.samples[0].sampleTimeOffsetSubMilliNs = sample.sampleTimeOffsetSubMilliNs;
            if (isBatchReady(eSLIM_SERVICE_SENSOR_GYRO, m_zGyroSensorData,
                  zSensorData,
                  sensorTimeNsec,
                  mtGyroDispatchTimestamp,
                  m_bIsGyroPPSTimeNotReached,
                  mGyro100HzBatchCount,
                  mCountBasedBatchingCfg,
                  mSyncOnceCfg))
            {
              SLIM_LOGD("Sending Gyro Data Len:%d \n",
                                                         m_zGyroSensorData.samples_len);
              routeIndication(SlimSensorTypeToService(m_zGyroSensorData.sensorType),
                  m_zGyroSensorData);
              m_zGyroSensorData.samples_len = 0;
            }
          }
        }
        break;
      case SENSOR_TYPE_MAGNETIC_FIELD:
      case SENSOR_TYPE_ORIENTATION:
      case SENSOR_TYPE_GRAVITY:
      case SENSOR_TYPE_LINEAR_ACCELERATION:
      case SENSOR_TYPE_ROTATION_VECTOR:
      case SENSOR_TYPE_LIGHT:
      case SENSOR_TYPE_PRESSURE:
      case SENSOR_TYPE_TEMPERATURE:
      case SENSOR_TYPE_PROXIMITY:
      case SENSOR_TYPE_RELATIVE_HUMIDITY:
      case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        SLIM_LOGD("sensor=%s, time=%lld, value=%f\n",
            getSensorName(events[i].type),
            events[i].timestamp,
            events[i].data[0]);
        break;

      default:
        SLIM_LOGD("sensor=%d, time=%lld, value=<%f,%f,%f, ...>\n",
            events[i].type,
            events[i].timestamp,
            events[i].data[0],
            events[i].data[1],
            events[i].data[2]);
        break;
    }

    /* Other than  ACCELEROMETER and GYROSCOPE will be handled here */
    if (events[i].type != SENSOR_TYPE_ACCELEROMETER &&
        events[i].type != SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED &&
        events[i].type != SENSOR_TYPE_GYROSCOPE &&
        events[i].type != SENSOR_TYPE_GYROSCOPE_UNCALIBRATED)
    {
      slimServiceEnumT service = SlimSensorTypeToService(m_zSensorData.sensorType);
      if (eSLIM_SERVICE_NONE != service)
      {
        SLIM_LOGD("Sending m_zSensorData Len:%d \n", m_zSensorData.samples_len);
        routeIndication(service, m_zSensorData);
      }
    }
  }
  /* Send all residual data if batching is H/W FIFO.
     Precaution only if driver sends odd events */
//  if(VARIABLE_COUNT_BASED_BATCHING_DAEMON == mCountBasedBatchingCfg)
  {
    if (0 < m_zAccSensorData.samples_len )
    {
      SLIM_LOGD("Sending residual accel Data: %d \n", m_zAccSensorData.samples_len);
      routeIndication(SlimSensorTypeToService(m_zAccSensorData.sensorType), m_zAccSensorData);
      m_zAccSensorData.samples_len = 0;
    }
    if (0 < m_zGyroSensorData.samples_len )
    {
      SLIM_LOGD("Sending residual gyro Data: %d \n", m_zGyroSensorData.samples_len);
      routeIndication(SlimSensorTypeToService(m_zGyroSensorData.sensorType), m_zGyroSensorData);
      m_zGyroSensorData.samples_len = 0;
    }
  }
}


/**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
slimErrorEnumT SensorHalDaemonProvider::getTimeUpdate(int32_t lTxnId)
{
  SLIM_LOGD("Requesting time update: txn=%" PRId32, lTxnId);
  routeTimeResponse(lTxnId, eSLIM_ERROR_UNSUPPORTED, 0, 0);
  return eSLIM_SUCCESS;
}

/* callback by Sensor telling  the actual configuration.
 */
void SensorHalDaemonProvider::ConfigCallback(int handle, float sampling_rate, int batch_count)
{
  SLIM_LOGD("Config registered: sampling rate = %f, batch_count = %d",
                                                         sampling_rate, batch_count);
}

void SensorHalDaemonProvider::ConfigCallbackWithRotation(int handle, float sampling_rate,
                                                            int batch_count, bool rotation_needed)
{
    SLIM_LOGD("Config registered: sampling rate = %f, batch_count = %d rotation_needed %d",
                                                      sampling_rate, batch_count, rotation_needed);
}

void SensorHalDaemonProvider::sendIndication(SHDIndicationType shdIndication)
{
  /* send message to event processing loop */
  if (!slim_IpcSendData(THREAD_ID_SENSOR_HAL_DAEMON, IPC_MSG_SHD_INDICTION,
                        &shdIndication, sizeof(shdIndication)))
  {
    SLIM_LOGE("Error sending IPC message to event processing loop");
  }
}

/**
  @brief Event handler function

  Handler to Identify if any pending activities from provider
  execute pending activities such as delete Buffer data and initialization
*/
bool SensorHalDaemonProvider::runEventLoop()
{
  // Create Sensor Interface object
  sensorInterface = new SensorClient(std::bind(&SensorHalDaemonProvider::onCapabilitiesCb,
                                                     this,
                                                     std::placeholders::_1));
  if (NULL != sensorInterface)
  {
    SLIM_LOGD("SHD Interface initialization Successful sensorInterface:%x", sensorInterface);
  }
  else
  {
    SLIM_LOGD("SHD Interface initialization Failed sensorInterface:%x", sensorInterface);
    return false;
  }
  while (1)
  {
    slim_IpcMsgT* pz_Msg = NULL;
    while (NULL == (pz_Msg = slim_IpcReceive()));
    SLIM_LOGD("IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
        (uint32_t)pz_Msg->q_MsgId,
        (uint32_t)pz_Msg->q_SrcThreadId);

    switch (pz_Msg->q_MsgId)
    {
      case IPC_MSG_SHD_INDICTION:
      {
        SHDIndicationType shdInication;
        slim_Memscpy(&shdInication, sizeof(shdInication), pz_Msg->p_Data, pz_Msg->q_Size);
        SLIM_LOGD("shd event received");
        if (INITIALIZE_SHD_PROVIDER == shdInication)
        {
          if (!s_pzInstance->initialize())
          {
            SLIM_LOGE("Sensor HAL Daemon provider initialization failed");
            delete s_pzInstance;
            s_pzInstance = 0;
          }
          else
            SLIM_LOGD("Sensor HAL Daemon provider initialization SUCCESS ");
        }
        else if (DELETE_BUFFER_DATA == shdInication)
        {
          bool uEnable = 0;// Delete Buffer data
          int retValbuffRead = -1;
          //Delete Buffer data as all read complete
          retValbuffRead = sensorInterface->sensor_read_buffer_data(uEnable,
               std::bind(&SensorHalDaemonProvider::bufferReadCallBack_delete,
               this, std::placeholders::_1, std::placeholders::_2));
          SLIM_LOGD("End Delete Buffer retValbuffRead:%d uEnable:%d\n", retValbuffRead, uEnable);
          isBufferDataTriggered = false;
        }
      }
      break;
      default:
        SLIM_LOGE("Message Type not handled %d", pz_Msg->q_MsgId);
      break;
    }
  }
  return true;
}
/**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using Sensor1 Buffering
  API.

  @todo The sampling frequency is a mere suggestion to the sensor1 daemon.
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
slimErrorEnumT SensorHalDaemonProvider::doUpdateSensorStatus(bool uEnable,
                                                             uint16_t wReportRateHz,
                                                             uint16_t wSampleCount,
                                                             slimServiceEnumT eService)
{
  int sensor = 0;
  int err = 0;
  int samplingRate = wReportRateHz * wSampleCount;

  SLIM_LOGD("sensorUpdate service:%d enable:%d rate:%d sampleCount:%d",
            eService, uEnable, wReportRateHz, wSampleCount);

  if (NULL == mSensorList) {
    SLIM_LOGE("Get Sensor during configure!!!\n");
    if (NULL != sensorInterface)
      sensorInterface->get_sensor_list(&mSensorList, &mSensorCount);
  }

  if (NULL == mSensorList) {
    SLIM_LOGE("Sensor List not initialized %d", eService);
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }
  else
  {
    SLIM_LOGD("\nsensorID:[%d] Name:%s \n\n sensorID:[%d] Name:%s \n",
                                 mSensorList[0].sensor_id, mSensorList[0].name,
                                 mSensorList[1].sensor_id, mSensorList[1].name);
  }

  if (NULL == sensorInterface)
  {
    SLIM_LOGE("Sensor Interface NOT Initialized !!!\n");
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }
  /*Halt buffer data if being read*/
  if ( mBufferTxStatus && !uEnable &&  mAccServiceStatus && mGyroServiceStatus )
  {
    m_Enable = uEnable;
    setBufferState(COMPLETED_BUFFER_DATA);
    SLIM_LOGD("Changing buffer force state \n");
  }
  switch (eService){
    case eSLIM_SERVICE_SENSOR_ACCEL:
      sensor = SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED;
      mAccReportRateHz = wReportRateHz;
      mAccSampleCount = wSampleCount;
      /* In case of Count based batching for QDR, BatchCount should be
         always set to 100 Hz equivalent number. */
      if ((wReportRateHz * wSampleCount) > BATCHSIZE_100HZ)
      {
        mAcc100HzBatchCount = (samplingRate / BATCHSIZE_100HZ);
      }
      else
      {
        mAcc100HzBatchCount = wSampleCount;
      }
      mAccServiceStatus = uEnable;
      break;
    case eSLIM_SERVICE_SENSOR_GYRO:
      sensor = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
      mGyroReportRateHz = wReportRateHz;
      mGyroSampleCount = wSampleCount;
      /* In case of Count based batching for QDR, BatchCount should be
         always set to 100 Hz equivalent number. */
      if ((wReportRateHz * wSampleCount) > BATCHSIZE_100HZ)
      {
        mGyro100HzBatchCount = (samplingRate / BATCHSIZE_100HZ);
      }
      else
      {
        mGyro100HzBatchCount = wSampleCount;
      }
      mGyroServiceStatus = uEnable;
      break;
    case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
      sensor = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
      break;
    case eSLIM_SERVICE_SENSOR_BARO:
      sensor = SENSOR_TYPE_PRESSURE;
      break;
    case eSLIM_SERVICE_SENSOR_MAG_CALIB:
    case eSLIM_SERVICE_MAG_FIELD_DATA:
      sensor = SENSOR_TYPE_MAGNETIC_FIELD;
      break;
    case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
    case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
      sensor = SENSOR_TYPE_TEMPERATURE;
      if (uEnable)
      {
        if ( mTempServiceStatus == false)
        {
          pthread_mutex_unlock(&mHalTempMutex);
          mTempServiceStatus = true;
        }
        mTempuS = (int)(1e6/samplingRate);
        SLIM_LOGD("Triggering temp read status- %d, mTempuS - %d \n", mTempServiceStatus, mTempuS);
      }
      else if (mTempServiceStatus == true)
      {
        pthread_mutex_lock(&mHalTempMutex);
        mTempServiceStatus = false;
        SLIM_LOGD("Stopping Temp Read  \n");
      }
      break;
    default:
      sensor = SENSOR_TYPE_META_DATA;
  }
  /* Allow sensor update rate on the fly
   * Find sensor from available list of sensors
   * Configure Sensor
   * Activate Sensor
   * Trigger Sensor Buffer Events
   * Trigger Sensor Events
   */
  for (int i = 0; i < mSensorCount; i++) {
    // Find sensor from available list of sensors
    if ((NULL != mSensorList) && (sensor != SENSOR_TYPE_TEMPERATURE)
        && (mSensorList[i].type == sensor))
    {
      int outSamplingRate = 0;
      SLIM_LOGD("Action for '%s' (%d) is %s\n",
          mSensorList[i].name, sensor, ((uEnable==1)?"ENABLE":"DISABLE"));
      if ((1 == uEnable) && (wReportRateHz > 0))
      {
        int retVal = -1;
        bool odrConvertStatus = false;

        SLIM_LOGD("Setting Configuration for %s sensor_id %d  as RateHz %d val %d, %lld",
            mSensorList[i].name, mSensorList[i].sensor_id, wReportRateHz,
            (NS_IN_ONE_SECOND / samplingRate), slim_TimeTickGetMilliseconds());

        // Convert Sampling rate to Nearest supported sampling rate
        odrConvertStatus = getNearestODR(sensor, samplingRate, &outSamplingRate);
        SLIM_LOGD("SensorID:%d InODR:%d OutODR:%d Status:%d",
                        sensor, samplingRate, outSamplingRate, odrConvertStatus);

        // Configure Sensor
#if !defined(SCI_MAJOR_VERSION)
       retVal = sensorInterface->sensor_config(mSensorList[i].sensor_id,
            outSamplingRate, wSampleCount,
            std::bind(&SensorHalDaemonProvider::ConfigCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));

#else
        bool isRotationNeeded = false;
        retVal = sensorInterface->sensor_config(mSensorList[i].sensor_id,
             outSamplingRate, wSampleCount, isRotationNeeded,
             std::bind(&SensorHalDaemonProvider::ConfigCallbackWithRotation, this,
             std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
             std::placeholders::_4));
#endif
        if (0 == retVal)
        {
          SLIM_LOGD("Sensor Config Success :%d \n", retVal);
        }
        else
        {
          SLIM_LOGE("Sensor Configure failed :%d \n", retVal);
          return eSLIM_ERROR_INTERNAL;
        }
      }

      // Activate/Deactivate Sensor
      err = sensorInterface->sensor_control( mSensorList[i].sensor_id, (sensor_state)uEnable);
      SLIM_LOGD("Activate required in end:%d id: %d isBufferDataTriggered:%d \n",
            err, mSensorList[i].sensor_id, isBufferDataTriggered);

      if ((1 == uEnable) && (wReportRateHz > 0))
      {
        int retValbuffRead = -1;
        int retValRead = -1;

        // Send configuration change to client
        if (eSLIM_SERVICE_NONE  != eService) {
            m_zSensorInfoList[eService].samplingRate = outSamplingRate;
            routeConfigurationChange(s_qSupportedServices, m_zSensorInfoList);
        }
        // Buffer data trigger should be called only once combined togather for both sensors
        if (false == isBufferDataTriggered)
        {
          // Trigger Sensor Buffer Events
          retValbuffRead = sensorInterface->sensor_read_buffer_data(uEnable,
            std::bind(&SensorHalDaemonProvider::bufferReadCallBack,
              this, std::placeholders::_1, std::placeholders::_2));
          if (SENSOR_RESPONSE_SUCCESS != retValbuffRead)
          {
            setBufferState(COMPLETED_BUFFER_DATA);
          }
          else
          {
            isBufferDataTriggered = true;
          }
          SLIM_LOGD("Read Buffer retValbuffRead:%d id: %d uEnable:%d\n",
                                       retValbuffRead, mSensorList[i].sensor_id, uEnable);
        }
        // Trigger Sensor Live Events
        retValRead = sensorInterface->sensor_read_events(
                     mSensorList[i].sensor_id, std::bind(&SensorHalDaemonProvider::eventCallback,
                                                     this,
                                                     std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3));
        SLIM_LOGD("Read Live data retValRead :%d id: %d uEnable:%d\n",
                                              retValRead, mSensorList[i].sensor_id, uEnable);
        if (err != 0) {
          SLIM_LOGE("Action(%d) for '%s' (%d) failed (%s)\n",
              uEnable, mSensorList[i].name, sensor, strerror(-err));
          return eSLIM_ERROR_INTERNAL;
        }
        else {
          break;
        }
      }
    }
  }
  return eSLIM_SUCCESS;
}

/* In case of Disable Sensor Data clear all buffers
 *  Invoked by multiplexer
 */
void SensorHalDaemonProvider::cleanBatchingBuffers()
{
  SLIM_LOGD("cleanBatchingBuffers");
  MutexLock _l(s_zInstanceMutex);
  m_zAccSensorData.samples_len = 0;
  m_zGyroSensorData.samples_len = 0;
  m_bIsAccelPPSTimeNotReached = true;
  m_bIsGyroPPSTimeNotReached = true;
}

/* reads the batching config, cout based value and sync based.
 */
void SensorHalDaemonProvider::setBatchingConfig(void)
{
  loc_param_s_type z_sapParameterTable[] =
  {
    { "COUNT_BASED_BATCHING", &mCountBasedBatchingCfg, NULL, 'n' },
    { "SYNC_ONCE", &mSyncOnceCfg, NULL, 'n' },
  };

  /* Read Batching configuration and Time Sync Needed */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
  SLIM_LOGV("mCountBasedBatchingCfg = %d and mSyncOnceCfg = %d \n",
      mCountBasedBatchingCfg,
      mSyncOnceCfg);
}
