/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef SLIM_CLIENT_IMPL_H
#define SLIM_CLIENT_IMPL_H

#include <slim_processor.h>
#include <slim_client_types.h>
#include <SlimClientBase.h>

using namespace slim;

class SlimClientIface : public ClientBase
{
private:
    slim_ClientEnumType   m_eClient;
    bool                  m_uAutoRestart;
    const slimClientIfaceCbs *mPCallbacks;
    static SlimClientIface *mInstance;
    SlimClientIface();
    ~SlimClientIface();
public:
    void *mClientContext;
    // singleton instance
    static SlimClientIface* getInstance();
    void init(void *clientContext);
    void setCallbacks(const slimClientIfaceCbs *callbacks);
    bool isServiceEnabled(slimServiceEnumT eService);
    void destroy();

protected:
    virtual void handleOpenResult
    (
        slimErrorEnumT eError
    );
    virtual void handleServiceEnableResult
    (
        slimServiceEnumT eService,
        void*            pUserCookie,
        slimErrorEnumT   eError
    );
    virtual void handleServiceError
    (
        slimServiceEnumT eService,
        slimErrorEnumT   eError
    );
    virtual void handleSensorData
    (
        slimServiceEnumT eService,
        const slimSensorDataStructT &zData
    );
    virtual void handleQmdData
    (
        slimServiceEnumT eService,
        const slimQmdDataStructT &zData
    );
    virtual void handlePedometerData
    (
        slimServiceEnumT eService,
        const slimPedometerDataStructT &zData
    );
    virtual void handleVehicleOdometryData
    (
        slimServiceEnumT eService,
        const slimVehicleOdometryDataStructT &zData
    );
    virtual void handleVehicleSensorData
    (
        slimServiceEnumT eService,
        const slimVehicleSensorDataStructT &zData
    );
    virtual void handleVehicleMotionData
    (
        slimServiceEnumT eService,
        const slimVehicleMotionDataStructT &zData
    );
    virtual void handleServiceStatus
    (
        const slimServiceStatusEventStructT &zData
    );
    virtual void handleServiceProviderChange
    (
        slimServiceEnumT eService,
        slimServiceProviderMaskT qRemoved,
        slimServiceProviderMaskT qAdded,
        slimServiceProviderMaskT qCurrent,
        slimSensorInfoStructT qSensorInfo
    );
};

#endif /* SLIM_CLIENT_IMPL_H */
