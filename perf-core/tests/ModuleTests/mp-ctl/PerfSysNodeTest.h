/******************************************************************************
  @file    PerfSysNodeTest.h
  @brief   Header for PerfSysNodeTest.cpp

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef __PERFSYSNODETEST__
#define __PERFSYSNODETEST__
#include <map>
#include "BoostConfigReader.h"


class avcSysNodesConfigInfo {
    public:
        explicit avcSysNodesConfigInfo(int16_t idx, int32_t perm, char * wVal, char *rVal);
        bool mSupported;
        int16_t mIndex;
        char mNodePath[NODE_MAX];
        int32_t mPermission;
        char mWriteValue[NODE_MAX];
        char mReadValue[NODE_MAX];
};

class PerfSysNodeTest {
    private:
        //ctor, copy ctor, assignment overloading
        static PerfSysNodeTest mPerfSysNodeTest;
        PerfSysNodeTest();
        PerfSysNodeTest(PerfSysNodeTest const& oh);
        PerfSysNodeTest& operator=(PerfSysNodeTest const& oh);
        //Avc sysnodes config
        static void avcSysNodesCB(xmlNodePtr node, void *);
        int8_t InitAvcSysNodesXML();
        bool mTestEnabled;
        map<uint16_t, avcSysNodesConfigInfo> mAVCSysNodesConfig;

    public:
        static PerfSysNodeTest &getPerfSysNodeTest() {
            return mPerfSysNodeTest;
        }

        map<uint16_t, avcSysNodesConfigInfo> &GetAllSysNodes() {
            return mAVCSysNodesConfig;
        }
        //Avc sys nodes configs
        ~PerfSysNodeTest();
};

#ifdef __cplusplus
extern "C" {
#endif
    int8_t sys_nodes_test(const char *);
#ifdef __cplusplus
}
#endif

#endif //__PERFSYSNODETEST__
