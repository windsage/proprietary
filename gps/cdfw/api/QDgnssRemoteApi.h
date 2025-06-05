/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  This file contains the remote interface definitions of the QTI DGNSS Correction
  Data framework.

  Copyright (c) 2019 - 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef QDGNSSREMOTEAPI_H
#define QDGNSSREMOTEAPI_H

#include "IQDgnss.h"

class QDgnssRemoteSource : public IQDgnssSource
{
public:
    /** @brief Create an instance of an IQDgnssSource object.
         An application shall create one source at a time.
         If the application changes its streaming source, then it
         shall release the old source and create a new source. Take
         for example, a source application that creates a source
         object, downloads RTCM messages from Ntrip Caster 1, and
         uses this source object to inject RTCM messages. If it
         connects to Ntrip Caster 2, the source application shall
         call unsubscribe() and createSource() rather than reuse the
         same source object to inject RTCM messages.
         Failure to follow the rule would affect the measurement and
         position accuracy which are calculated by the QC position
         engine.


        @param dataSource
        Tell the correction data framework who is going to inject the data
        @param dataFormat
        Tell the correction data framework the injected data format
        @param statusCallback
        The correction data framework updates the status to the application
        through this call back function.
        @param userName
        userName is an 8 byte long string which includes '\n',
        The correction data framework only accepts a userName which has
        been predefined in gps.conf
        @param dataPrecision
        Tell correction data framework the injected data precision.
        By default, data precision should be CDFW_PRECISION_RTK.

        @return IQDgnssSource pointer if success.
        nullptr if failed, the failure reason will be sent to the caller
        through the statusCallback, please check QDgnssSourceStatus in IQDgnss.h.

    */
    static shared_ptr<QDgnssRemoteSource> createSource(
        QDgnssDataSource dataSource,
        QDgnssInjectDataFormat dataFormat,
        QDgnssSourceStatusCb statusCallback,
        const char* userName,
        QDgnssInjectDataPrecision dataPrecision = CDFW_PRECISION_UNKNOWN);

    /** @brief Unsubscribe the IQDgnssSource object which was returned from
        createSource().
    */
    void unsubscribe();

    /** @brief Inject the raw correction data to the correction data framework.
        The correction data framework will parse the injected buffer into
        QDGNss messages.

        @param buffer
        The raw correction data message buffer
        @param bufferSize
        The buffer size

    */
    void injectCorrectionData(const uint8_t* buffer, uint32_t bufferSize) override;

    /** @brief Default destructor */
    virtual ~QDgnssRemoteSource() {}

protected:
    QDgnssRemoteSource() {}
    QDgnssRemoteSource(const QDgnssRemoteSource& source) = delete;
    QDgnssRemoteSource& operator=(const QDgnssRemoteSource& source) = delete;
};

class QDgnssRemoteListener : public IQDgnssListener
{
public:
    /** @brief QDgnssRemoteListener object receives QDgnss messages.

        @param dGnssMsg: DGnss message pointer
        The correction data framework sends QDgnss messages to the
        listener. The dGnssMsg is owned by the correction data
        framework. The listener must not use the dGnssMsg after
        onReceiveMsg returns.
        IQDgnssMsg is defined in IQDgnss.h.
    */
    virtual void onReceiveMsg(unique_ptr<IQDgnssMsg>& dGnssMsg) = 0;

    /** @brief Default destructor */
    virtual ~QDgnssRemoteListener();

    void reportDataUsage(const SourceDataUsageMsg& sourceDataUsage) override;

    virtual void updateListenerStatus(bool onListening);

protected:
    QDgnssRemoteListener(QDgnssMsgMask registeredMsgs, const char* userName);
    QDgnssRemoteListener(const QDgnssRemoteListener& listener) = delete;
    QDgnssRemoteListener& operator=(const QDgnssRemoteListener& listener) = delete;
};

#endif /* QDGNSSREMOTEAPI_H */
