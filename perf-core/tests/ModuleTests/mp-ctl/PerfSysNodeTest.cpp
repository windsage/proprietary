/******************************************************************************
  @file    PerfSysNodeTest.cpp
  @brief   Implementation of PerfSysNodeTest

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF"
#define LOG_TAG_AVC "AVC-TEST-SYSNODES"
#include "PerfLog.h"
#include "config.h"
#include <stdio.h>
#include "PerfSysNodeTest.h"
#include "RegressionFramework.h"

#define RESOURCE_CONFIGS_XML_ROOT "ResourceConfigs"
#define RESOURCE_CONFIGS_XML_CHILD_CONFIG "PerfResources"
#define RESOURCE_CONFIGS_XML_SYSNODE_TAG "SysNode"
#define RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG "Idx"
#define RESOURCE_CONFIGS_XML_ELEM_PERMISSION_TAG "Perm"
#define RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG "WVal"
#define RESOURCE_CONFIGS_XML_ELEM_READ_TAG "RVal"
#define AVC_SYSNODES_CONFIGS_XML (VENDOR_DIR"/perf/avcsysnodesconfigs.xml")
#define AVC_TARGET_SYSNODES_CONFIGS_XML (VENDOR_DIR"/perf/targetavcsysnodesconfigs.xml")

#define BUFFER_LEN 128

PerfSysNodeTest PerfSysNodeTest::mPerfSysNodeTest;

avcSysNodesConfigInfo::avcSysNodesConfigInfo(int16_t idx, int32_t perm, char *wVal, char *rVal) {
    PerfDataStore *store = PerfDataStore::getPerfDataStore();

    mIndex = idx;
    mPermission = perm;

    memset(mNodePath, 0, sizeof(mNodePath));
    store->GetSysNode(mIndex, mNodePath, mSupported);

    memset(mWriteValue, 0, sizeof(mWriteValue));
    memset(mReadValue, 0, sizeof(mReadValue));
    if ((wVal != NULL) && (strlen(wVal) > 0)) {
        strlcpy(mWriteValue, wVal, NODE_MAX);
        strlcpy(mReadValue, wVal, NODE_MAX);
    }
    if ((rVal != NULL) && (strlen(rVal) > 0)) {
        strlcpy(mReadValue, rVal, NODE_MAX);
    }
}

PerfSysNodeTest::~PerfSysNodeTest() {
}

char* getPassFail(bool rc, char* ptr) {
    if (rc == true) {
        snprintf(ptr,10,"PASSED");
    } else {
        snprintf(ptr,10,"FAILED");
    }
    return ptr;
}

int8_t checkNodeExist(const char * node_path) {
    if (access(node_path, F_OK) == 0) {
        return SUCCESS;
    }
    return FAILED;
}

PerfSysNodeTest::PerfSysNodeTest() {
    mTestEnabled = false;
    if (InitAvcSysNodesXML() == SUCCESS) {
        mTestEnabled = true;
    }
}

int8_t PerfSysNodeTest::InitAvcSysNodesXML() {
    const string xmlResourceConfigRoot(RESOURCE_CONFIGS_XML_ROOT);
    const string xmlChildResourceConfig(RESOURCE_CONFIGS_XML_CHILD_CONFIG);
    const string fAVCSysNodesName(AVC_SYSNODES_CONFIGS_XML);
    const string ftargetAVCSysNodesName(AVC_TARGET_SYSNODES_CONFIGS_XML);
    int8_t idnum = -1;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return FAILED;
    }

    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, avcSysNodesCB, NULL);
    xmlParser->Parse(fAVCSysNodesName);
    xmlParser->DeRegister(idnum);

    idnum = -1;
    idnum = xmlParser->Register(xmlResourceConfigRoot, xmlChildResourceConfig, avcSysNodesCB, NULL);
    xmlParser->Parse(ftargetAVCSysNodesName);
    xmlParser->DeRegister(idnum);
    delete xmlParser;
    return SUCCESS;
}

void PerfSysNodeTest::avcSysNodesCB(xmlNodePtr node, void *) {
    int32_t idx_value = -1;
    char *nodePath = NULL;
    int32_t permission = 0;
    char *writeValue = NULL;
    char *readValue = NULL;

    //Parsing the SysNode tag.
    if (!xmlStrcmp(node->name, BAD_CAST RESOURCE_CONFIGS_XML_SYSNODE_TAG)) {
        // Parsing Idx
        if (!xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG)) {
            return;
        }
        idx_value = PerfDataStore::ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_INDEX_TAG, idx_value);

        // Parsing Perm
        if(xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_PERMISSION_TAG)) {
            permission = PerfDataStore::ConvertNodeValueToInt(node, RESOURCE_CONFIGS_XML_ELEM_PERMISSION_TAG, permission);
        }

        // Parsing Wval
        if(xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG)) {
            writeValue = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_WRITE_TAG);
        }
        if(xmlHasProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_READ_TAG)) {
            readValue = (char *)xmlGetProp(node, BAD_CAST RESOURCE_CONFIGS_XML_ELEM_READ_TAG);
        }

        QLOGI(LOG_TAG, "Identified resource with index_value=%" PRId32 " Permission=%" PRId32 " WriteValue=%s", idx_value, permission, writeValue);
        mPerfSysNodeTest.mAVCSysNodesConfig.insert_or_assign(idx_value, avcSysNodesConfigInfo(idx_value, permission, writeValue, readValue));

        if (writeValue != NULL) {
            xmlFree(writeValue);
        }

        if (readValue != NULL) {
            xmlFree(readValue);
        }
    }
}

int8_t sys_nodes_test(const char *filename) {

    int8_t res = FAILED;
    if (filename == NULL) {
        QLOGE(LOG_TAG_AVC, "Invalid FileName received for %s",__func__);
        return res;
    }
    int8_t rc = FAILED;
    FILE *defval = NULL;
    char outfile[OUT_LINE_BUF] = {0};
    char rc_s[NODE_MAX] = "";
    char read_out[NODE_MAX] = "";
    char write_out[NODE_MAX] = "";

    PerfSysNodeTest &mPSNT = PerfSysNodeTest::getPerfSysNodeTest();
    map<uint16_t, avcSysNodesConfigInfo> &xmlFilesContainer = mPSNT.GetAllSysNodes();
    FILE * filePtr = NULL;
    char readVal[BUFFER_LEN] = {0};
    bool failureFlag = false;
    uint32_t scsfulChecks = xmlFilesContainer.size();
    if (scsfulChecks <= 0) {
        QLOGD(LOG_TAG_AVC, "No Nodes to check in the xmlfile ..");
        return FAILED;
    }

    defval = fopen(filename, "w+");
    if (defval == NULL) {
        QLOGE(LOG_TAG_AVC, "Cannot open/create default values file\n");
        return FAILED;
    }
    snprintf(outfile, OUT_LINE_BUF, "Index|SysNode|Permissions|WVal|RVal|Supported|What Failed|RESULT\n");
    fwrite(outfile, sizeof(char), strlen(outfile), defval);
    res = SUCCESS;
    for (uint8_t idx = 0; idx < SYSNODE_END; idx++) {
        if (idx == SCROLL_NOTIF) {
            continue;
        }
        memset(read_out, 0, NODE_MAX);
        memset(write_out, 0, NODE_MAX);
        memset(outfile, 0, NODE_MAX);
        auto element = xmlFilesContainer.find(idx);
        if (element == xmlFilesContainer.end()) {
            //node does not exist in test xml fail
            rc = FAILED;
            res = FAILED;
            snprintf(read_out, NODE_MAX,"Node does not exist in test XML");
            snprintf(write_out, NODE_MAX,"READ WRITE");
            snprintf(outfile, OUT_LINE_BUF, "%" PRIu8 "|%s|-|-|-|-|%s|%s\n",
                    idx, read_out, write_out,
                    getPassFail(rc == SUCCESS, rc_s));
        } else {
            rc = SUCCESS;
            snprintf(read_out, NODE_MAX,"");
            snprintf(write_out, NODE_MAX,"");
            avcSysNodesConfigInfo &fileElement = element->second;

            QLOGD(LOG_TAG_AVC, "Index = %" PRId16 ", Path = %s, Permission = %" PRId32 ", writeVal = %s",
                    fileElement.mIndex, fileElement.mNodePath,
                    fileElement.mPermission, fileElement.mWriteValue);
            failureFlag = false;
            if (fileElement.mSupported == false) {
                rc = SUCCESS;
                // node is not supported on target pass
            } else {
                /* Block to check the permissions of Files  - START*/
                if (checkNodeExist(fileElement.mNodePath) == FAILED) {
                    rc = FAILED;
                    snprintf(read_out, NODE_MAX,"Node Does not Exist on Target");
                } else {
                    int32_t perms = fileElement.mPermission;
                    if ((perms & 4) != 0) {
                        filePtr = fopen(fileElement.mNodePath, "r");
                        if (filePtr == NULL) {
                            QLOGE(LOG_TAG_AVC, "Could not open the file = %s for reading. Err : %s", fileElement.mNodePath, strerror(errno));
                            failureFlag = true;
                        } else {
                            fclose(filePtr);
                        }
                        if (failureFlag == true) {
                            rc = FAILED;
                            snprintf(read_out, NODE_MAX,"READ");
                        }
                    } if ((perms & 2) != 0) {
                        filePtr = fopen(fileElement.mNodePath, "w");
                        if (filePtr == NULL) {
                            QLOGE(LOG_TAG_AVC, "Could not open the file = %s for writing. Err : %s",
                                    fileElement.mNodePath, strerror(errno));
                            failureFlag = true;
                        } else {
                            if(fileElement.mWriteValue[0] != '\0' ) {
                                fputs(fileElement.mWriteValue, filePtr);
                            }
                            fclose(filePtr);
                            /* Confirm the value written to file. */
                            filePtr = fopen(fileElement.mNodePath, "r");
                            if (filePtr != NULL) {
                                fgets(readVal, BUFFER_LEN, filePtr);
                                uint32_t len = strlen(readVal);
                                if (len > 0) {
                                    readVal[len - 1] = '\0'; // Removing the New line inserted by fgets.
                                    QLOGD(LOG_TAG_AVC, "read value = %s, write value = %s", readVal, fileElement.mWriteValue);
                                }
                                if (strncmp(readVal, fileElement.mReadValue, BUFFER_LEN) == 0) {
                                    QLOGD(LOG_TAG_AVC, "Write completed successfully.");
                                } else {
                                    QLOGE(LOG_TAG_AVC, "Written value = %s is not reflected in file = %s. Err : %s",
                                            fileElement.mWriteValue, fileElement.mNodePath, strerror(errno));
                                    failureFlag = true;
                                }
                                fclose(filePtr);
                            } else {
                                QLOGW(LOG_TAG_AVC, "Could not open the the file = %s for reading after writing. Err : %s",
                                        fileElement.mNodePath, strerror(errno)); // This need not be Err. A file can have only 'w' perm, but not 'r'.
                                failureFlag = true;
                            }
                        }
                        if (failureFlag == true) {
                            rc = FAILED;
                            snprintf(write_out, NODE_MAX,"WRITE");
                        }
                    }
                }

            }
            /* Block to check the permissions of Files  - END*/
            if (failureFlag) {
                scsfulChecks--;
            }
            snprintf(outfile, OUT_LINE_BUF,"0x%" PRIX8 "|%s|%" PRId32 "|%s|%s|%" PRId8 "|%s %s|%s\n",
                    idx, fileElement.mNodePath, fileElement.mPermission,
                    fileElement.mWriteValue, fileElement.mReadValue, fileElement.mSupported,
                    read_out, write_out,
                    getPassFail(rc == SUCCESS, rc_s));
        }
        fwrite(outfile, sizeof(char), strlen(outfile), defval);
        if (rc == FAILED) {
            res = rc;
        }
    }
    if (rc == FAILED) {
        res = rc;
    }
    fclose(defval);
    QLOGE(LOG_TAG_AVC, "Check %s for Result\n", filename);
    return res;
}
