/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hidl_impl/1.1/radio_service_utils_1_1.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

RIL_Errno fillNetworkScanRequest_1_1(const V1_1::NetworkScanRequest& request,
                                     RIL_NetworkScanRequest& scanRequest) {
  if (request.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
    return RIL_E_INVALID_ARGUMENTS;
  }

  if (request.type == V1_1::ScanType::ONE_SHOT) {
    scanRequest.type = RIL_ONE_SHOT;
  } else if (request.type == V1_1::ScanType::PERIODIC) {
    scanRequest.type = RIL_PERIODIC;
  }
  scanRequest.interval = request.interval;
  scanRequest.specifiers_length = request.specifiers.size();

  for (size_t i = 0; i < request.specifiers.size(); ++i) {
    if (request.specifiers[i].geranBands.size() > MAX_BANDS ||
        request.specifiers[i].utranBands.size() > MAX_BANDS ||
        request.specifiers[i].eutranBands.size() > MAX_BANDS ||
        request.specifiers[i].channels.size() > MAX_CHANNELS) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    const V1_1::RadioAccessSpecifier& ras_from = request.specifiers[i];
    RIL_RadioAccessSpecifier& ras_to = scanRequest.specifiers[i];

    ras_to.radio_access_network = static_cast<RIL_RadioAccessNetworks>(ras_from.radioAccessNetwork);
    ras_to.channels_length = ras_from.channels.size();

    std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
    const std::vector<uint32_t>* bands = nullptr;
    switch (request.specifiers[i].radioAccessNetwork) {
      case V1_1::RadioAccessNetworks::GERAN:
        ras_to.bands_length = ras_from.geranBands.size();
        bands = (std::vector<uint32_t>*)&ras_from.geranBands;
        break;
      case V1_1::RadioAccessNetworks::UTRAN:
        ras_to.bands_length = ras_from.utranBands.size();
        bands = (std::vector<uint32_t>*)&ras_from.utranBands;
        break;
      case V1_1::RadioAccessNetworks::EUTRAN:
        ras_to.bands_length = ras_from.eutranBands.size();
        bands = (std::vector<uint32_t>*)&ras_from.eutranBands;
        break;
      default:
        return RIL_E_INVALID_ARGUMENTS;
    }
    // safe to copy to geran_bands because it's a union member
    for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
      ras_to.bands.geran_bands[idx] = (RIL_GeranBands)(*bands)[idx];
    }
  }

  return RIL_E_SUCCESS;
}

}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android
