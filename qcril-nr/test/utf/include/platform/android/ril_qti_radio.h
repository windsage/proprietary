/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __RIL_QTI_RADIO_H_
#define __RIL_QTI_RADIO_H_ 1

#include <telephony/ril.h>

// For UTF only
// Same as RIL_REQUEST_START_NETWORK_SCAN
#define RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN 0xF001
// Same as RIL_QTI_RADIO_REQUEST_QUERY_FACILITY_LOCK
#define RIL_QTI_RADIO_REQUEST_QUERY_FACILITY_LOCK 0xF002

#define RIL_QTI_RADIO_UNSOL_NETWORK_SCAN_RESULT 0xE001

#endif /* __RIL_QTI_RADIO_H_ */
