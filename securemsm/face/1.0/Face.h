/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <aidl/android/hardware/biometrics/face/BnFace.h>

namespace aidl::android::hardware::biometrics::face
{
class Face : public BnFace
{
   public:
    ndk::ScopedAStatus getSensorProps(
        std::vector<SensorProps>* _aidl_return) override;

    ndk::ScopedAStatus createSession(
        int32_t sensorId, int32_t userId,
        const std::shared_ptr<ISessionCallback>& cb,
        std::shared_ptr<ISession>* _aidl_return) override;
};

}  // namespace aidl::android::hardware::biometrics::face
