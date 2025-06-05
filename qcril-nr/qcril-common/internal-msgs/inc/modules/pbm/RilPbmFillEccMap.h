/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef RIL_FILL_ECC_MAP
#define RIL_FILL_ECC_MAP


#include <memory>
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "interfaces/nas/nas_types.h"

/**
 * Message to fill ECC Map with emergency numbers, mcc and mnc.
 */

class RilPbmFillEccMap : public SolicitedSyncMessage<bool>,public add_message_id<RilPbmFillEccMap> {
    private:
        char* ecc_numbers;
        std::shared_ptr<qcril::interfaces::RIL_EccMapType> ecc_map;
        qcril::interfaces::RIL_EccNumberSourceMask sources;
        char* mcc;
        char* mnc;
    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_PBM_FILL_ECC_MAP";

        inline RilPbmFillEccMap():SolicitedSyncMessage<bool>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }

        ~RilPbmFillEccMap();
        void setEmergencyNums(char* number);
        void setEccMap(std::shared_ptr<qcril::interfaces::RIL_EccMapType> ecc_map);
        void setSources(qcril::interfaces::RIL_EccNumberSourceMask sources);
        void setMcc(char* mccVal);
        void setMnc(char* mncVal);
        char* getEmergencyNums();
        std::shared_ptr<qcril::interfaces::RIL_EccMapType> getEccMap();
        qcril::interfaces::RIL_EccNumberSourceMask getSources();
        char* getMcc();
        char* getMnc();

        string dump();
};


#endif // RIL_FILL_ECC_MAP
