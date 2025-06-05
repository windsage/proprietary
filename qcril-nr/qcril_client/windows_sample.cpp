/******************************************************************************
#  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <RilApiSession.hpp>
#include <iostream>
#include <synchapi.h>

int main(int argc, char** argv) {
    RilApiSession session(RilInstance::FIRST, "127.0.0.1");
    Status s = session.initialize();

    if (s != Status::SUCCESS) {
        std::cout << "[RilApiSessionSampleApp]: Failed to initialize API session." << std::endl;
        return 1;
    }

    RIL_Dial dialParams;
    std::string addr("0123456789");
    dialParams.address = const_cast<char*>(addr.c_str());
    dialParams.clir = 0;
    dialParams.uusInfo = nullptr;

    while (true) {
        session.dial(
            dialParams,
            [] (RIL_Errno err) -> void {
                std::cout << "Got response for dial request: " << std::to_string(err) << std::endl;
            }
        );

        session.registerSignalStrengthIndicationHandler(
            [] (const RIL_SignalStrength& signalStrength) {
                (void) signalStrength;
                std::cout << "Received signal strength indication" << std::endl;
            }
        );

        Sleep(120000);
    }
}
