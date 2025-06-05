/**
 * Copyright (c) 2016 - 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * @file
 * @brief Sensor HAL provider interface file.
 *
 * This file declares Sensor HAL Daemon provider object.
 *
 * @ingroup slim_SensorHALDaemonProvider
 */
#ifndef SENSOR_HAL_DAEMON_PROVIDER_H_INCLUDED
#define SENSOR_HAL_DAEMON_PROVIDER_H_INCLUDED

#include <MultiplexingProvider.h>
#include <slim_api.h>
#include <stdio.h>
#include <list>
#include <string>
#include <loc_cfg.h>
#include <fstream>
#include <vector>
#include <SensorClientApi.h>

using sensor_client::SensorClient;

namespace slim
{

  /**
    @brief Sensor HAL provider class.

    SensorHALProvider class implements methods specific for Sensor on HAL
    connection.
    */
  class SensorHalDaemonProvider : public MultiplexingProvider
  {

    typedef enum {
      DELETE_BUFFER_DATA =0,
      INITIALIZE_SHD_PROVIDER=1
    }SHDIndicationType;
    /* SHD FullScale, vendor and sensor ODR Info */
    slimSensorInfoStructT m_zSensorInfoList[SLIM_SENSOR_SETTING_PROVIDER_MAX];

    /**< New SHD Event message ID */
#define IPC_MSG_SHD_INDICTION  (C_USR_MSG_ID_FIRST + 1)
    static const uint32_t s_qThreadId;
    static Mutex s_zInstanceMutex;
    static SensorHalDaemonProvider *s_pzInstance;
    static const slimAvailableServiceMaskT s_qSupportedServices;
    slimSensorDataStructT m_zSensorData;
    slimSensorDataStructT m_zAccSensorData;
    slimSensorDataStructT m_zGyroSensorData;
    uint64_t mtAccelDispatchTimestamp;
    uint64_t mtGyroDispatchTimestamp;
    int  mTempuS;
    bool mTempServiceStatus;
    slim_ThreadT m_zThread;
    Mutex m_zMutex;

#define BATCHSIZE_100HZ (10)
    enum BufferTxStatus
    {
      COMPLETED_BUFFER_DATA,
      KERNEL_BUFFER_DATA_INIT,
      KERNEL_BUFFER_DATA_IN_PROGRESS,
      SLIM_BUFFER_DATA_IN_PROGRESS,
    };
    slim_ThreadT m_zBuffThread;
    slimSensorDataStructT m_zAccBuffSensorData;
    slimSensorDataStructT m_zGyroBuffSensorData;
    BufferTxStatus mBufferTxStatus;
    std::list<slimSensorSampleStructT> mSecAccBufferList;
    std::list<slimSensorSampleStructT> mSecGyroBufferList;
    pthread_mutex_t mHalTempMutex;
    uint16_t mAccReportRateHz;
    uint16_t mAccSampleCount;
    uint16_t mGyroReportRateHz;
    uint16_t mGyroSampleCount;
    uint16_t mAcc100HzBatchCount;
    uint16_t mGyro100HzBatchCount;
    bool mAccServiceStatus;
    bool mGyroServiceStatus;
    bool m_bIsAccelPPSTimeNotReached;
    bool m_bIsGyroPPSTimeNotReached;
    slim_ThreadT m_zTempSensorThread;
    bool m_Enable;
    uint32_t        mCountBasedBatchingCfg;
    uint32_t        mSyncOnceCfg;
    ///@brief sensor API interface pointer
    SensorClient* sensorInterface;
    sensor_list *mSensorList;
    int mSensorCount;
    /* Flags if buffer data request is already triggered for any sensor
     * to avoid raising multiple request for each sensor
     * flag is cleared once buffere data delete requested
     */
    bool isBufferDataTriggered;

    /**
      @brief Main thread

      Sensor HAL provider Main handler thread.
      */
    static void threadMain(void *pData);

    /**
      @brief Class constructor

      Creates and initializes new instance of the class.
      */
    SensorHalDaemonProvider();

    /**
      @brief Class destructor

      Destroys current instance
      */
    ~SensorHalDaemonProvider();

    /**
      @brief Initialization function

      Sensor HAL provider Initialization functionality handler.
      */
    bool initialize();

    void sendIndication(SHDIndicationType pFrame);
    /**
      @brief Run Event Loop function

      Sensor HAL provider Buffer Data delete and initialization handler function.
      */
    bool runEventLoop();

    /**
      @brief Initiates time offset request.

      Function for making the time request. Successful response enable SLIM to
      calculate the offset between modem time and sensor time.

      @param[in] lTxnId Service transaction id.
      @return eSLIM_SUCCESS if time request is made successfully.
      */
    virtual slimErrorEnumT getTimeUpdate(int32_t lTxnId);

    private:

    /**
     * @brief Initiates the buffere data thread
     *
     * @return true on sucess.
     */
    bool memsBufferDataInit();

    /**
     * @brief Deinitiates the buffer data thread and closes open files
     */
    void memsBufferDataDeInit();

    void convert_event(const struct sensors_event_t *e, slimSensorSampleStructT *samples);

    /**
     * @brief Handler for Buffered data.
     *
     * Function for polling buffered data from sysfs interface
     * and send it for formatting and routing through slim core.
     *
     * @param[in] pData .
     *
     * @return void.
     */
    static void bufferedDataHandler(void *pData);

    /**
     * @brief baching and for formatting of buffered data.
     *
     * Function for converting buffered data to SLIM service sepecific format
     * Read queue until next EV_SYN , this will fill only newly received data
     * if some fields of sample are not received will maintain older data
     *
     * @param[in/out] zEventBuffer - input previous data, output new data,
     * if some fields are not received will hold previous data.
     * @param[in] fd - pointer to file descritor
     * @param[in] eSensorType - sensorType
     *
     * @return void.
     */
    bool getBufferedSample(slimServiceEnumT eSensorType, FILE* fd,
                           slimSensorSampleStructT *zEventBuffer);

    /**
     * @brief set buffere data state.
     *
     * @param[in] tate.
     *
     * @return void.
     */
    void setBufferState(BufferTxStatus state);

    /**
     * Scans the input device and stores the fhandle
     * for reading the buffer data from kernel
     *
     * @param[in] sensor buffer name.
     * @param[in] pointer to file descriptor.
     *
     * return val: SUCCESS -  0
     *               FAIL - -1
     */
    int snsBufDevOpen(char *s_name, FILE** fd);

    /* file descriptor for Accel event file bmi160_accbuf */
    FILE* mfdBuffAccel;
    /* file descriptor for Gyro event file bmi160_gyrobuf */
    FILE* mfdBuffGyro;
    /* file descriptor for Accel buffer file read_acc_boot_sample */
    FILE* mfdBuffAccelE;
    /* file descriptor for Gyro buffer file read_gyro_boot_sample */
    FILE* mfdBuffGyroE;

    /**
      @brief Converts sensor number to sensor name.

      @param[in] type - sensor number
      @return character pointer (name of sensor)
      */
    char const* getSensorName(int type);

    /**
      @brief Converts sensor type from sensor number to slim sensor enum number

      @param[in] sensorType - sensor number
      @return slimSensorTypeEnumT enum value
      */
    slimSensorTypeEnumT SensorTypeToSlimSensorType(int sensorType);

    /**
      @brief Converts slim sensor number to corresponding service

      @param[in] slimSensorTypeEnumT  slimSensorType- sensor enum value
      @return slimServiceEnumT service corresponding to the sensor number
      */
    slimServiceEnumT SlimSensorTypeToService(slimSensorTypeEnumT slimSensorType);

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
    virtual slimErrorEnumT doUpdateSensorStatus(bool uEnable,
                                                uint16_t wReportRateHz,
                                                uint16_t wSampleCount,
                                                slimServiceEnumT eService
                                               );

    void setBatchingConfig(void);

    /**
     * @brief clean the buffers used for Batching in case of Disable sensor data.
     *
     * @return void.
     */
    virtual void cleanBatchingBuffers();

    public:
    /*!
     * @brief Returns provider instance.
     *
     * Method provides access to provider instance. If necessary, the instance
     * is created and initialized.
     *
     * @return Provider instance.
     * @retval 0 On error.
     */
    static slim_ServiceProviderInterfaceType *getProvider();
    /**
     * @brief Temp Sensor data processing task.
     *
     * Function for processing buffered data from sysfs interface
     * and send it for formatting and routing through slim core.
     *
     * @return void.
     */
    void tempSensorDataPollTask();
    /**
     * @brief Temperature data member init
     *
     * @param[in].
     *
     * @return void.
     */
    bool tempSensorDataInit();
    /**
     * @brief Start Temp sensor task.
     *
     * Function for Start Temp sensor task
     *
     * @param[in] pData .
     *
     * @return void.
     */
    static void tempSensorDataHandler(void *pData);
    /**
     * @brief Finds location of fle.
     *
     *
     * @param[in] aeType - type of device, aeFtype- type of file,
     * aBuffPath- output,aLength- sizeof the output
     *
     * @return void.
     */
    void flushBuffer(slimServiceEnumT eService);
    /* callback by Sensor telling  the actual configuration.
    */
    void ConfigCallback(int handle, float sampling_rate, int batch_count);
    void ConfigCallbackWithRotation(int handle, float sampling_rate, int batch_count,
           bool rotation_needed);
    /**
     * @brief Start Temp sensor callback .
     *
     * Function for handling Temp sensor data
     *
     * @param[in] pData .
     *
     * @return void.
     */
    void temperatureCallback(float temperature);
    /**
     * @brief Buffered data processing task.
     *
     * Function for processing buffered data from sysfs interface
     * and send it for formatting and routing through slim core.
     *
     * @return void.
     */
    void bufferReadCallBack(const sensors_event_t *events, uint32_t count);
    /**
     * @brief api shared by SLIM to SENSOR once buffer data is read to delete it.
     *
     * @param[in] events = event info
     *            count  = num events
     *
     * @return void.
     */
    void bufferReadCallBack_delete(const sensors_event_t *events, uint32_t count);
    /**
     * @brief live data callback.
     *
     * @param[in] sensor_id = type
     *            events = data
     *            count = num data
     *
     * @return void.
     */
    void eventCallback(int sensor_id, const sensors_event_t *events, uint32_t count);
    /**
     * @brief get available sensors mask from list.
     *
     * Function to convert sensor list to sensor available mask
     *
     * @param[in] senList.
     *
     * @return slimAvailableServiceMaskT.
     */
    slimAvailableServiceMaskT getAvailableSensorsMask(sensor_list *senList);

    /**
     * @brief live Capability callback.
     *
     * @param[in] mask indicates current state
     *
     * @return void.
     */
    void onCapabilitiesCb(SensorCapabilitiesMask mask);

    /**
     * @brief Find nearest supported ODR.
     *
     * @param[in] sensor   Sensor number
     * @param[in] inODR    Input ODR requested
     * @param[out] outODR  Output ODR supported by SHD
     *
     * @return bool true if nearest found else false if not found in list
     */
    bool getNearestODR(int sensor, int inODR, int *outODR);
  };
}

#endif /* SENSOR_HAL_DAEMON_PROVIDER_H_INCLUDED */
