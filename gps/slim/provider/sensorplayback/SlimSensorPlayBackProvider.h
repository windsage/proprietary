/*
   Copyright (c) 2019-2020 , 2023 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
   */
/**
  @file
  @brief Sensor PlayBack provider interface file.

  This file declares Sensor PlayBack provider object.

  @ingroup slim_SensorPlayBackProvider
  */
#ifndef SLIM_SENSOR_PLAYBACK_H_INCLUDED
#define SLIM_SENSOR_PLAYBACK_H_INCLUDED

#include <MultiplexingProvider.h>
#include <slim_api.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <loc_cfg.h>
#include <inttypes.h>
#include <slim_os_log_api.h>
#include <slim_os_api.h>
#include <loc_cfg.h>


#include <linux/input.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>
#include <fstream>
#include <dlfcn.h>
#include <loc_pla.h>
#include <loc_cfg.h>
#include "diag_lsm.h"
#include "EnginePluginImpl.h"
#include "EnginePluginLogImpl.h"

#include <utils/SystemClock.h>
#include <utils/Timers.h>
#include <signal.h>

void* GetDCIInitPtr();
void* GetCreateTimerPtr();
void* GetGpsTimeRxdPtr();
void* GetDCIDeInitPtr();
void* GetTimerOffsetMutexPtr();
void* GetReadPtr();
void* GetRetrievePtr();
void* GetSessionStartedPtr();
void* GetTimerIdPtr();
void* GetPlayBackTimerMutexPtr();
void* GetPlayBackTimerCondPtr();
void* GetTimerElapsedPtr();
typedef void*(*getsymptr)();

namespace slim {

  typedef PACKED struct PACKED_POST
  {
    uint16 len; /* Specifies the length, in bytes of the
                   entry, including this header. */
    uint16 code; /* Specifies the log code for the entry as
                    enumerated above. Note: This is
                    specified as word to guarantee size. */
    // removed AMSS specific code
    //qword ts;     The system timestamp for the log entry. The
    /*upper 48 bits represent elapsed time since
      6 Jan 1980 00:00:00 in 1.25 ms units. The
      low order 16 bits represent elapsed time
      since the last 1.25 ms tick in 1/32 chip
      units (this 16 bit counter wraps at the
      value 49152). */
    uint32 ts_lo; /* Time stamp */
    uint32 ts_hi;
  }log_hdr_type;

  /**
    @brief Sensor PlayBack provider class.

    SensorPlayBackProvider class implements methods specific for Sensor on PlayBack
    connection.
    */

  typedef uint16_t epDiagGnssConstellationTypeMask;
  class SensorPlayBackProvider: public MultiplexingProvider {

    static volatile boolean timerElapsed;
    /* Buffer will use the Sensor info in this struct format  */
    struct SensorData {
      unsigned int GpsMsec;
      unsigned int RTCMs;
      unsigned int SensorType;
      double SensorXaxis;
      double SensorYaxis;
      double SensorZaxis;
      SensorData(unsigned int GpsMsecKey) :
        GpsMsec(GpsMsecKey) {
        }

      bool operator ()(const SensorData& m) const {
        return GpsMsec <= m.GpsMsec;
      }

      bool operator <(const SensorData& m) const {
        return GpsMsec < m.GpsMsec;
      }
    };

    /* Singleton static object and its associates */
    static SensorPlayBackProvider *s_pzInstance;
    static Mutex s_zInstanceMutex;
    static const uint32_t s_qThreadId;
    static const slimAvailableServiceMaskT s_qSupportedServices;

    /* Other static objects */
    /* Thread ID for Sensor PlackBack File Read */
    static const uint32_t s_qFileReadThreadId;
    /* When Timer elapses it will signal this condition */
    static pthread_cond_t s_zPlayBackTimerCond;
    static pthread_mutex_t s_zPlayBackTimerMutex;
    /* GPS2QTimerOffset protector Mutex */
    static pthread_mutex_t s_zGPS2APSSTimerOffsetMutex;
    /* Timer for Senor batch interval */
    static timer_t s_ztimer_id;
    /* Senor batch interval */
    static long mTimer_Interval;
    /* Senor Look Ahead Buffer Size */
    static unsigned long Buffer_Size;
    /* Indicator for Session start */
    static uint8 s_uSession_Started;
    /* Indicator for Valid GPS Time */
    static boolean s_bValidGpsTimeRxd;
    /* Indicator for Valid GPS Time */
    static long long s_qGPS2APSSTimerOffset;
    /* Buffer to keep GpsToQTimer Offsets before Calibration */
    static std::vector<unsigned int> m_zGpsToQTimerVector;

    struct HalLibInfo
    {
      uint8_t sensorType;
      uint8_t sensorHalLibPathStrSet;
      /* As this is read using loc cfg routine, the buffer size
         need to conform to what is supported by loc cfg module. */
      char    sensorHalLibPath[LOC_MAX_PARAM_STRING];
    };

    static HalLibInfo m_zLibInfo;

    /* Main Thread responsible to send the Sensor Packets */
    slim_ThreadT m_zThread;
    /* Sensor PlayBack File read Thread */
    slim_ThreadT m_zFileReadThread;
    /*! @brief Mutex for atomic operations */
    mutable Mutex m_zLock;

    /* When Enable Sensor data request comes from clients it will signal this condition */
    pthread_cond_t m_zPlayBackStartCond;
    pthread_mutex_t m_zPlayBackStartMutex;

    pthread_mutex_t m_zPlayBackStartMutex2;
    /* From Main thread when the Buffer does not have data or Buffer is not full,
     * it will signal this condition */
    pthread_cond_t m_zPlayBackFileReadCond;
    pthread_mutex_t m_zPlayBackFileReadMutex;

    /* Only in case of Lookup fails from the buffer this is needed.
       In that case after finding the data from file, File Read thread will signal this condition */

    /* Single Look ahead Buffer for all Sensor data */
    std::vector<SensorData> m_zSensorDataVector;

    /* Temporary SensorDat structs which will be accepted by RouteIndication Function */
    slimSensorDataStructT m_zAccSensorData;
    slimSensorDataStructT m_zGyroSensorData;
    slimSensorDataStructT m_zMagSensorData;
    /*  DIAG_PROC ID */
    int mDci_proc;
    /* Diag Client ID  */
    int mClient_id;
    /*  Key GPS Time to search from File */
    unsigned int m_qKeyGpsMsec = 0;
    /*  Flags to keep the Senor is enabled or not */
    boolean m_bACCELService = FALSE;
    boolean m_bGYROService = FALSE;
    boolean m_bMAGService = FALSE;
    /*  Target Latency Configured  */
    unsigned int m_qTargetLatency = 0;
    /*  Inject Latency Configured  */
    unsigned int m_qInjectLatency = 0;
    /*  Senor Play Back File name  */
    char mqSensorPlayBackFile[LOC_MAX_PARAM_STRING];
    /*  Indicator to search from the Senor Play Back File */
    boolean m_bSearchFile;
    /*  Indicator to tell Time not found in the Senor Play Back File */
    boolean m_bNotFoundInFile = FALSE;
    FILE * fPTR;
    /*  Indicator to tell LowerLimit should be calculated again */
    boolean m_bSearchLowerLimit = TRUE;
    boolean mfileReadCondWait = FALSE;
    static unsigned int m_ProcessClkCntr;

    private:
    /*! Sensor PlayBack provider main thread function */
    static void threadMain(void *pData);

    /*! Sensor Plackback Record File Read thread function */
    static void FileReadthreadMain(void *pData);

    /**
      @brief Class constructor

      Creates and initializes new instance of the class.

*/
    SensorPlayBackProvider();

    /**
      @brief Class destructor

      Destroys current instance

*/
    ~SensorPlayBackProvider();

    /**
      @brief Class initializor

      initializes the Singleton object of SensorPlayBackProvider

*/
    bool initialize();

    /**
      @brief Main Thread function

      Responsible to send the Sensor Packets

*/
    void runEventLoop();

    /**
      @brief File Thread function

      Responsible to fill the Look ahead buffer

*/
    void runFileReadLoop();

    bool readDiscardHeading(FILE *fPTR);

    /**
      @brief Initiates time offset request.

      Function for making the time request. Successful response enable SLIM to
      calculate the offset between modem time and sensor time.

      @param[in] lTxnId Service transaction id.
      @return eSLIM_SUCCESS if time request is made successfully.
      */
    //virtual slimErrorEnumT getTimeUpdate(int32_t lTxnId);

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

      @param[in] uEnable       Flag that indicates if the service sPlayBackl be enabled
      or disabled.
      @param[in] wReportRateHz Requested reporting rate in Hz.
      @param[in] wSampleCount  Requested sample count in one batch.
      @param[in] eService      Service to control.

      @return eSLIM_SUCCESS is operation succeeded.
      */
    virtual slimErrorEnumT doUpdateSensorStatus(bool uEnable,
        uint16_t wReportRateHz, uint16_t wSampleCount,
        slimServiceEnumT eService);

    /**
      @brief Diag stream register function

Diag_Stream_Config: Set log & event masks

@param[in] client_id      client ID

@return eSLIM_SUCCESS is operation succeeded.
*/
    int Diag_Stream_Config(int mClient_id);

    /**
      @brief Diag call back function for Clock report

      Call back function to decode the 0x1478 Clock report

      @param[in] pRaw      Raw Byte array

*/
    static void processClkRptLog(unsigned char *pRaw);

    static void processGPS2APSSClkLog(unsigned char *pRaw);
    /**
      @brief Diag call back function for set of Logs

      Call back function to Process DCI log stream

      @param[in] ptr      Raw Byte array
      @param[in] len      Raw Byte array length

*/
    static void process_dci_log_stream(unsigned char *ptr, int len);

    /**
      @brief Diag call back function for set of events

      Call back function to Process DCI events stream

      @param[in] ptr      Raw Byte array
      @param[in] len      Raw Byte array length

*/
    static void process_dci_event_stream(unsigned char *ptr, int len);

    /**
      @brief Diag Initializer

      Diag Initializing function

      @param[out] client_id      DCI client ID

      @return eSLIM_SUCCESS is operation succeeded.
      */
    slimErrorEnumT Diag_DCI_Init(int *pmClient_id);

    /**
      @brief Diag De-Initializer

      Diag De-Initializing function

      @param[in] client_id      client ID

      @return eSLIM_SUCCESS is operation succeeded.
      */
    slimErrorEnumT Diag_DCI_DeInit(int mClient_id);

    /**
      @brief Time Converter

      Convert Time from CDMA Time format to UTC Ms
      Start from 1970/1/1 0:0:0

      @param[in] High and Low DWORDS

      @return Time in Ms.
      */
    static double CDMATimeToUTCMs(uint32 ts_hi, uint32 ts_low);

    /**
      @brief Time Converter

      Convert Time from CDMA Time format to Ms
      Start from 1970/1/1 0:0:0

      @param[in] High and Low DWORDS

      @return Time in Ms.
      */

    static double CDMATimeToMs(uint32 ts_hi, uint32 ts_low);

    /**
      @brief Notify Function Handler

      Gets called when there is a change in the channel connection

      @param[in] signal ID
      @param[in] signal Info

*/
    static void notify_handler(int signal, siginfo_t *info, void* /*unused*/
        );

    /**
      @brief Timer Creation function

      Responsible to create the Timer for Sensor sampling.

*/
    static void create_timer(const long& Timer_Interval);

    /**
      @brief Timer Creation function

      Responsible to create the Timer for Sensor sampling.

*/
    static void set_timer(const long& Timer_Interval);

    /**
      @brief Timer Thread main function

      Responsible to signal Main thread to prepare and send sensor data

*/
    static void timer_thread(union sigval arg);

    /**
      @brief Main Thread subroutine function

      Responsible to fill the SensorData and Invoke the routeIndication Function

*/
    void SendSensorData(slimSensorDataStructT& SensorData, uint8 sensorType);

    /**
      @brief Main Thread subroutine function

      Responsible to get sensor data from the Look ahead buffer

*/
    void RetrieveSensorData();

    uint32_t calcMsTimeDiffTick(uint64_t tTs1Ms, uint64_t tTs2Ms);

/**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.
*/
  virtual slimErrorEnumT getTimeUpdate(int32_t lTxnId);

public:
  typedef slimErrorEnumT (SensorPlayBackProvider::*fptr)(int*);
  fptr DCIInitPtr;
typedef void (*fptr1)(const long& Timer_Interval);
  fptr1 create_timerPtr;
typedef volatile uint8 *VPtr1;
 VPtr1 SessionStartedPtr;
typedef volatile boolean *VPtr2;
 VPtr2 ValidGpsTimeRxdPtr;
 VPtr2 TimerElapsedPtr;

typedef slimErrorEnumT (SensorPlayBackProvider::*fptr2)(int mClient_id);
 fptr2 DCIDeInitPtr;

typedef pthread_mutex_t *VPtr3;
VPtr3 TimerOffsetMutexPtr;

typedef void (*fptr3)(void*);
fptr3 ReadDataPtr;

typedef void(SensorPlayBackProvider::*fptr4)(void);
fptr4 RetrieveData;

typedef volatile timer_t* VPtr4;
VPtr4 TimerIdPtr;

VPtr3 PlaybackTimeMutexPtr;
typedef pthread_cond_t* VPtr5;
VPtr5 PlayBackTimerCondPtr;
friend void* ::GetDCIInitPtr();
friend void* ::GetCreateTimerPtr();
friend void* ::GetGpsTimeRxdPtr();
friend void* ::GetDCIDeInitPtr();
friend void* ::GetTimerOffsetMutexPtr();
friend void* ::GetReadPtr();
friend void* ::GetRetrievePtr();
friend void* ::GetSessionStartedPtr();
friend void* ::GetTimerIdPtr();
friend void* ::GetPlayBackTimerMutexPtr();
friend void* ::GetPlayBackTimerCondPtr();
friend void* ::GetTimerElapsedPtr();
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
  };


}

#endif /* SLIM_SENSOR_PLAYBACK_H_INCLUDED */
