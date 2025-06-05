/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#ifndef QHCI_XM_INTF_H
#define QHCI_XM_INTF_H

#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <stdint.h>
#include <hidl/HidlSupport.h>
#include "hci_transport.h"
#include "qhci_main.h"
#include "qhci_packetizer.h"
#include "xpan_utils.h"


namespace xpan {
namespace implementation {

class QHciXmIntf
{
  public:
    QHciXmIntf();
    ~QHciXmIntf();
    void RemoteSupportXpanToXm(bdaddr_t bd_addr, bool enable);
    void PrepareAudioBearerReqToXm(bdaddr_t bd_addr,
                                             TransportType transport_type);
    void PrepareAudioBearerReqFromXm(bdaddr_t bd_addr, TransportType transport_type); 
    void PrepareAudioBearerRspToXm(bdaddr_t bd_addr, RspStatus status);
    void PrepareAudioBearerRspFromXm(bdaddr_t bd_addr, RspStatus status);
    bool UseCaseUpdateToXm(UseCaseType);
    void UseCaseUpdateConfirmFromXm(uint8_t usecase, bool status);
    void UnPrepareAudioBearerReqToXm(bdaddr_t bd_addr,
                                             TransportType transport_type);
    void UnPrepareAudioBearerRspFromXm(bdaddr_t bd_addr, bool status);
    void UnPrepareAudioBearerReqFromXm(bdaddr_t bd_addr, TransportType transport_type);
    void TransportEnabledFromXm(bdaddr_t bd_addr, TransportType type,
                                                bool status, uint8_t reason);
    void UpdateTransportFromXm(TransportType transport_type);
    void DelayReporting(uint32_t delay);
    void UpdateXpanBondedDevices(uint8_t num_devices, bdaddr_t *);
    void BearerSwitchInd(bdaddr_t addr, TransportType type, uint8_t status);
    void SendEncoderLimitToXm(uint8_t num_limits, uint8_t* data);
    void AudioTransportUpdateToXm(bdaddr_t addr, TransportType type);
    UseCaseType GetUsecaseTypeInfo();
    void ConnectLeLinkReqFromXm(bdaddr_t addr);
    void connectLELinkRspToXm(bdaddr_t addr, RspStatus status);
    bool isStreamingActive(bdaddr_t bd_addr);
    void UpdateWifiScanStarted(bdaddr_t bdaddr, uint8_t state);
};

} // namespace implementation
} // namespace xpan

#endif
