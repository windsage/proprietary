/**
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "Face.h"
#include "Session.h"

namespace aidl::android::hardware::biometrics::face
{
const int kSensorId = 4;
const common::SensorStrength kSensorStrength = common::SensorStrength::STRONG;
const int kMaxEnrollmentsPerUser = 5;
const FaceSensorType kSensorType = FaceSensorType::RGB;
const bool kHalControlsPreview = true;
const std::string kHwComponentId = "faceSensor";
const std::string kHardwareVersion = "vendor/model/revision";
const std::string kFirmwareVersion = "1.01";
const std::string kSerialNumber = "00000001";
const std::string kSwComponentId = "matchingAlgorithm";
const std::string kSoftwareVersion = "vendor/version/revision";

ndk::ScopedAStatus Face::getSensorProps(std::vector<SensorProps>* return_val)
{
    ALOGI("%s in", __func__);

    common::ComponentInfo hw_component_info;
    hw_component_info.componentId = kHwComponentId;
    hw_component_info.hardwareVersion = kHardwareVersion;
    hw_component_info.firmwareVersion = kFirmwareVersion;
    hw_component_info.serialNumber = kSerialNumber;
    hw_component_info.softwareVersion = "";

    common::ComponentInfo sw_component_info;
    sw_component_info.componentId = kSwComponentId;
    sw_component_info.hardwareVersion = "";
    sw_component_info.firmwareVersion = "";
    sw_component_info.serialNumber = "";
    sw_component_info.softwareVersion = kSoftwareVersion;

    common::CommonProps commonProps;
    commonProps.sensorId = kSensorId;
    commonProps.sensorStrength = kSensorStrength;
    commonProps.maxEnrollmentsPerUser = kMaxEnrollmentsPerUser;
    commonProps.componentInfo = {std::move(hw_component_info),
                                 std::move(sw_component_info)};

    SensorProps props;
    props.commonProps = std::move(commonProps);
    props.sensorType = kSensorType;
    props.halControlsPreview = kHalControlsPreview;
    props.enrollPreviewWidth = 1080;
    props.enrollPreviewHeight = 1920;
    props.enrollTranslationX = 100.f;
    props.enrollTranslationY = 50.f;
    props.enrollPreviewScale = 1.f;

    *return_val = {std::move(props)};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Face::createSession(
    int32_t sensorId, int32_t userId,
    const std::shared_ptr<ISessionCallback>& cb,
    std::shared_ptr<ISession>* return_val)
{
    ALOGI("%s in", __func__);
    ALOGI("%s - the sensorId is 0x%" PRIx32, __func__, sensorId);
    ALOGI("%s - the userId is 0x%" PRIx32, __func__, userId);

    *return_val = SharedRefBase::make<Session>(cb);
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::biometrics::face
