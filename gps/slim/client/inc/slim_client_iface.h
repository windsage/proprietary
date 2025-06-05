/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef SLIM_CLIENT_IFACE_H
#define SLIM_CLIENT_IFACE_H

#include <slim_client_types.h>

typedef struct {
    void (* handleInitResultCb)(
            slimErrorEnumT eError,
            void* pUserCookie);
    void (* handleServiceEnableResultCb)(
            slimServiceEnumT eService,
            void* pUserCookie,
            slimErrorEnumT   eError);
    void (* handleServiceErrorCb)(
            slimServiceEnumT eService,
            slimErrorEnumT eError,
            void* pUserCookie);
    void (* handleServiceStatusCb)(
            const slimServiceStatusEventStructT &zData,
            void* pUserCookie);
    void (* handleServiceProviderChangeCb)(
            slimServiceEnumT eService,
            slimServiceProviderMaskT qRemoved,
            slimServiceProviderMaskT qAdded,
            slimServiceProviderMaskT qCurrent,
            slimSensorInfoStructT qSensorInfo,
            void* pUserCookie);
    void (* handleSensorDataCb)(slimServiceEnumT eService,
            const slimSensorDataStructT &zData,
            void* pUserCookie);
    void (* handlePedometerDataCb)(
            slimServiceEnumT eService,
            const slimPedometerDataStructT &zData,
            void* pUserCookie);
    void (* handleVehicleOdometryDataCb)(
            slimServiceEnumT eService,
            const slimVehicleOdometryDataStructT &zData,
            void* pUserCookie);
    void (* handleVehicleSensorDataCb)(
            slimServiceEnumT eService,
            const slimVehicleSensorDataStructT &zData,
            void* pUserCookie);
    void (* handleVehicleMotionDataCb)(slimServiceEnumT eService,
            const slimVehicleMotionDataStructT &zData,
            void* pUserCookie);
    uint8_t reserve[44];
} slimClientIfaceCbs;

typedef struct {
    void (* init)(void *clientContext);
    bool (* isServiceEnabled)(slimServiceEnumT eService);
    bool (* isServiceStateInTransition)(slimServiceEnumT eService);
    slimErrorEnumT (* enableSensorData)(
            const slimEnableSensorDataRequestStructT &zRequest,
            void *pUserCookie);
    slimErrorEnumT (* enableVehicleData)(
            const slimEnableVehicleDataRequestStructT &zRequest,
            void *pUserCookie);
    slimErrorEnumT (* enablePedometer) (
        const slimEnablePedometerRequestStructT &zRequest,
        void *pUserCookie);
    void (* deInit)();
    uint8_t reserve[44];
} slimClientIface;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const slimClientIface* get_slim_client_iface
(
    const slimClientIfaceCbs *callbacks
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SLIM_CLIENT_IFACE_H */
