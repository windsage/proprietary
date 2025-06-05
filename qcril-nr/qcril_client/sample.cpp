/******************************************************************************
#  Copyright (c) 2019-2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <inttypes.h>

#include <RilApiSession.hpp>
#include <Logger.h>

#define TAG "RIL_Sample_Client"

void testDataCall(RilApiSession* session) {
    //RIL_REQUEST_SETUP_DATA_CALL
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession testDataCall enter ");
        RIL_RadioAccessNetworks accessNetwork = (RIL_RadioAccessNetworks)0x03;
        bool roamingAllowed = true;
        RIL_RadioDataRequestReasons reason = (RIL_RadioDataRequestReasons)0x01;
        RIL_DataProfileInfo dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo));
        dataProfile.profileId = 1;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IP");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IP");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred =0 ;
        dataProfile.persistent = 1;
        if ( session->setupDataCall( accessNetwork, dataProfile, roamingAllowed, reason, [] (RIL_Errno err, Status status, const RIL_Data_Call_Response_v11* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for setupDatacall request: %d", err);
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("SetupDataCall : Failure in socket data read. Exiting testcase");
                return;
            }
            if(response) {
                QCRIL_HAL_LOG_DEBUG("status :%d", response->status);
                QCRIL_HAL_LOG_DEBUG("suggestedRetryTime :%" PRId64, response->suggestedRetryTime);
                QCRIL_HAL_LOG_DEBUG("cid :%d", response->cid);
                QCRIL_HAL_LOG_DEBUG("active :%d", response->active);
                if(response->type) {
                    QCRIL_HAL_LOG_DEBUG("type :%s", response->type);
                }
                if(response->ifname) {
                    QCRIL_HAL_LOG_DEBUG("ifname :%s", response->ifname);
                }
                if(response->addresses) {
                    QCRIL_HAL_LOG_DEBUG("addresses :%s", response->addresses);
                }
                if(response->dnses) {
                    QCRIL_HAL_LOG_DEBUG("dnses :%s", response->dnses);
                }
                if(response->gateways) {
                    QCRIL_HAL_LOG_DEBUG("gateways :%s", response->gateways);
                }
                if(response->pcscf) {
                    QCRIL_HAL_LOG_DEBUG("pcscf :%s", response->pcscf);
                }
                QCRIL_HAL_LOG_DEBUG("mtu :%d", response->mtu);
            } else {
                QCRIL_HAL_LOG_DEBUG("response is NULL!!!");
            }
        }) == Status::FAILURE) {
            QCRIL_HAL_LOG_DEBUG("SetupDataCall Failed Due to Marshalling Failure.");
        }
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetupDataCall exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testDeactivateCall(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession deactivateDataCall enter ");
        int32_t cid = 0;
        bool reason = true;
        session->deactivateDataCall( cid, reason, [] (RIL_Errno err) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for deactivateDataCall request: %d", err); });
        QCRIL_HAL_LOG_DEBUG("RilApiSession deactivateDataCall exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolDataCallListChanged(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession testUnsolDataCallListChanged enter ");
        session->registerDataCallListChangedIndicationHandler(
            [] (const RIL_Data_Call_Response_v11 response[], size_t sz) {
                QCRIL_HAL_LOG_DEBUG("RadioDataCallListChangeInd  size = %zu", sz);
                for (int i = 0; (i < sz); i++) {
                    QCRIL_HAL_LOG_DEBUG("RadioDataCallListChangeInd reading entry ");
                    QCRIL_HAL_LOG_DEBUG("status :%d", response[i].status);
                    QCRIL_HAL_LOG_DEBUG("suggestedRetryTime :%" PRId64, response[i].suggestedRetryTime);
                    QCRIL_HAL_LOG_DEBUG("cid :%d", response[i].cid);
                    QCRIL_HAL_LOG_DEBUG("active :%d", response[i].active);
                    if(response[i].type) {
                        QCRIL_HAL_LOG_DEBUG("type :%s", response[i].type);
                    }
                    if(response[i].ifname) {
                        QCRIL_HAL_LOG_DEBUG("ifname :%s", response[i].ifname);
                    }
                    if(response[i].addresses) {
                        QCRIL_HAL_LOG_DEBUG("addresses :%s", response[i].addresses);
                    }
                    if(response[i].dnses) {
                        QCRIL_HAL_LOG_DEBUG("dnses :%s", response[i].dnses);
                    }
                    if(response[i].gateways) {
                        QCRIL_HAL_LOG_DEBUG("gateways :%s", response[i].gateways);
                    }
                    if(response[i].pcscf) {
                        QCRIL_HAL_LOG_DEBUG("pcscf :%s", response[i].pcscf);
                    }
                    QCRIL_HAL_LOG_DEBUG("mtu :%d", response[i].mtu);
                }
                QCRIL_HAL_LOG_DEBUG("Received DataCallListChangedIndication");
            }
        );
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetInitialAttachApn(RilApiSession* session){
    if (session) {
        QCRIL_HAL_LOG_DEBUG("testSetInitialAttachApn enter ");
        RIL_DataProfileInfo dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo));
        dataProfile.profileId = 1;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IPV4");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IPV4");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred =0 ;
        dataProfile.persistent = 1;
        session->setInitialAttachApn( dataProfile, [] (RIL_Errno err) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for setInitialAttachApn request: %d", err); });
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}


void testGetDataCallList(RilApiSession* session){
    if (session) {
        QCRIL_HAL_LOG_DEBUG("testGetDataCallList enter ");
        session->getDataCallList( [] (const RIL_Errno e, const RIL_Data_Call_Response_v11* response, const unsigned long sz) -> void {
            QCRIL_HAL_LOG_DEBUG("testGetDataCallList status: %d", e);
            if(!response) {
                QCRIL_HAL_LOG_DEBUG("testGetDataCallList response NULL ");
                return;
            }
            QCRIL_HAL_LOG_DEBUG("testGetDataCallList  received Data Call size = %lu", sz);
            for (int i = 0; (i < sz); i++) {
                QCRIL_HAL_LOG_DEBUG("testGetDataCallList reading entry ");
                QCRIL_HAL_LOG_DEBUG("status :%d", response[i].status);
                QCRIL_HAL_LOG_DEBUG("suggestedRetryTime :%" PRId64, response[i].suggestedRetryTime);
                QCRIL_HAL_LOG_DEBUG("cid :%d", response[i].cid);
                QCRIL_HAL_LOG_DEBUG("active :%d", response[i].active);
                if(response[i].type) {
                    QCRIL_HAL_LOG_DEBUG("type :%s", response[i].type);
                }
                if(response[i].ifname) {
                    QCRIL_HAL_LOG_DEBUG("ifname :%s", response[i].ifname);
                }
                if(response[i].addresses) {
                    QCRIL_HAL_LOG_DEBUG("addresses :%s", response[i].addresses);
                }
                if(response[i].dnses) {
                    QCRIL_HAL_LOG_DEBUG("dnses :%s", response[i].dnses);
                }
                if(response[i].gateways) {
                    QCRIL_HAL_LOG_DEBUG("gateways :%s", response[i].gateways);
                }
                if(response[i].pcscf) {
                    QCRIL_HAL_LOG_DEBUG("pcscf :%s", response[i].pcscf);
                }
                QCRIL_HAL_LOG_DEBUG("mtu :%d", response[i].mtu);
            }
            QCRIL_HAL_LOG_DEBUG("Received testGetDataCallList Done!!");
        });
        QCRIL_HAL_LOG_DEBUG("testGetDataCallList exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetDataProfile(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("testSetDataProfile enter ");
        RIL_DataProfileInfo dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo));
        dataProfile.profileId = 0;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IPV4V6");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IPV4V6");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred = 0 ;
        dataProfile.persistent = 1;

        RIL_DataProfileInfo dataProfile2;
        memset(&dataProfile2, 0, sizeof(RIL_DataProfileInfo));

        dataProfile2.profileId = 2;

        std::string apnname_1("wirelessone.com");
        dataProfile2.apn = const_cast<char*>(apnname_1.c_str());

        std::string protocoltype_1("IPV6");
        dataProfile2.protocol = const_cast<char*>(protocoltype_1.c_str());

        std::string roamingProtocoltype_1("IPV6");
        dataProfile2.roamingProtocol = const_cast<char*>(roamingProtocoltype_1.c_str());

        dataProfile2.authType = 0;

        std::string username_1("xyz");
        dataProfile2.user = const_cast<char*>(username_1.c_str());

        std::string password_1("xyz");
        dataProfile2.password = const_cast<char*>(password_1.c_str());

        dataProfile2.type = 1;
        dataProfile2.maxConnsTime = 0;
        dataProfile2.maxConns = 0;
        dataProfile2.waitTime = 1;
        dataProfile2.enabled = 1;
        dataProfile2.supportedTypesBitmask = 0X1;
        dataProfile2.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile2.mtu = 1400;
        dataProfile2.preferred =0 ;
        dataProfile2.persistent = 1;

        RIL_DataProfileInfo dataProfileList[2] = {  dataProfile, dataProfile2 };
        RIL_DataProfileInfo *dataProfilePtr = dataProfileList;

        if(dataProfilePtr) {
           int len = sizeof(dataProfileList)/sizeof(RIL_DataProfileInfo);
           QCRIL_HAL_LOG_DEBUG(" Sending SetDataProfile  ");
           session->setDataProfile( dataProfilePtr, len, [] (RIL_Errno err) -> void { QCRIL_HAL_LOG_DEBUG("Got response for setDataProfile request: %d", err); });
        }
        QCRIL_HAL_LOG_DEBUG(" testSetDataProfile done!! ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testStartLceData(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession StartLCEData enter ");
        int32_t interval = 1000;
        int32_t mode = 1;
        session->StartLceData( interval, mode, [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for StartLCEData request: %d", err);
            if (response) {
                QCRIL_HAL_LOG_DEBUG("Pull LCE:LCE status =%c", response->lce_status);
                QCRIL_HAL_LOG_DEBUG("Pull LCE:Actual Interval in ms = %u",
                        response->actual_interval_ms);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession StartLCEData exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testStopLceData(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession StopLCEData enter ");
        session->StopLceData( [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for StopLCEData request: %d", err);
            if (response) {
                QCRIL_HAL_LOG_DEBUG("Pull LCE:LCE status =%c",
                        response->lce_status);
                QCRIL_HAL_LOG_DEBUG("Pull LCE:Actual Interval in ms = %u",
                        response->actual_interval_ms);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession StopLCEData exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testPullLceData(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession PullLCEData enter ");
        session->PullLceData( [] (RIL_Errno err, const RIL_LceDataInfo* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for PullLCEData request: %d", err);
            if (response) {
                QCRIL_HAL_LOG_DEBUG("Pull LCE:Last hop capacity in kbps =%u",
                        response->last_hop_capacity_kbps);
                QCRIL_HAL_LOG_DEBUG("Pull LCE:Confidence level = %c",
                        response->confidence_level);
                QCRIL_HAL_LOG_DEBUG("Pull LCE:LCE suspended status=%c",
                        response->lce_suspended);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession PullLCEData exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolPcoDataChanged(RilApiSession* session) {
    if (session) {
        session->registerPcoDataChangeHandler(
            [] (const  RIL_PCO_Data PcoData) {
                QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: cid = %d",
                        PcoData.cid);
                if (PcoData.bearer_proto) {
                    QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: BearerProtocol = %s",
                            PcoData.bearer_proto);
                }
                else {
                    QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: bearer_proto is NULL");
                }
                QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: PcoId %d",
                        PcoData.pco_id);
                if (PcoData.contents) {
                    for (int i = 0; i < PcoData.contents_length; i++) {
                        QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: content %d = %c",
                                i, PcoData.contents[i]);
                    }
                }
                else {
                    QCRIL_HAL_LOG_DEBUG("UnsolPcoDataChanged: contents is NULL");
                }
                QCRIL_HAL_LOG_DEBUG("Received PCODataChangeIndication");
            }
        );
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolLCEDataChanged(RilApiSession* session) {
    if (session) {
        session->registerLCEDataChangeHandler(
            [] (const  RIL_LinkCapacityEstimate LCEData) {
                QCRIL_HAL_LOG_DEBUG("UnsolLceDataChanged:Downlink capacity in kbps = %u",
                        LCEData.downlinkCapacityKbps);
                QCRIL_HAL_LOG_DEBUG("UnsolLceDataChanged:Uplink capacity in kbps = %u",
                        LCEData.uplinkCapacityKbps);
                QCRIL_HAL_LOG_DEBUG("Received LCEDataChangeIndication");
            }
        );
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testRadioKeepAliveChanged(RilApiSession* session) {
    if (session) {
        session->registerRadioKeepAliveHandler(
        [] (const Status status, const uint32_t handle, const RIL_KeepaliveStatusCode statuscode) {
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("UnsolRadioKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            QCRIL_HAL_LOG_DEBUG("UnsolRadioKeepAlive:Handle = %" PRIu32, handle);
            QCRIL_HAL_LOG_DEBUG("UnsolRadioKeepAlive:Status = %d", statuscode);
            QCRIL_HAL_LOG_DEBUG("Received RadioKeepAliveIndication");
        });
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetLinkCapFilterMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetLinkCapFilterMessage enter ");
        RIL_ReportFilter enable_bit = RIL_ReportFilter::RIL_REPORT_FILTER_ENABLE;
        session->SetLinkCapFilter(enable_bit, [] (RIL_Errno err, const int* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for SetLinkCapFilterMessage request: %d", err);
            if (response) {
                QCRIL_HAL_LOG_DEBUG("SetLinkCapFilter:Response is = %d", *response);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetLinkCapFilterMessage exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolDataNrIconChange(RilApiSession* session) {
    if (session) {
        session->registerDataNrIconChangeHandler(
        [] (const Status status, const  five_g_icon_type icon) {
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("UnsolDataNrIconChange : Failure in socket data read. Exiting testcase");
                return;
            }
            QCRIL_HAL_LOG_DEBUG("UnsolDataNrIconChange: NR Icon Type = %d",
                    static_cast<int>(icon));
            QCRIL_HAL_LOG_DEBUG("Received UnsolDataNrIconChange");
            }
        );
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void PhysicalConfigStructUpdateMessage(RilApiSession* session) {
    if (session) {
        if ( session->registerPhysicalConfigStructHandler(
            [] (const Status status, const  std::vector<RIL_PhysicalChannelConfig> arg) {
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase");
                return;
            }
            for (int j = 0; j < arg.size(); j++) {
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: List content = %d", j);
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: status = %d",
                        static_cast<int>(arg[j].status));
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: BandwidthDownlink = %d",
                        arg[j].cellBandwidthDownlink);
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: Network RAT %d",
                        static_cast<int>(arg[j].rat));
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: Range = %d",
                        static_cast<int>(arg[j].rfInfo.range));
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: ChannelNumber = %d",
                        arg[j].rfInfo.channelNumber);
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: Number of Context Ids = %d",
                        arg[j].num_context_ids);
                for (int i = 0; i < arg[j].num_context_ids; i++) {
                    QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: Cid %d = %d",
                        i, arg[j].contextIds[i]);
                }
                QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: physicalCellId = %d",
                        arg[j].physicalCellId);
            }
            QCRIL_HAL_LOG_DEBUG("Received PhysicalConfigStructUpdate");
            }
        ) == Status::FAILURE) {
            QCRIL_HAL_LOG_DEBUG("PhysicalConfigStructUpdate: Failed Due to Marshalling Failure.");
        }
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testGetDataNrIconTypeMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession GetDataNrIconType enter ");
        session->GetDataNrIconType([] (RIL_Errno err, const Status status, const five_g_icon_type* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for GetDataNrIconType request: %d", err);
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("GetDataNrIconType : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                QCRIL_HAL_LOG_DEBUG("GetDataNrIconType:Nr Icon Type is = %d",
                        static_cast<int>(*response));
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession GetDataNrIconType exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testStopKeepAliveMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession StopKeepAlive enter ");
        int handle = -1;
        std::cout << "RilApiSession StopKeepAlive enter handle id: ";
        std::cin >> handle;
        session->StopKeepAlive(handle, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for StopKeepAlive request: %d", err);
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("StopKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                QCRIL_HAL_LOG_DEBUG("StopKeepAlive:RIL response is = %d",
                        *response);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession StopKeepAlive exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testStartKeepAliveMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession StartKeepAlive enter ");
        RIL_KeepaliveRequest request = {};
        request.type = RIL_KeepaliveType::NATT_IPV4;
        std::string s = "";
        int x = -1;
        std::cout << "StartKeepAlive enter IPV4 source address: ";
        std::cin >> s;
        strlcpy(request.sourceAddress, s.c_str(), (s.length()+1));
        s = "";
        std::cout << "StartKeepAlive enter IPV4 source port: ";
        std::cin >> x;
        request.sourcePort = x;
        x = -1;
        std::cout << "StartKeepAlive enter IPV4 destination address: ";
        std::cin >> s;
        strlcpy(request.destinationAddress, s.c_str(), (s.length()+1));
        std::cout << "StartKeepAlive enter IPV4 destination port: ";
        std::cin >> x;
        request.destinationPort = x;
        x = -1;
        std::cout << "StartKeepAlive enter interval in miliseconds: ";
        std::cin >> x;
        request.maxKeepaliveIntervalMillis = x;
        x = -1;
        std::cout << "StartKeepAlive enter cid: ";
        std::cin >> x;
        request.cid = x;
        session->StartKeepAlive(request, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response,
        const uint32_t* handle, const RIL_KeepaliveStatus_t* statuscode) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for StartKeepAlive request: %d", err);
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("StartKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                QCRIL_HAL_LOG_DEBUG("StartKeepAlive:Response Error is = %d", *response);
            }
            if (handle) {
                QCRIL_HAL_LOG_DEBUG("StartKeepAlive:Handle is = %" PRIu32, *handle);
            }
            if (statuscode) {
                QCRIL_HAL_LOG_DEBUG("StartKeepAlive:Status is = %d", *statuscode);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession StartKeepAlive exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetPreferredDataModemRequestMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetPreferredDataModem enter ");
        int modemId = 0;
        session->SetPreferredDataModem(modemId, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for SetPreferredDataModem request: %d", err);
            if (status == Status::FAILURE) {
                QCRIL_HAL_LOG_DEBUG("SetPreferredDataModem : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                QCRIL_HAL_LOG_DEBUG("SetPreferredDataModem:Response is = %d", *response);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetPreferredDataModem exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetLinkCapRptCriteriaMessage(RilApiSession* session) {
    if (session) {
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetLinkCapRptCriteriaMessage enter ");
        RIL_LinkCapCriteria criteria = {};
        criteria.hysteresisMs = 3000;
        criteria.hysteresisDlKbps = 50;
        criteria.hysteresisUlKbps = 50;
        criteria.thresholdsDownLength = 11;
        criteria.thresholdsDownlinkKbps = (int*) calloc (criteria.thresholdsDownLength, sizeof(int));
        if (criteria.thresholdsDownlinkKbps) {
            criteria.thresholdsDownlinkKbps[0] = 100;
            criteria.thresholdsDownlinkKbps[1] = 500;
            criteria.thresholdsDownlinkKbps[2] = 1000;
            criteria.thresholdsDownlinkKbps[3] = 5000;
            criteria.thresholdsDownlinkKbps[4] = 10000;
            criteria.thresholdsDownlinkKbps[5] = 20000;
            criteria.thresholdsDownlinkKbps[6] = 50000;
            criteria.thresholdsDownlinkKbps[7] = 100000;
            criteria.thresholdsDownlinkKbps[8] = 200000;
            criteria.thresholdsDownlinkKbps[9] = 500000;
            criteria.thresholdsDownlinkKbps[10] = 1000000;
        }
        criteria.thresholdsUpLength = 9;
        criteria.thresholdsUplinkKbps = (int*) calloc (criteria.thresholdsUpLength, sizeof(int));
        if (criteria.thresholdsUplinkKbps) {
            criteria.thresholdsUplinkKbps[0] = 100;
            criteria.thresholdsUplinkKbps[1] = 500;
            criteria.thresholdsUplinkKbps[2] = 1000;
            criteria.thresholdsUplinkKbps[3] = 5000;
            criteria.thresholdsUplinkKbps[4] = 10000;
            criteria.thresholdsUplinkKbps[5] = 20000;
            criteria.thresholdsUplinkKbps[6] = 50000;
            criteria.thresholdsUplinkKbps[7] = 100000;
            criteria.thresholdsUplinkKbps[8] = 200000;
        }
        criteria.ran = RIL_RAN::LTE_RAN;
        session->SetLinkCapRptCriteria(criteria, [] (RIL_Errno err, const RIL_LinkCapCriteriaResult* response) -> void {
            QCRIL_HAL_LOG_DEBUG("Got response for SetLinkCapRptCriteriaMessage request: %d", err);
            if (response) {
                QCRIL_HAL_LOG_DEBUG("SetLinkCapRptCriteria:Response is = %d", *response);
            }
            });
        QCRIL_HAL_LOG_DEBUG("RilApiSession SetLinkCapRptCriteriaMessage exit ");
    }
    else {
        QCRIL_HAL_LOG_DEBUG("RilApiSession is NULL. Exiting Testcase");
    }
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
    QCRIL_HAL_LOG_DEBUG("RilApiSession sample usage");
    RilApiSession session("/dev/socket/qcrild/rild0");
    Status s = session.initialize(
        [] (Status s) {
            QCRIL_HAL_LOG_DEBUG("Error detected in session. Error: %d", static_cast<int>(s));
        }
    );

    if (s != Status::SUCCESS) {
        QCRIL_HAL_LOG_DEBUG("Failed to initialize API session.");
    }

    RIL_Dial dialParams;
    std::string addr("0123456789");
    dialParams.address = const_cast<char*>(addr.c_str());
    dialParams.clir = 0;
    dialParams.uusInfo = nullptr;

    testUnsolDataCallListChanged(&session);
    testUnsolLCEDataChanged(&session);
    testUnsolPcoDataChanged(&session);
    testUnsolDataNrIconChange(&session);
    testRadioKeepAliveChanged(&session);
    // Disable this while loop for other testing .Data only test use below while loop
    while(true) {
        int x = 0;
        std::cout << "=======AVAILABLE OPTIONS===========" << std::endl;
        std::cout << "1  :: testSetInitialAttachApn" << std::endl;
        std::cout << "2  :: testSetDataProfile" << std::endl;
        std::cout << "3  :: testDataCall" << std::endl;
        std::cout << "4  :: testDeactivateCall" << std::endl;
        std::cout << "5  :: getDataCallList" << std::endl;
        std::cout << "6  :: StartLCE" << std::endl;
        std::cout << "7  :: StopLCE" << std::endl;
        std::cout << "8  :: PullLCE" << std::endl;
        std::cout << "9  :: SetLinkCapFilter" << std::endl;
        std::cout << "10 :: SetLinkCapRptCriteria" << std::endl;
        std::cout << "11 :: GetDataNrIconType" << std::endl;
        std::cout << "12 :: SetPreferredDataModem" << std::endl;
        std::cout << "13 :: StartKeepAlive" << std::endl;
        std::cout << "14 :: StopKeepAlive" << std::endl;
        std::cout << "Enter the option: ";
        std::cin >> x;
        std::cout << "input choice selected ===> " << x << std::endl;
        switch (x) {
            case 1:
                QCRIL_HAL_LOG_DEBUG("Choice is 1 -testSetInitialAttachApn");
                testSetInitialAttachApn(&session);
                break;
            case 2:
                QCRIL_HAL_LOG_DEBUG("Choice is 2 -SetDataProfile");
                testSetDataProfile(&session);
                break;
            case 3:
                QCRIL_HAL_LOG_DEBUG("Choice is 3 testDataCall");
                testDataCall(&session);
                break;
            case 4:
                QCRIL_HAL_LOG_DEBUG("Choice is 4 testDeactivateCall");
                testDeactivateCall(&session);
                break;
            case 5:
                QCRIL_HAL_LOG_DEBUG("Choice is 5 get testGetDataCallList");
                testGetDataCallList(&session);
                break;
            case 6:
                QCRIL_HAL_LOG_DEBUG("Choice is 6 get testStartLceData");
                testStartLceData(&session);
                break;
            case 7:
                QCRIL_HAL_LOG_DEBUG("Choice is 7 get testStopLceData");
                testStopLceData(&session);
                break;
            case 8:
                QCRIL_HAL_LOG_DEBUG("Choice is 8 get testPullLceData");
                testPullLceData(&session);
                break;
            case 9:
                QCRIL_HAL_LOG_DEBUG("Choice is 9 SetLinkCapFilter");
                testSetLinkCapFilterMessage(&session);
                break;
            case 10:
                QCRIL_HAL_LOG_DEBUG("Choice is 10 SetLinkCapRptCriteria");
                testSetLinkCapRptCriteriaMessage(&session);
                break;
            case 11:
                QCRIL_HAL_LOG_DEBUG("Choice is 11 GetDataNrIconType");
                testGetDataNrIconTypeMessage(&session);
                break;
            case 12:
                QCRIL_HAL_LOG_DEBUG("Choice is 12 SetPreferredDataModem");
                testSetPreferredDataModemRequestMessage(&session);
                break;
            case 13:
                QCRIL_HAL_LOG_DEBUG("Choice is 13 StartKeepAlive");
                testStartKeepAliveMessage(&session);
                break;
            case 14:
                QCRIL_HAL_LOG_DEBUG("Choice is 14 StopKeepAlive");
                testStopKeepAliveMessage(&session);
                break;
            default:
                QCRIL_HAL_LOG_DEBUG("Choice other than 1 to 14");
                break;
        }
        sleep(60);
    }

    while (true) {
        session.dial(
            dialParams,
            [] (RIL_Errno err) -> void {
                QCRIL_HAL_LOG_DEBUG("Got response for dial request: %d", err);
            }
        );

        session.registerSignalStrengthIndicationHandler(
            [] (const RIL_SignalStrength& signalStrength) {
                (void) signalStrength;
                QCRIL_HAL_LOG_DEBUG("Received signal strength indication");
            }
        );

        session.registerCellInfoIndicationHandler(
            [] (const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
                (void) cellInfoList;
                (void) cellInfoListLen;
                QCRIL_HAL_LOG_DEBUG("Received cell info indication");
            }
        );

        session.getCellInfo(
            [] (RIL_Errno err, const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
                QCRIL_HAL_LOG_DEBUG("Got response for get cell info request: %d", err);

                if (cellInfoList == nullptr || cellInfoListLen == 0) {
                    QCRIL_HAL_LOG_DEBUG("No cell info received.");
                    return;
                } else {
                    QCRIL_HAL_LOG_DEBUG("%zu elements in the cell info array.", cellInfoListLen);
                }

                std::for_each(
                    cellInfoList,
                    cellInfoList + cellInfoListLen,
                    [] (const RIL_CellInfo_v12& cellInfo) {
                        const char* mcc = nullptr;
                        const char* mnc = nullptr;

                        switch (cellInfo.cellInfoType) {
                            case RIL_CELL_INFO_TYPE_GSM:
                                mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                                mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_LTE:
                                mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                                mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_WCDMA:
                                mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                                mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                                mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                                mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                                break;
                            default:
                                break;
                        }

                        QCRIL_HAL_LOG_DEBUG("cellInfoType: %d, registered: %d, mcc: %s, mnc: %s",
                                cellInfo.cellInfoType, cellInfo.registered,
                                (mcc ? mcc : ""), (mnc ? mnc : ""));
                    }
                );
            }
        );

        sleep(60);
    }
}
