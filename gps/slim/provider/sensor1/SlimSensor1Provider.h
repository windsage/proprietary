/*
Copyright (c) 2011, 2014-2015, 2020-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2012 Qualcomm Atheros, Inc. All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/
/**
@file
@brief GNSS / Sensor Interface Framework Support

This file defines the interface to accessing sensor data through SensorProtobuf API.

@ingroup slim_SensorProtobufProvider
*/


#ifndef __SLIM_SENSOR1_PROVIDER_H__
#define __SLIM_SENSOR1_PROVIDER_H__

#include <MultiplexingProvider.h>
#include "SlimSscUtils.h"

//! @addtogroup slim_SensorProtobufProvider
//! @{

namespace slim
{

typedef enum {
  SPB_TIME_BASED_BATCHING,
  SPB_FIXED_COUNT_BASED_BATCHING,
  SPB_VARIABLE_COUNT_BASED_BATCHING
} eSensorProtobufBatchCfgType;

typedef struct ProtoEvent_t
{
  uint8_t *data;
  size_t size;
}ProtoBufEvent;

#define SLIMPB_QTIMER_BOOTTIME_OFFSET_CAL_TIMER_MSEC         (30000)
#define SLIMPB_QMI_RECONNECT_TIMER_MSEC                      (5000)
#define SLIMPB_SUID_CB_TIMEOUT_MSEC                          (10000)

enum {
  eIPC_MSG_TIMER = M_IPC_MSG_TIMER,
  /**< Timer message ID */

  ePROVIDER_IPC_MSG_ID_REOPEN = C_USR_MSG_ID_FIRST + 1,
  ePROVIDER_IPC_MSG_ID_ENABLE_SERVICE,
  ePROVIDER_IPC_MSG_NEW_SENSORPROTOBUF_EVENT,
  ePROVIDER_IPC_MSG_ROUTE_CONFIG_CHANGE
};

/*
    Supported Sensor list
    add new string and enum for new sensor addition
    Modify the SPB_MAX_SERV based on sensor addition
*/
#define SPB_MAX_SERV 3

#define SPB_SENSOR_ACCEL_STR             "accel"
#define SPB_SENSOR_GYRO_STR              "gyro"
#define SPB_SENSOR_TEMPERATURE_STR       "sensor_temperature"

enum eSensorList{
  eSPB_SensorACCEL = 0,
  eSPB_SensorGYRO = 1,
  eSPB_SensorTEMP = 2,
  eSPB_SensorMax = SPB_MAX_SERV
};

/*!
 * @brief SensorProtobuf Provider implementation
 *
 * SensorProtobuf Provider enables access of SensorProtobuf services (ADSP over SSC) on APSS.
 * This provider is a functional equivalent of SSC Provider on modem.
 *
 * @ingroup slim_SensorProtobufProvider
 */
class SensorProtobufProvider :
  public MultiplexingProvider
{
  enum {
  eTIMER_QTIMER_OFFSET_CAL_TIMER,
  eTIMER_QTIMER_QMI_RECONNECT_TIMER,
  eTIMER_SUID_CB_EXPIRY_TIMER
};

  struct SensorState
  {
     bool                          running;            /* Is reporting currently? */
     uint32_t                      reporting_rate;     /* Current sampling rate in Hz */
     uint32_t                      batching_rate;      /* Current batching rate in Hz */
     float                         reporting_interval; /* Current sampling interval in ms */
     float                         batching_interval;  /* Current batching interval in ms */

     SensorState() :
       running(false),
       reporting_rate(0),
       batching_rate(0),
       reporting_interval(0),
       batching_interval(0)
     {
     }
  };

  static Mutex s_zInstanceMutex;
  static SensorProtobufProvider *s_pzInstance;

  slim_ThreadT      m_zThread;
  Mutex             m_zMutex;

  SensorState       m_zGyroState;        /* All necessary state for accel sensor */
  SensorState       m_zAccelState;       /* All necessary state for gyro sensor */
  SensorState       m_zGyroTempState;    /* All necessary state for gyro temperature sensor */
  SensorState       m_zAccelTempState;   /* All necessary state for accel temperature sensor */
  volatile int      m_eOpenResult;

  bool              m_uBatchMode;
private:
  uint32_t          m_syncOnceCfg;
  uint32_t          m_countBasedBatchingCfg;
  uint16_t          m_acc100HzBatchCount;
  uint16_t          m_gyro100HzBatchCount;
  uint16_t          m_temp100HzBatchCount;

  /* memory for sensor data */
  slimSensorDataStructT m_zAccSensorData;
  slimSensorDataStructT m_zGyroSensorData;
  slimSensorDataStructT m_zTempSensorData;

  /* Qtimer related */
  slim_TimerT      *mQtimerOffsetTimer;
  slim_TimerT      *mQMIReconnectTimer;

  /* suid_cb expiry timer */
  slim_TimerT      *mSuidCbExpiryTimer;

  bool              mQMIConnectionPending;

  /* reference to suid_lookup */
  suid_lookup       *lookUpSuid;
  /**
   @brief Thread main function

   @param[in] pData User data pointer.
   */
  static void threadMain
  (
    void *pData
  );
  /**
  @brief Runloop to handle Events

  Function performs event handling of sensor data.
  */
  void runEventLoop();
  /**
  @brief Constructs object

  Function performs partial initialization of object state.
  */
  SensorProtobufProvider();
  /**
  @brief Releases object data
  */
  ~SensorProtobufProvider();

  /**
  @brief Opens SensorProtobuf connection.

  Opens connection to SensorProtobuf API. The result can be one of the following:
  - Success
  - Partial success
  - Error

  @return true if result is success or partial success; false otherwise.
  */
  bool open();

  /**
  @brief Closes SensorProtobuf connection.

  Closes connection to SensorProtobuf API.

  @return true if operation is successful; false otherwise.
  */
  bool close();
  /**
  @brief Initializes internal structures for sensor provider.

  The method prepares provider for operation.
  @return true if operation is successful; false otherwise.
  */
  bool init();
  bool startQMIReconnectTimer();
  void restoreServiceStates();
protected:
  void SensorProtobufProcessSensorData(const uint8_t *data, size_t size);

  /**
  @brief start timer to Qtimer offset estimate

  Function starts timer which will regulary estimate offset
  to Qtimer and APSS timeStamp.
  */
  bool startQtimerOffsetCalTimer();

  /**
  @brief Baching configuration function

  Function handles batching configuration of sensor data.
  */
  void setBatchingConfig(void);

  /**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );

  /**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using SensorProtobuf Buffering
  API.

  @param[in] uEnable       Flag that indicates if the service shall be enabled
                           or disabled.
  @param[in] wReportRateHz Requested reporting rate in Hz.
  @param[in] wSampleCount  Requested sample count in one batch.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );
  /**
  @brief Handler for messages targeted for provider.

  Function handles messages sent via SLIM core to provider.

  @param[in] qMessageId      Message id.
  @param[in] qMsgPayloadSize Message size.
  @param[in] pMsgPayload     Pointer to the message payload.
  */
  virtual void handleProviderMessage
  (
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
  );
public:
  /**
  @brief Returns SLIM provider interface

  Method constructs provider instance, initializes it, and returns SLIM
  provider interface.

  @return SLIM provider interface of 0 on error.
  */
  static slim_ServiceProviderInterfaceType *getServiceProvider();
  /**
  @brief Returns SLIM interface

  The method initializes singleton instance and returns SLIM provider
  interface.

  @return Provider object
  */
  static SensorProtobufProvider *getInstance();

  /**
  @brief Configures batch mode support

  @param[in] uFlag Flag to enable (true) or disable batch mode.
   */
  void setBatchMode
  (
    bool uFlag
  );
};

}

//! @}

#endif /* __SLIM_SENSOR1_PROVIDER_H__ */
