/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

//#include <telux/tel/PhoneFactory.hpp>

//#include "MyNetworkSelectionHandler.hpp"
#include "NetworkMenu.hpp"
//#include "Utils.hpp"
#include <telephony/ril.h>

#define UNKNOWN 0

NetworkMenu::NetworkMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  //  Get the PhoneFactory and NetworkManger instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   networkManager_
  //      = telux::tel::PhoneFactory::getInstance().getNetworkSelectionManager(DEFAULT_SLOT_ID);

  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();

  //  Check if network subsystem is ready
  //   bool subSystemStatus = networkManager_->isSubsystemReady();
  bool subSystemStatus = true;

  //  If network subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "\n\n Network subsystem is not ready, Please wait." << std::endl;
  //      std::future<bool> f = networkManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for network subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //  Exit the application, if SDK is unable to initialize network subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Elapsed Time for Subsystems to ready: " << elapsedTime.count() << "s\n"
              << std::endl;
  } else {
    std::cout << " *** ERROR - Unable to initialize network subsystem" << std::endl;
    exit(0);
  }

  //   networkListener_ = std::make_shared<MyNetworkSelectionListener>();
  //   telux::common::Status status = networkManager_->registerListener(networkListener_);
  //
  //   if(status != telux::common::Status::SUCCESS) {
  //      std::cout << "Failed to registerListener for network Manager" << std::endl;
  //   }
}

NetworkMenu::~NetworkMenu() {
  //   networkManager_->deregisterListener(networkListener_);
  //   networkManager_ = nullptr;
}

void NetworkMenu::init() {
  //   std::shared_ptr<ConsoleAppCommand> performNetworkScanCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "5", "perform_network_scan", {},
  //         std::bind(&NetworkMenu::performNetworkScan, this, std::placeholders::_1)));
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListNetworkSubMenu = {
    CREATE_COMMAND(NetworkMenu::getNetworkSelectionMode, "get_selection_mode"),
    CREATE_COMMAND(NetworkMenu::setNetworkSelectionMode, "set_selection_mode"),
    CREATE_COMMAND(NetworkMenu::getPreferredNetworks, "get_preferred_networks"),
    CREATE_COMMAND(NetworkMenu::setPreferredNetworks, "set_preferred_networks"),
    CREATE_COMMAND(NetworkMenu::startNetworkScan, "start_network_scan"),
    CREATE_COMMAND(NetworkMenu::stopNetworkScan, "stop_network_scan"),
    CREATE_COMMAND(NetworkMenu::getSystemSelectionChannels, "get_system_selection_channels"),
    CREATE_COMMAND(NetworkMenu::setSystemSelectionChannels, "set_system_selection_channels"),
    CREATE_COMMAND(NetworkMenu::setBandMode, "set_band_mode"),
    CREATE_COMMAND(NetworkMenu::queryAvailableBandMode, "get_band_mode"),
    CREATE_COMMAND(NetworkMenu::registerForUnsols, "register_for_unsols"),
    CREATE_COMMAND(NetworkMenu::queryAvailableNetworks, "query_available_networks"),
    CREATE_COMMAND(NetworkMenu::setUsageSetting, "set_usage_setting"),
    CREATE_COMMAND(NetworkMenu::getUsageSetting, "get_usage_setting"),
  };

  addCommands(commandsListNetworkSubMenu);
  ConsoleApp::displayMenu();
}

std::istream& operator >> (std::istream &in, RIL_ScanType& type) {
    std::cout << "Scan Type:" << std::endl;
    std::cout << "1. RIL_ONE_SHOT" << std::endl;
    std::cout << "2. RIL_PERIODIC" << std::endl;
    // To avoid loop, use integer to read value
    int val;
    in >> val;
    type = static_cast<RIL_ScanType>(val);
    return in;
}

std::istream& operator >> (std::istream &in, RIL_RadioAccessNetworks& ran) {
    std::cout << "RAN Type:" << std::endl;
    std::cout << "0. UNKNOWN" << std::endl;
    std::cout << "1. GERAN:" << std::endl;
    std::cout << "2. UTRAN:" << std::endl;
    std::cout << "3. EUTRAN:" << std::endl;
    std::cout << "4. NGRAN:" << std::endl;
    std::cout << "5. CDMA:" << std::endl;
    std::cout << "6. IWLAN:" << std::endl;
    int val;
    in >> val;
    ran = static_cast<RIL_RadioAccessNetworks>(val);
    return in;
}

std::ostream& operator<<(std::ostream& os, const RIL_NgranBands& info) {
    switch(info) {
        case NGRAN_BAND_1:
            os << "NGRAN_BAND_1" << std::endl;
        break;
        case NGRAN_BAND_2:
            os << "NGRAN_BAND_2" << std::endl;
        break;
        case NGRAN_BAND_3:
            os << "NGRAN_BAND_3" << std::endl;
        break;
        case NGRAN_BAND_5:
            os << "NGRAN_BAND_5" << std::endl;
        break;
        case NGRAN_BAND_7:
            os << "NGRAN_BAND_7" << std::endl;
        break;
        case NGRAN_BAND_8:
            os << "NGRAN_BAND_8" << std::endl;
        break;
        case NGRAN_BAND_12:
            os << "NGRAN_BAND_12" << std::endl;
        break;
        case NGRAN_BAND_14:
            os << "NGRAN_BAND_14" << std::endl;
        break;
        case NGRAN_BAND_18:
            os << "NGRAN_BAND_18" << std::endl;
        break;
        case NGRAN_BAND_20:
            os << "NGRAN_BAND_20" << std::endl;
        break;
        case NGRAN_BAND_25:
            os << "NGRAN_BAND_25" << std::endl;
        break;
        case NGRAN_BAND_28:
            os << "NGRAN_BAND_28" << std::endl;
        break;
        case NGRAN_BAND_29:
            os << "NGRAN_BAND_29" << std::endl;
        break;
        case NGRAN_BAND_30:
            os << "NGRAN_BAND_30" << std::endl;
        break;
        case NGRAN_BAND_34:
            os << "NGRAN_BAND_34" << std::endl;
        break;
        case NGRAN_BAND_38:
            os << "NGRAN_BAND_38" << std::endl;
        break;
        case NGRAN_BAND_39:
            os << "NGRAN_BAND_39" << std::endl;
        break;
        case NGRAN_BAND_40:
            os << "NGRAN_BAND_40" << std::endl;
        break;
        case NGRAN_BAND_41:
            os << "NGRAN_BAND_41" << std::endl;
        break;
        case NGRAN_BAND_48:
            os << "NGRAN_BAND_48" << std::endl;
        break;
        case NGRAN_BAND_50:
            os << "NGRAN_BAND_50" << std::endl;
        break;
        case NGRAN_BAND_51:
            os << "NGRAN_BAND_51" << std::endl;
        break;
        case NGRAN_BAND_65:
            os << "NGRAN_BAND_65" << std::endl;
        break;
        case NGRAN_BAND_66:
            os << "NGRAN_BAND_66" << std::endl;
        break;
        case NGRAN_BAND_70:
            os << "NGRAN_BAND_70" << std::endl;
        break;
        case NGRAN_BAND_71:
            os << "NGRAN_BAND_71" << std::endl;
        break;
        case NGRAN_BAND_74:
            os << "NGRAN_BAND_74" << std::endl;
        break;
        case NGRAN_BAND_75:
            os << "NGRAN_BAND_75" << std::endl;
        break;
        case NGRAN_BAND_76:
            os << "NGRAN_BAND_76" << std::endl;
        break;
        case NGRAN_BAND_77:
            os << "NGRAN_BAND_77" << std::endl;
        break;
        case NGRAN_BAND_78:
            os << "NGRAN_BAND_78" << std::endl;
        break;
        case NGRAN_BAND_79:
            os << "NGRAN_BAND_79" << std::endl;
        break;
        case NGRAN_BAND_80:
            os << "NGRAN_BAND_80" << std::endl;
        break;
        case NGRAN_BAND_81:
            os << "NGRAN_BAND_81" << std::endl;
        break;
        case NGRAN_BAND_82:
            os << "NGRAN_BAND_82" << std::endl;
        break;
        case NGRAN_BAND_83:
            os << "NGRAN_BAND_83" << std::endl;
        break;
        case NGRAN_BAND_84:
            os << "NGRAN_BAND_84" << std::endl;
        break;
        case NGRAN_BAND_85:
            os << "NGRAN_BAND_85" << std::endl;
        break;
        case NGRAN_BAND_86:
            os << "NGRAN_BAND_86" << std::endl;
        break;
        case NGRAN_BAND_90:
            os << "NGRAN_BAND_90" << std::endl;
        break;
        case NGRAN_BAND_257:
            os << "NGRAN_BAND_257" << std::endl;
        break;
        case NGRAN_BAND_258:
            os << "NGRAN_BAND_258" << std::endl;
        break;
        case NGRAN_BAND_259:
            os << "NGRAN_BAND_259" << std::endl;
        break;
        case NGRAN_BAND_260:
            os << "NGRAN_BAND_260" << std::endl;
        break;
        case NGRAN_BAND_261:
            os << "NGRAN_BAND_261" << std::endl;
        break;
        default:
            os << "Unknown band:" << static_cast<int>(info) << std::endl;
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_EutranBands& info) {
    switch(info) {
        case EUTRAN_BAND_1:
            os << "EUTRAN_BAND_1" << std::endl;
        break;
        case EUTRAN_BAND_2:
            os << "EUTRAN_BAND_2" << std::endl;
        break;
        case EUTRAN_BAND_3:
            os << "EUTRAN_BAND_3" << std::endl;
        break;
        case EUTRAN_BAND_4:
            os << "EUTRAN_BAND_4" << std::endl;
        break;
        case EUTRAN_BAND_5:
            os << "EUTRAN_BAND_5" << std::endl;
        break;
        case EUTRAN_BAND_6:
            os << "EUTRAN_BAND_6" << std::endl;
        break;
        case EUTRAN_BAND_7:
            os << "EUTRAN_BAND_7" << std::endl;
        break;
        case EUTRAN_BAND_8:
            os << "EUTRAN_BAND_8" << std::endl;
        break;
        case EUTRAN_BAND_9:
            os << "EUTRAN_BAND_9" << std::endl;
        break;
        case EUTRAN_BAND_10:
            os << "EUTRAN_BAND_10" << std::endl;
        break;
        case EUTRAN_BAND_11:
            os << "EUTRAN_BAND_11" << std::endl;
        break;
        case EUTRAN_BAND_12:
            os << "EUTRAN_BAND_12" << std::endl;
        break;
        case EUTRAN_BAND_13:
            os << "EUTRAN_BAND_13" << std::endl;
        break;
        case EUTRAN_BAND_14:
            os << "EUTRAN_BAND_14" << std::endl;
        break;
        case EUTRAN_BAND_17:
            os << "EUTRAN_BAND_17" << std::endl;
        break;
        case EUTRAN_BAND_18:
            os << "EUTRAN_BAND_18" << std::endl;
        break;
        case EUTRAN_BAND_19:
            os << "EUTRAN_BAND_19" << std::endl;
        break;
        case EUTRAN_BAND_20:
            os << "EUTRAN_BAND_20" << std::endl;
        break;
        case EUTRAN_BAND_21:
            os << "EUTRAN_BAND_21" << std::endl;
        break;
        case EUTRAN_BAND_22:
            os << "EUTRAN_BAND_22" << std::endl;
        break;
        case EUTRAN_BAND_23:
            os << "EUTRAN_BAND_23" << std::endl;
        break;
        case EUTRAN_BAND_24:
            os << "EUTRAN_BAND_24" << std::endl;
        break;
        case EUTRAN_BAND_25:
            os << "EUTRAN_BAND_25" << std::endl;
        break;
        case EUTRAN_BAND_26:
            os << "EUTRAN_BAND_26" << std::endl;
        break;
        case EUTRAN_BAND_27:
            os << "EUTRAN_BAND_27" << std::endl;
        break;
        case EUTRAN_BAND_28:
            os << "EUTRAN_BAND_28" << std::endl;
        break;
        case EUTRAN_BAND_29:
            os << "EUTRAN_BAND_29" << std::endl;
        break;
        case EUTRAN_BAND_30:
            os << "EUTRAN_BAND_30" << std::endl;
        break;
        case EUTRAN_BAND_31:
            os << "EUTRAN_BAND_31" << std::endl;
        break;
        case EUTRAN_BAND_32:
            os << "EUTRAN_BAND_32" << std::endl;
        break;
        case EUTRAN_BAND_33:
            os << "EUTRAN_BAND_33" << std::endl;
        break;
        case EUTRAN_BAND_34:
            os << "EUTRAN_BAND_34" << std::endl;
        break;
        case EUTRAN_BAND_35:
            os << "EUTRAN_BAND_35" << std::endl;
        break;
        case EUTRAN_BAND_36:
            os << "EUTRAN_BAND_36" << std::endl;
        break;
        case EUTRAN_BAND_37:
            os << "EUTRAN_BAND_37" << std::endl;
        break;
        case EUTRAN_BAND_38:
            os << "EUTRAN_BAND_38" << std::endl;
        break;
        case EUTRAN_BAND_39:
            os << "EUTRAN_BAND_39" << std::endl;
        break;
        case EUTRAN_BAND_40:
            os << "EUTRAN_BAND_40" << std::endl;
        break;
        case EUTRAN_BAND_41:
            os << "EUTRAN_BAND_41" << std::endl;
        break;
        case EUTRAN_BAND_42:
            os << "EUTRAN_BAND_42" << std::endl;
        break;
        case EUTRAN_BAND_43:
            os << "EUTRAN_BAND_43" << std::endl;
        break;
        case EUTRAN_BAND_44:
            os << "EUTRAN_BAND_44" << std::endl;
        break;
        case EUTRAN_BAND_45:
            os << "EUTRAN_BAND_45" << std::endl;
        break;
        case EUTRAN_BAND_46:
            os << "EUTRAN_BAND_46" << std::endl;
        break;
        case EUTRAN_BAND_47:
            os << "EUTRAN_BAND_47" << std::endl;
        break;
        case EUTRAN_BAND_48:
            os << "EUTRAN_BAND_48" << std::endl;
        break;
        case EUTRAN_BAND_49:
            os << "EUTRAN_BAND_49" << std::endl;
        break;
        case EUTRAN_BAND_65:
            os << "EUTRAN_BAND_65" << std::endl;
        break;
        case EUTRAN_BAND_66:
            os << "EUTRAN_BAND_66" << std::endl;
        break;
        case EUTRAN_BAND_67:
            os << "EUTRAN_BAND_67" << std::endl;
        break;
        case EUTRAN_BAND_68:
            os << "EUTRAN_BAND_68" << std::endl;
        break;
        case EUTRAN_BAND_70:
            os << "EUTRAN_BAND_70" << std::endl;
        break;
        case EUTRAN_BAND_71:
            os << "EUTRAN_BAND_71" << std::endl;
        break;
        case EUTRAN_BAND_72:
            os << "EUTRAN_BAND_72" << std::endl;
        break;
        case EUTRAN_BAND_73:
            os << "EUTRAN_BAND_73" << std::endl;
        break;
        case EUTRAN_BAND_85:
            os << "EUTRAN_BAND_85" << std::endl;
        break;
        case EUTRAN_BAND_125:
            os << "EUTRAN_BAND_125" << std::endl;
        break;
        case EUTRAN_BAND_126:
            os << "EUTRAN_BAND_126" << std::endl;
        break;
        case EUTRAN_BAND_127:
            os << "EUTRAN_BAND_127" << std::endl;
        break;
        default:
            os << "Unknown band:" << static_cast<int>(info) << std::endl;
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_UtranBands& info) {
    switch(info) {
        case UTRAN_BAND_1:
            os << "UTRAN_BAND_1" << std::endl;
        break;
        case UTRAN_BAND_2:
            os << "UTRAN_BAND_2" << std::endl;
        break;
        case UTRAN_BAND_3:
            os << "UTRAN_BAND_3" << std::endl;
        break;
        case UTRAN_BAND_4:
            os << "UTRAN_BAND_4" << std::endl;
        break;
        case UTRAN_BAND_5:
            os << "UTRAN_BAND_5" << std::endl;
        break;
        case UTRAN_BAND_6:
            os << "UTRAN_BAND_6" << std::endl;
        break;
        case UTRAN_BAND_7:
            os << "UTRAN_BAND_7" << std::endl;
        break;
        case UTRAN_BAND_8:
            os << "UTRAN_BAND_8" << std::endl;
        break;
        case UTRAN_BAND_9:
            os << "UTRAN_BAND_9" << std::endl;
        break;
        case UTRAN_BAND_10:
            os << "UTRAN_BAND_10" << std::endl;
        break;
        case UTRAN_BAND_11:
            os << "UTRAN_BAND_11" << std::endl;
        break;
        case UTRAN_BAND_12:
            os << "UTRAN_BAND_12" << std::endl;
        break;
        case UTRAN_BAND_13:
            os << "UTRAN_BAND_13" << std::endl;
        break;
        case UTRAN_BAND_14:
            os << "UTRAN_BAND_14" << std::endl;
        break;
        case UTRAN_BAND_19:
            os << "UTRAN_BAND_19" << std::endl;
        break;
        case UTRAN_BAND_20:
            os << "UTRAN_BAND_20" << std::endl;
        break;
        case UTRAN_BAND_21:
            os << "UTRAN_BAND_21" << std::endl;
        break;
        case UTRAN_BAND_22:
            os << "UTRAN_BAND_22" << std::endl;
        break;
        case UTRAN_BAND_25:
            os << "UTRAN_BAND_25" << std::endl;
        break;
        case UTRAN_BAND_26:
            os << "UTRAN_BAND_26" << std::endl;
        break;
        default:
            os << "Unknown band:" << static_cast<int>(info) << std::endl;
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_GeranBands& info) {
    switch(info) {
        case GERAN_BAND_T380:
            os << "GERAN_BAND_T380" << std::endl;
        break;
        case GERAN_BAND_T410:
            os << "GERAN_BAND_T410" << std::endl;
        break;
        case GERAN_BAND_450:
            os << "GERAN_BAND_T380" << std::endl;
        break;
        case GERAN_BAND_480:
            os << "GERAN_BAND_450" << std::endl;
        break;
        case GERAN_BAND_710:
            os << "GERAN_BAND_710" << std::endl;
        break;
        case GERAN_BAND_750:
            os << "GERAN_BAND_750" << std::endl;
        break;
        case GERAN_BAND_T810:
            os << "GERAN_BAND_T810" << std::endl;
        break;
        case GERAN_BAND_850:
            os << "GERAN_BAND_850" << std::endl;
        break;
        case GERAN_BAND_P900:
            os << "GERAN_BAND_P900" << std::endl;
        break;
        case GERAN_BAND_E900:
            os << "GERAN_BAND_E900" << std::endl;
        break;
        case GERAN_BAND_R900:
            os << "GERAN_BAND_R900" << std::endl;
        break;
        case GERAN_BAND_DCS1800:
            os << "GERAN_BAND_DCS1800" << std::endl;
        break;
        case GERAN_BAND_PCS1900:
            os << "GERAN_BAND_PCS1900" << std::endl;
        break;
        case GERAN_BAND_ER900:
            os << "GERAN_BAND_ER900" << std::endl;
        break;
        default:
            os << "Unknown band:" << static_cast<int>(info) << std::endl;
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_RadioAccessSpecifier& info) {
    os << info.radio_access_network << std::endl;
    os << "bands_length : " << info.bands_length << std::endl;
    os << "channels_length : " << info.channels_length << std::endl;
    for(int i=0; i < info.bands_length; ++i) {
        switch(info.radio_access_network) {
            case GERAN:
                os << info.bands.geran_bands[i];
            break;
            case UTRAN:
                os << info.bands.utran_bands[i];
            break;
            case EUTRAN:
                os << info.bands.eutran_bands[i];
            break;
            case NGRAN:
                os << info.bands.ngran_bands[i];
            break;
            default:
            break;
        }
    }
    if(info.channels_length > 0) {
        os << "Channels : ";
    }
    for(int i=0; i < info.channels_length; i++) {
        os << " " << info.channels[i];
    }
    os << std::endl;
    return os;
}

std::istream& operator >> (std::istream &in, RIL_RadioAccessSpecifier& req) {
    in >> req.radio_access_network;
    std::cout << "Add Bands to specififer? (0 or 1):" << std::endl;
    int bandFlag = 0;
    in >> bandFlag;
    if(bandFlag) {
        std::cout << "Number of Bands:" << std::endl;
        in >> req.bands_length;
        for(int i=0; i < req.bands_length; i++) {
            std::cout << "Enter Band:" << std::endl;
            uint32_t band;
            switch(req.radio_access_network) {
                case GERAN:
                    in >> band;
                    req.bands.geran_bands[i] = static_cast<RIL_GeranBands>(band);
                    break;
                case UTRAN:
                    in >> band;
                    req.bands.utran_bands[i] = static_cast<RIL_UtranBands>(band);
                    break;
                case EUTRAN:
                    in >> band;
                    req.bands.eutran_bands[i] = static_cast<RIL_EutranBands>(band);
                    break;
                case NGRAN:
                    in >> band;
                    req.bands.ngran_bands[i] = static_cast<RIL_NgranBands>(band);
                    break;
                default:
                    break;
            }
        }
    }

    std::cout << "Add channels to specifer? (0 or 1):" << std::endl;
    int channelFlag = 0;
    in >> channelFlag;
    if(channelFlag) {
        std::cout << "Number of Channels:" << std::endl;
        in >> req.channels_length;
        for(int i=1; i <= req.channels_length; i++) {
            std::cout << "Enter Channel " << i << ":" << std::endl;
            in >> req.channels[i];
        }
    }
    return in;
}

std::istream& operator>>(std::istream& in, RIL_AccessMode& accessMode)
{
    std::cout << "0. RIL_ACCESS_MODE_INVALID" << std::endl;
    std::cout << "1. RIL_ACCESS_MODE_PLMN" << std::endl;
    std::cout << "2. RIL_ACCESS_MODE_SNPN" << std::endl;
    std::cout << "Enter AccessMode: " << std::endl;
    int val;
    in >> val;
    accessMode = static_cast<RIL_AccessMode>(val);
    return in;
}

std::istream& operator>>(std::istream& in, RIL_SearchType& searchType)
{
    std::cout << "0. RIL_SEARCH_TYPE_INVALID" << std::endl;
    std::cout << "1. RIL_SEARCH_TYPE_PLMN_AND_CAG" << std::endl;
    std::cout << "2. RIL_SEARCH_TYPE_PLMN_ONLY" << std::endl;
    std::cout << "Enter Search Type: " << std::endl;
    int val;
    in >> val;
    searchType = static_cast<RIL_SearchType>(val);
    return in;
}

std::istream& operator >> (std::istream &in, RIL_NetworkScanRequest& req) {
    in >> req.type;
    if(req.type == RIL_PERIODIC) {
        std::cout << "Interval (min:" << SCAN_MIN_INTERVAL << " max:"
            << SCAN_MAX_INTERVAL << "):" << std::endl;
        in >> req.interval;
    }

    std::cout << "Specifier len:"<< std::endl;
    in >> req.specifiers_length;
    for(int i=0; i < req.specifiers_length; i++) {
        in >> req.specifiers[i];
    }

    std::cout << "Max Search Time:" << std::endl;
    in >> req.maxSearchTime;

    std::cout << "Incremental results (0 or 1):" << std::endl;
    in >> req.incrementalResults;
    if(req.incrementalResults) {
        std::cout << "Incremental Periodicity in secs:"<< std::endl;
        in >> req.incrementalResultsPeriodicity;
    }

    int plmnFlag = 0;
    std::cout << "Plmns specific Scan? (0 or 1):" << std::endl;
    in >> plmnFlag;
    if(plmnFlag) {
        std::cout << "Number of Plmns:" << std::endl;
        in >> req.mccMncLength;
        for(int i=0; i < req.mccMncLength; i++) {
            in >> req.mccMncs[i];
        }
    }

    in >> req.accessMode;
    in >> req.searchType;
    return in;
}

std::ostream& operator<<(std::ostream& os, const RIL_SysSelChannels& info) {
    os << "specifiers_length :" << info.specifiers_length << std::endl;
    for(int i=0; i<info.specifiers_length; i++) {
      std::cout << info.specifiers_latest[i];
    }
    return os;
}

std::istream& operator >> (std::istream &in, RIL_SysSelChannels& req)
{
    std::cout << "Specified Length: (max:8):" << std::endl;
    do {
        in >> req.specifiers_length;
        if(req.specifiers_length > 0 &&
           req.specifiers_length <= MAX_RADIO_ACCESS_NETWORKS) {
           std::cout << "Specifiers is " << req.specifiers_length << std::endl;
           req.specifiers_latest = new RIL_RadioAccessSpecifier[req.specifiers_length];
           break;
        } else if (req.specifiers_length == 0){
            std::cout << "Specifiers is 0" << std::endl;
            break;
        } else {
            std::cout << "Enter Valid Specified Length: (max:8):" << std::endl;
        }
    } while(1);
    if (req.specifiers_latest) {
      std::cout << "specifiers_latest is not null" << std::endl;
      for(int i=0; i < req.specifiers_length; i++) {
        in >> req.specifiers_latest[i];
      }
    }
    return in;
}

std::ostream& operator<<(std::ostream& os, const RIL_EmergencyServiceCategory& cat) {
    os << "Category:";
    if(cat & RIL_EMERGENCY_CATEGORY_UNSPECIFIED) {
        os << "UNSPECIFIED ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_POLICE) {
        os << "POLICE ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_AMBULANCE) {
        os << "AMBULANCE ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE) {
        os << "FIRE_BRIGADE ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_MARINE_GUARD) {
        os << "MARINE_GUARD ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE) {
        os << "MOUNTAIN_RESCUE ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_MIEC) {
        os << "MIEC ";
    }
    if(cat & RIL_EMERGENCY_CATEGORY_AIEC) {
        os << "AIEC";
    }
    os << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_EmergencyNumberSource& source) {
    os << "Sources:";
    if(source & RIL_EMERGENCY_NUMBER_SOURCE_DEFAULT) {
        os << "DEFAULT ";
    }
    if(source & RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING) {
        os << "NETWORK_SIGNALING ";
    }
    if(source & RIL_EMERGENCY_NUMBER_SOURCE_SIM) {
        os << "SIM ";
    }
    if(source & RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG) {
        os << "MODEM_CONFIG";
    }
    os << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_EmergencyNumberInfo& result) {
    os << "**************************************************************" << std::endl;
    os << "Number:" << result.number<< std::endl;
    os << "mcc:" << result.mcc << " mnc:" << result.mnc << std::endl;
    os << static_cast<RIL_EmergencyServiceCategory>(result.category) << std::endl;
    os << static_cast<RIL_EmergencyNumberSource>(result.sources) << std::endl;
    os << "no_of_urns: " << result.no_of_urns << std::endl;
    for(int i=0; i < result.no_of_urns; i++) {
        os << result.urn[i]<< std::endl;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_ScanStatus status) {
    os << "Status:";
    switch(status) {
        case PARTIAL:
            os << "PARTIAL";
            break;
        case COMPLETE:
            os << "COMPLETE";
            break;
        default:
            break;
    }
    os << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_NetworkScanResult& result) {
    os << "Result:" << static_cast<int>(result.error) << std::endl;
    os << result.status << std::endl;
    os << result.network_infos_length << std::endl;
    std::for_each(
        result.network_infos,
        result.network_infos + result.network_infos_length,
        [&os] (const RIL_CellInfo_v12& cellInfo) {
            const char* mcc = nullptr;
            const char* mnc = nullptr;
            const char* shortName = nullptr;
            const char* longName = nullptr;
            bool cagInfoValid = false;
            bool snpnInfoValid = false;
            switch (cellInfo.cellInfoType) {
                case RIL_CELL_INFO_TYPE_GSM:
                    mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                    mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                    shortName = cellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort;
                    longName = cellInfo.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong;
                    break;
                case RIL_CELL_INFO_TYPE_LTE:
                    mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                    mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                    shortName = cellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort;
                    longName = cellInfo.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong;
                    break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                    mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                    mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                    shortName = cellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort;
                    longName = cellInfo.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong;
                    break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                    mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                    mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                    shortName =
                        cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaShort;
                    longName = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames.alphaLong;
                    break;
                case RIL_CELL_INFO_TYPE_NR:
                    mcc = cellInfo.CellInfo.nr.cellIdentityNr.mcc;
                    mnc = cellInfo.CellInfo.nr.cellIdentityNr.mnc;
                    shortName = cellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaShort;
                    longName = cellInfo.CellInfo.nr.cellIdentityNr.operatorNames.alphaLong;
                    if (cellInfo.CellInfo.nr.cellIdentityNr.cag_info_valid) {
                        cagInfoValid = true;
                    }
                    if (cellInfo.CellInfo.nr.cellIdentityNr.snpn_info_valid) {
                        snpnInfoValid = true;
                    }
                    break;
                default:
                    break;
            }

            os << "cellInfoType: " << cellInfo.cellInfoType
               << " registered: " << cellInfo.registered << " mcc: " << (mcc ? mcc : "")
               << " mnc: " << (mnc ? mnc : "")
               << "short: " << (shortName ? shortName : "")
               << "long: " << (longName ? longName : "")<< std::endl;

            if (snpnInfoValid) {
                os << "snpn nid: ";
                for (int i = 0; i < MAX_SNPN_NID_LEN; i++) {
                    os << static_cast<unsigned int>(
                              cellInfo.CellInfo.nr.cellIdentityNr.snpn_info.nid[i])
                       << " ";
                }
                os << std::endl;
                os << "snpn mcc: " << cellInfo.CellInfo.nr.cellIdentityNr.snpn_info.mcc << std::endl;
                os << "snpn mnc: " << cellInfo.CellInfo.nr.cellIdentityNr.snpn_info.mnc << std::endl;
                os << "snpn signalStrength: "
                   << cellInfo.CellInfo.nr.cellIdentityNr.snpn_info.signalStrength << std::endl;
                os << "snpn signalQuality: "
                   << cellInfo.CellInfo.nr.cellIdentityNr.snpn_info.signalQuality << std::endl;
            }

            if (cagInfoValid) {
                os << "cagInfo cagName: " << cellInfo.CellInfo.nr.cellIdentityNr.cag_info.cagName
                   << std::endl;
                os << "cagInfo cagId: " << cellInfo.CellInfo.nr.cellIdentityNr.cag_info.cagId
                   << std::endl;
                os << "cagInfo cagOnlyAccess: "
                   << static_cast<unsigned int>(
                          cellInfo.CellInfo.nr.cellIdentityNr.cag_info.cagOnlyAccess)
                   << std::endl;
                os << "cagInfo cagInAllowedList: "
                   << static_cast<unsigned int>(
                          cellInfo.CellInfo.nr.cellIdentityNr.cag_info.cagInAllowedList)
                   << std::endl;
            }
        }
    );

    return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_EmergencyList& result) {
    os << "Length:" << result.emergency_list_length << std::endl;
    for(int i=0; i<result.emergency_list_length; i++) {
        os << result.emergency_numbers[i]<< std::endl;
    }
    return os;
}

std::istream& operator >> (std::istream &in, RIL_RadioBandMode& bandMode) {
    std::cout << "Enter BandMode:" << std::endl;
    std::cout << "0. BAND_MODE_UNSPECIFIED" << std::endl;
    std::cout << "1. BAND_MODE_EURO" << std::endl;
    std::cout << "2. BAND_MODE_USA" << std::endl;
    std::cout << "3. BAND_MODE_JPN" << std::endl;
    std::cout << "4. BAND_MODE_AUS" << std::endl;
    std::cout << "5. BAND_MODE_AUS_2" << std::endl;
    std::cout << "6. BAND_MODE_CELL_800" << std::endl;
    std::cout << "7. BAND_MODE_PCS" << std::endl;
    std::cout << "8. BAND_MODE_JTACS" << std::endl;
    std::cout << "9. BAND_MODE_KOREA_PCS" << std::endl;
    std::cout << "10. BAND_MODE_5_450M" << std::endl;
    std::cout << "11. BAND_MODE_IMT2000" << std::endl;
    std::cout << "12. BAND_MODE_7_700M_2" << std::endl;
    std::cout << "13. BAND_MODE_8_1800M" << std::endl;
    std::cout << "14. BAND_MODE_9_900M " << std::endl;
    std::cout << "15. BAND_MODE_10_800M_2" << std::endl;
    std::cout << "16. BAND_MODE_EURO_PAMR_400M" << std::endl;
    std::cout << "17. BAND_MODE_AWS" << std::endl;
    std::cout << "18. BAND_MODE_USA_2500M" << std::endl;
    int val;
    in >> val;
    bandMode = static_cast<RIL_RadioBandMode>(val);
    return in;
}

std::istream& operator >> (std::istream &in, RIL_UsageSettingMode& settingMode) {
    std::cout << "Enter Usage Setting Mode:" << std::endl;
    std::cout << "1. VOICE_CENTRIC" << std::endl;
    std::cout << "2. DATA_CENTRIC" << std::endl;
    int val;
    in >> val;
    settingMode = static_cast<RIL_UsageSettingMode>(val);
    return in;
}


std::ostream& operator<<(std::ostream& os, const RIL_RadioBandMode& bandMode) {
    os << "BandMode:";
    switch(bandMode) {
        case BAND_MODE_UNSPECIFIED:
            os << "BAND_MODE_UNSPECIFIED";
        break;
        case BAND_MODE_EURO:
            os << "BAND_MODE_EURO";
        break;
        case BAND_MODE_USA:
            os << "BAND_MODE_USA";
        break;
        case BAND_MODE_JPN:
            os << "BAND_MODE_JPN";
        break;
        case BAND_MODE_AUS:
            os << "BAND_MODE_AUS";
        break;
        case BAND_MODE_AUS_2:
            os << "BAND_MODE_AUS_2";
        break;
        case BAND_MODE_CELL_800:
            os << "BAND_MODE_CELL_800";
        break;
        case BAND_MODE_PCS:
            os << "BAND_MODE_PCS";
        break;
        case BAND_MODE_JTACS:
            os << "BAND_MODE_JTACS";
        break;
        case BAND_MODE_KOREA_PCS:
            os << "BAND_MODE_KOREA_PCS";
        break;
        case BAND_MODE_5_450M:
            os << "BAND_MODE_5_450M";
        break;
        case BAND_MODE_IMT2000:
            os << "BAND_MODE_IMT2000";
        break;
        case BAND_MODE_7_700M_2:
            os << "BAND_MODE_7_700M_2";
        break;
        case BAND_MODE_8_1800M:
            os << "BAND_MODE_8_1800M";
        break;
        case BAND_MODE_9_900M:
            os << "BAND_MODE_9_900M";
        break;
        case BAND_MODE_10_800M_2:
            os << "BAND_MODE_10_800M_2";
        break;
        case BAND_MODE_EURO_PAMR_400M:
            os << "BAND_MODE_EURO_PAMR_400M";
        break;
        case BAND_MODE_AWS:
            os << "BAND_MODE_AWS";
        break;
        case BAND_MODE_USA_2500M:
            os << "BAND_MODE_USA_2500M";
        break;
    }
    return os;
}

void NetworkMenu::getNetworkSelectionMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "Get Network Selection Mode:";
  Status s = rilSession.QueryNetworkSelectionMode([](RIL_Errno err, int mode) -> void {
    std::cout << "Got response for Network Selection Mode : " << err << std::endl;
    std::cout << "Network Selection Mode : " << (mode ? "MANUAL" : "AUTOMATIC") << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getNetworkSelectionMode" << std::endl;
  }
}

void NetworkMenu::setNetworkSelectionMode(std::vector<std::string> userInput) {
  //   if(networkManager_) {
  //      bool selectionMode;
  //      std::string mcc;
  //      std::string mnc;
  //      telux::common::Status retStatus = telux::common::Status::FAILED;
  //      std::cout << "Enter Network Selection Mode(0-AUTOMATIC,1-MANUAL): ";
  //      std::cin >> selectionMode;
  //      Utils::validateInput(selectionMode);
  //      if(selectionMode == 1) {
  //         telux::tel::NetworkSelectionMode selectMode = telux::tel::NetworkSelectionMode::MANUAL;
  //         std::cout << "Enter MCC: ";
  //         std::cin >> mcc;
  //         Utils::validateInput(mcc);
  //         std::cout << "Enter MNC: ";
  //         std::cin >> mnc;
  //         Utils::validateInput(mnc);
  //         retStatus = networkManager_->setNetworkSelectionMode(
  //            selectMode, mcc, mnc, &MyNetworkResponsecallback::setNetworkSelectionModeResponseCb);
  //
  //      } else if(selectionMode == 0) {
  //         telux::tel::NetworkSelectionMode selectMode = telux::tel::NetworkSelectionMode::AUTOMATIC;
  //         mcc = "0";
  //         mnc = "0";
  //         retStatus = networkManager_->setNetworkSelectionMode(
  //            selectMode, mcc, mnc, &MyNetworkResponsecallback::setNetworkSelectionModeResponseCb);
  //
  //      } else {
  //         std::cout << "Invalid network selection mode input, Valid values are 0 or 1";
  //      }
  //      if(retStatus == telux::common::Status::SUCCESS) {
  //         std::cout << "\nSet network selection mode request sent successfully\n";
  //      } else {
  //         std::cout << "\nSet network selection mode request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  bool selectionMode;
  std::string mcc;
  std::string mnc;
  std::cout << "Enter Network Selection Mode(0-AUTOMATIC,1-MANUAL): ";
  std::cin >> selectionMode;
  if (selectionMode == 1) {
    std::cout << "Enter MCC: ";
    std::cin >> mcc;
    std::cout << "Enter MNC: ";
    std::cin >> mnc;
    Status s = rilSession.setNetworkSelectionManual(mcc, mnc, [](RIL_Errno err) -> void {
      std::cout << "Got response for Set Network Selection Mode Manual: " << err << std::endl;
    });
    if(s != Status::SUCCESS) {
      std::cout << "Failed to send setNetworkSelectionManual" << std::endl;
    }
  } else {
    Status s = rilSession.setNetworkSelectionAutomatic([](RIL_Errno err) -> void {
      std::cout << "Got response for Set Network Selection Mode Automatic : " << err << std::endl;
    });
    if(s != Status::SUCCESS) {
      std::cout << "Failed to send setNetworkSelectionAutomatic" << std::endl;
    }
  }
}

const std::string convertToPrefType(int type) {
  std::string ret;

  switch (type) {
    case 0:
      ret = "GSM WCDMA";
      break;
    case 1:
      ret = "GSM ONLY";
      break;
    case 2:
      ret = "WCDMA";
      break;
    case 3:
      ret = "GSM/WCDMA AUTO";
      break;
    case 4:
      ret = "CDMA/EVDO AUTO";
      break;
    case 5:
      ret = "CDMA ONLY";
      break;
    case 6:
      ret = "EVDO ONLY";
      break;
    case 7:
      ret = "GSM/WCDMA CDMA EVDO AUTO";
      break;
    case 8:
      ret = "LTE CDMA EVDO";
      break;
    case 9:
      ret = "LTE GSM WCDMA";
      break;
    case 10:
      ret = "LTE CMDA EVDO GSM WCDMA";
      break;
    case 11:
      ret = "LTE ONLY";
      break;
    case 12:
      ret = "LTE WCDMA";
      break;
    case 13:
      ret = "TD-SCDMA ONLY";
      break;
    case 14:
      ret = "TD-SCDMA WCDMA";
      break;
    case 15:
      ret = "TD-SCDMA LTE";
      break;
    case 16:
      ret = "TD-SCDMA GSM";
      break;
    case 17:
      ret = "TD-SCDMA GSM LTE";
      break;
    case 18:
      ret = "TD-SCDMA GSM WCDMA";
      break;
    case 19:
      ret = "TD-SCDMA WCDMA LTE";
      break;
    case 20:
      ret = "TD-SCDMA GSM WCDMA LTE";
      break;
    case 21:
      ret = "TD-SCDMA GSM WCDMA CDMA EVDO AUTO";
      break;
    case 22:
      ret = "TD-SCDMA LTE CDMA EVDO GSM WCDMA";
      break;
    case 23:
      ret = "NR5G Only";
      break;
    case 24:
      ret = "NR5G TD-SCDMA LTE CDMA EVDO GSM WCDMA";
      break;
    case 25:
      ret = "GSM WCDMA LTE NR5G";
      break;
    case 26:
      ret = "GSM WCDMA NR5G";
      break;
    case 27:
      ret = "GSM LTE NR5G";
      break;
    case 28:
      ret = "WCDMA LTE NR5G";
      break;
    case 29:
      ret = "GSM NR5G";
      break;
    case 30:
      ret = "WCDMA NR5G";
      break;
    case 31:
      ret = "LTE NR5G";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}

void printPrefTypes() {
  std::cout << "Choose from below Network Types" << std::endl;
  std::cout << "===============================" << std::endl
            << "0   -> GSM/WCDMA" << std::endl
            << "1   -> GSM only " << std::endl
            << "2   -> WCDMA  " << std::endl
            << "3   -> GSM/WCDMA" << std::endl
            << "4   -> CDMA and EvDo" << std::endl
            << "5   -> CDMA only " << std::endl
            << "6   -> EvDo only " << std::endl
            << "7   -> GSM/WCDMA, CDMA, and EvDo" << std::endl
            << "8   -> LTE, CDMA and EvDo " << std::endl
            << "9   -> LTE, GSM/WCDMA " << std::endl
            << "10  -> LTE, CDMA, EvDo, GSM/WCDMA " << std::endl
            << "11  -> LTE only " << std::endl
            << "12  -> LTE/WCDMA " << std::endl
            << "13  -> TD-SCDMA only " << std::endl
            << "14  -> TD-SCDMA and WCDMA " << std::endl
            << "15  -> TD-SCDMA and LTE " << std::endl
            << "16  -> TD-SCDMA and GSM " << std::endl
            << "17  -> TD-SCDMA,GSM and LTE " << std::endl
            << "18  -> TD-SCDMA, GSM/WCDMA " << std::endl
            << "19  -> TD-SCDMA, WCDMA and LTE " << std::endl
            << "20  -> TD-SCDMA, GSM/WCDMA and LTE " << std::endl
            << "21  -> TD-SCDMA, GSM/WCDMA, CDMA and EvDo " << std::endl
            << "22  -> TD-SCDMA, LTE, CDMA, EvDo GSM/WCDMA " << std::endl
            << "23  -> NR5G Only" << std::endl
            << "24  -> NR5G, TD-SCDMA, LTE, CDMA, EvDo GSM/WCDMA " << std::endl
            << "25  -> GSM, WCDMA, LTE, NR5G " << std::endl
            << "26  -> GSM WCDMA NR5G " << std::endl
            << "27  -> GSM LTE NR5G " << std::endl
            << "28  -> WCDMA LTE NR5G " << std::endl
            << "29  -> GSM NR5G " << std::endl
            << "30  -> WCDMA NR5G " << std::endl
            << "31  -> LTE NR5G " << std::endl
            << std::endl
            << "Enter the Preferred Type: ";
}

void NetworkMenu::getPreferredNetworks(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getPreferredNetworkType([](RIL_Errno err, int prefMode) -> void {
    std::cout << "Got response for Preferred request." << err << std::endl;
    std::cout << "Preferred Network Type: " << convertToPrefType(prefMode) << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getPreferredNetworks" << std::endl;
  }
}

int NetworkMenu::convertToRatType(int input) {
  //   switch(input) {
  //      case 1:
  //         return telux::tel::RatType::GSM;
  //      case 2:
  //         return telux::tel::RatType::LTE;
  //      case 3:
  //         return telux::tel::RatType::UMTS;
  //      default:
  //         return UNKNOWN;
  //   }
  return 1;
}

// telux::tel::PreferredNetworkInfo NetworkMenu::getNetworkInfoFromUser() {
//   telux::tel::PreferredNetworkInfo networkInfo = {};
//   telux::tel::RatMask rat;
//   uint16_t mcc;
//   uint16_t mnc;
//   std::string preference;
//   std::vector<int> options;
//   std::cout << "Enter MCC: ";
//   std::cin >> mcc;
//   Utils::validateInput(mcc);
//   networkInfo.mcc = mcc;
//   std::cout << "Enter MNC: ";
//   std::cin >> mnc;
//   Utils::validateInput(mnc);
//   networkInfo.mnc = mnc;
//   std::cout << "Select RAT types (1-GSM, 2-LTE, 3-UMTS) \n";
//   std::cout << "Enter RAT types\n(For example: enter 1,2 to set GSM & "
//                "LTE RAT type): ";
//   std::cin >> preference;
//   Utils::validateNumericString(preference);
//   std::stringstream ss(preference);
//   int pref;
//   while(ss >> pref) {
//      options.push_back(pref);
//      if(ss.peek() == ',' || ss.peek() == ' ')
//         ss.ignore();
//   }
//   for(auto &opt : options) {
//      if((opt == 1) || (opt == 2) || (opt == 3)) {
//         rat.set(convertToRatType(opt));
//      } else {
//         std::cout << "Preference should not be out of range" << std::endl;
//      }
//   }
//   options.clear();
//   networkInfo.ratMask = rat;
//   return networkInfo;
//}

void NetworkMenu::setPreferredNetworks(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  printPrefTypes();

  int type;
  std::cin >> type;
  Status s = rilSession.setPreferredNetworkType(type, [](RIL_Errno err) -> void {
    std::cout << "Got response for Set preferred Network Type: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setPreferredNetworks" << std::endl;
  }
}

void NetworkMenu::performNetworkScan(std::vector<std::string> userInput) {
  //   if(networkManager_) {
  //      auto ret = networkManager_->performNetworkScan(
  //         MyPerformNetworkScanCallback::performNetworkScanResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "\nPerform network scan request sent successfully\n";
  //      } else {
  //         std::cout << "\nPerform network scan request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void NetworkMenu::startNetworkScan(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_NetworkScanRequest req {};
  std::cin >> req;
  Status s = rilSession.startNetworkScan(req, [](RIL_Errno err) -> void {
    std::cout << "Got response for start network scan request : " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send startNetworkScan" << std::endl;
  }
}

void NetworkMenu::stopNetworkScan(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.stopNetworkScan([](RIL_Errno err) -> void {
    std::cout << "Got response for stop network scan request : " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send stopNetworkScan" << std::endl;
  }
}

void NetworkMenu::getSystemSelectionChannels(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.getSystemSelectionChannels([](RIL_Errno err,
      const RIL_SysSelChannels& resp) -> void {
    std::cout << "Got response for getSystemSelectionChannels: " << err << std::endl;
    std::cout << resp << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getSystemSelectionChannels" << std::endl;
  }
}

void NetworkMenu::setSystemSelectionChannels(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_SysSelChannels req {};
  std::cin >> req;
  Status s = rilSession.setSystemSelectionChannels(req, [](RIL_Errno err) -> void {
    std::cout << "Got response for setSystemSelectionChannels: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setSystemSelectionChannels" << std::endl;
  }
}

void NetworkMenu::setBandMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_RadioBandMode bandMode = BAND_MODE_UNSPECIFIED;
  std::cin >> bandMode;
  Status s = rilSession.setBandMode(static_cast<int>(bandMode),
    [](RIL_Errno err) -> void {
    std::cout << "Got response for setBandMode: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setBandMode" << std::endl;
  }
}

void NetworkMenu::queryAvailableBandMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.queryAvailableBandMode([](RIL_Errno err, const int *bandMode) -> void {
    std::cout << "Got response for queryAvailableBandMode: " << err << std::endl;
    if (bandMode) {
        size_t size = bandMode[0];
        std::cout << "Size:" << size << std::endl;
        for(int i=1; i<=size; i++) {
            std::cout << static_cast<RIL_RadioBandMode>(bandMode[i]) << std::endl;
        }
    }
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send queryAvailableBandMode" << std::endl;
  }
}

void NetworkMenu::getUsageSetting(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.getUsageSetting([](RIL_Errno err, const int settingMode) -> void {
    std::cout << "Got response for getUsageSetting: " << err << std::endl;
    switch (settingMode) {
      case 1:
        std::cout << "Sub is in VOICE centric mode" << std::endl;
        break;
      case 2:
        std::cout << "Sub is in DATA centric mode"  << std::endl;
        break;
      default:
        std::cout << "Unknown usage setting mode" << std::endl;
    }
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send queryAvailableBandMode" << std::endl;
  }
}

void NetworkMenu::setUsageSetting(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_UsageSettingMode settingMode = RIL_UNKNOWN_USAGE_SETTING;
  std::cin >> settingMode;
  Status s = rilSession.setUsageSetting(static_cast<int>(settingMode),
    [](RIL_Errno err) -> void {
    std::cout << "Got response for setUsageSetting: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setUsageSetting" << std::endl;
  }
}

void NetworkMenu::registerForUnsols(std::vector<std::string> userInput) {
    rilSession.registerNetworksScanIndicationHandler(
        [] (RIL_NetworkScanResult result) {
            std::cout<<"[NetworkMenu]: Received scan results";
            std::cout<<result<<std::endl;
        }
    );

    rilSession.registerEmergencyListIndicationHandler(
        [] (RIL_EmergencyList list) {
            std::cout<<"[NetworkMenu]: Received emergency numbers list";
            std::cout<<list<<std::endl;
        }
    );
}

void NetworkMenu::queryAvailableNetworks(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.queryAvailableNetworks([](RIL_Errno err, const std::vector<RIL_OperatorInfo>& nw_list) -> void {
    std::cout << "Got response for query available networks request : " << err << std::endl;
    for(auto i = 0; i < nw_list.size(); i++)
    {
        std::cout << "alphaLong: " << nw_list[i].alphaLong << std::endl;
        std::cout << "alphaShort: " << nw_list[i].alphaShort << std::endl;
        std::cout << "operatorNumeric: " << nw_list[i].operatorNumeric << std::endl;
        std::cout << "status: " << nw_list[i].status << std::endl;
    }
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send queryAvailableNetworks" << std::endl;
  }
}
