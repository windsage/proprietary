/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "slim_client_iface.h"
#include "SlimClientIface.h"

using namespace slim;

SlimClientIface *SlimClientIface::mInstance = nullptr;

SlimClientIface * SlimClientIface::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new SlimClientIface();
    }
    return mInstance;
}

SlimClientIface::SlimClientIface() :
    ClientBase(),
    m_eClient((slim_ClientEnumType)0),
    mPCallbacks(nullptr),
    m_uAutoRestart(true),
    mClientContext(nullptr)
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
}

SlimClientIface::~SlimClientIface()
{
}

void SlimClientIface::destroy()
{
    if (nullptr != mInstance) {
        delete mInstance;
        mInstance = nullptr;
    }
}

void SlimClientIface::init(void *clientContext)
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
    mClientContext = clientContext;
    this->open(m_uAutoRestart);
}

void SlimClientIface::setCallbacks(const slimClientIfaceCbs *callbacks)
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
    mPCallbacks = callbacks;
}

void SlimClientIface::handleOpenResult
(
    slimErrorEnumT eError
)
{
    if (mPCallbacks && mPCallbacks->handleInitResultCb) {
        mPCallbacks->handleInitResultCb(eError, mClientContext);
    }
}

void SlimClientIface::handleServiceEnableResult
(
    slimServiceEnumT eService,
    void*            pUserCookie,
    slimErrorEnumT   eError
)
{
    if (mPCallbacks && mPCallbacks->handleServiceEnableResultCb) {
        mPCallbacks->handleServiceEnableResultCb(eService, pUserCookie, eError);
    }
}

void SlimClientIface::handleServiceError
(
    slimServiceEnumT eService,
    slimErrorEnumT   eError
)
{
    if (mPCallbacks && mPCallbacks->handleServiceErrorCb) {
        mPCallbacks->handleServiceErrorCb(eService, eError, mClientContext);
    }
}

void SlimClientIface::handleSensorData
(
    slimServiceEnumT eService,
    const slimSensorDataStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handleSensorDataCb) {
        mPCallbacks->handleSensorDataCb(eService, zData, mClientContext);
    }
}

void SlimClientIface::handleQmdData
(
    slimServiceEnumT eService,
    const slimQmdDataStructT &zData
)
{
    SLIM_LOGD("<<< QMD_DATA[%d]: TIME(%d)=%" PRIu64" motionState=%d\n",
            eService,
            zData.timeSource,
            zData.timestamp,
            zData.motionState);
}

void SlimClientIface::handlePedometerData
(
    slimServiceEnumT eService,
    const slimPedometerDataStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handlePedometerDataCb) {
        mPCallbacks->handlePedometerDataCb(eService, zData, mClientContext);
    }
}

void SlimClientIface::handleVehicleOdometryData
(
    slimServiceEnumT eService,
    const slimVehicleOdometryDataStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handleVehicleOdometryDataCb) {
        mPCallbacks->handleVehicleOdometryDataCb(eService, zData, mClientContext);
    }
}

void SlimClientIface::handleVehicleSensorData
(
    slimServiceEnumT eService,
    const slimVehicleSensorDataStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handleVehicleSensorDataCb) {
        mPCallbacks->handleVehicleSensorDataCb(eService, zData, mClientContext);
    }
}

void SlimClientIface::handleVehicleMotionData
(
    slimServiceEnumT eService,
    const slimVehicleMotionDataStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handleVehicleMotionDataCb) {
        mPCallbacks->handleVehicleMotionDataCb(eService, zData, mClientContext);
    }
}

void SlimClientIface::handleServiceStatus
(
    const slimServiceStatusEventStructT &zData
)
{
    if (mPCallbacks && mPCallbacks->handleServiceStatusCb) {
        mPCallbacks->handleServiceStatusCb(zData, mClientContext);
    }
}

void SlimClientIface::handleServiceProviderChange
(
    slimServiceEnumT eService,
    slimServiceProviderMaskT qRemoved,
    slimServiceProviderMaskT qAdded,
    slimServiceProviderMaskT qCurrent,
    slimSensorInfoStructT qSensorInfo
)
{
    if (mPCallbacks && mPCallbacks->handleServiceProviderChangeCb) {
        mPCallbacks->handleServiceProviderChangeCb(
                eService, qRemoved, qAdded, qCurrent, qSensorInfo, mClientContext);
    }
}

bool SlimClientIface::isServiceEnabled(slimServiceEnumT eService)
{
    return (eSERVICE_STATE_ERROR != getServiceState(eService)) ? true: false;
}

static bool isSensorServiceEnabled(slimServiceEnumT eService)
{
    return SlimClientIface::getInstance()->isServiceEnabled(eService);
}

static bool isServiceStateInTransition(slimServiceEnumT eService)
{
    return SlimClientIface::getInstance()->isServiceStateInTransition(eService);
}

static slimErrorEnumT enableSensorData
(
    const slimEnableSensorDataRequestStructT &zRequest,
    void *pUserCookie
)
{
    return SlimClientIface::getInstance()->enableSensorData(zRequest, pUserCookie);
}

static slimErrorEnumT enableVehicleData
(
    const slimEnableVehicleDataRequestStructT &zRequest,
    void *pUserCookie
)
{
    return SlimClientIface::getInstance()->enableVehicleData(zRequest, pUserCookie);
}

static slimErrorEnumT enablePedometer
(
    const slimEnablePedometerRequestStructT &zRequest,
    void *pUserCookie
)
{
    return SlimClientIface::getInstance()->enablePedometer(zRequest, pUserCookie);
}

static void init(void *clientContext)
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
    SlimClientIface::getInstance()->init(clientContext);
}

static void deInit()
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
    SlimClientIface::getInstance()->destroy();
}

static slimClientIface slimClientInterface = {
    init,
    isSensorServiceEnabled,
    isServiceStateInTransition,
    enableSensorData,
    enableVehicleData,
    enablePedometer,
    deInit,
    .reserve = {0}
};

SLIM_API const slimClientIface* get_slim_client_iface
(
    const slimClientIfaceCbs *callbacks
)
{
    SLIM_LOGD("[%s] Entry.\n", __func__);
    SlimClientIface::getInstance()->setCallbacks(callbacks);
    return &slimClientInterface;
}
