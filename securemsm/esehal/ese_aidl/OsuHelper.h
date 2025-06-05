/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define BASIC_CHANNEL_OP     1
#define LOGICAL_CHANNEL_OP   2
#define TRANSMIT_OP          3

bool isOSUMode(uint8_t operation, const std::vector<uint8_t>& data);
TEEC_Result handleNxpOsuOpenBasicChannel(const std::vector<uint8_t>& aid, int p2, std::vector<uint8_t>* aidl_return);
TEEC_Result handleNxpOsuTransmit(const std::vector<uint8_t>& data, std::vector<uint8_t>* aidl_return);
