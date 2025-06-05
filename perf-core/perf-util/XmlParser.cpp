/******************************************************************************
  @file  XmlParser.cpp`
  @brief  xmlparser module to parse xml files and notify registered clients

  xmlparser module to parse xml files and notify registered clients

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-XMLPARSER"
#include "XmlParser.h"

#include <unistd.h>
#include <exception>
#include <cstring>
#include "PerfLog.h"

using namespace std;

mutex AppsListXmlParser::mMutex;
AppsListXmlParser::AppsListXmlParser() {
}

AppsListXmlParser::~AppsListXmlParser() {
}

int8_t AppsListXmlParser::Register(const string &xmlRoot,
                                const string &xmlChildToParse,
                                void (*parseElem)(xmlNodePtr, void *),
                                void *data) {
    int8_t ret = -1;
    if (NULL == parseElem) {
        QLOGE(LOG_TAG, "No parsing function specified for XML");
        return -1;
    }
    try {
        lock_guard<mutex> guard(mMutex);
        for (uint8_t i=0; i<MAX_CALLBACKS; i++) {
            if (!mParserClients[i].mRegistered) {
                mParserClients[i].mParseElem = parseElem;
                mXmlRoot = xmlRoot;
                mParserClients[i].mXmlChildToParse = xmlChildToParse;
                mParserClients[i].mData = data;
                mParserClients[i].mRegistered = true;
                ret = i;
                break;
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Caught exception: %s in %s",e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Error in %s",__func__);
    }
    return ret;
}

bool AppsListXmlParser::DeRegister(int8_t idx){
    bool rc = false;
    if (idx <0) {
        QLOGE(LOG_TAG, "Could do not de-register the callback");
        return rc;
    }
    try {
        lock_guard<mutex> guard(mMutex);
        mParserClients[idx].mParseElem = NULL;
        mParserClients[idx].mXmlChildToParse = "";
        mParserClients[idx].mRegistered = false;
        rc = true;
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Caught exception: %s in %s",e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Error in %s",__func__);
    }
    return rc;
}

int8_t AppsListXmlParser::Parse(const string &fName) {
    xmlDocPtr doc;
    xmlNodePtr currNode;
    int8_t idx = -1;

    if (access(fName.c_str(), F_OK) < 0) {
        QLOGE(LOG_TAG, "Could not access the XML file at %s", fName.c_str());
        return -1;
    }
    try {
        lock_guard<mutex> guard(mMutex);
        doc = xmlReadFile(fName.c_str(), "UTF-8", XML_PARSE_RECOVER);
        if(!doc) {
            QLOGE(LOG_TAG, "XML Document not parsed successfully");
            return -1;
        }

        currNode = xmlDocGetRootElement(doc);
        if(!currNode) {
            QLOGE(LOG_TAG, "Empty document");
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return -1;
        }

        // Confirm the root-element of the tree
        if(!IsRoot(currNode->name)) {
            QLOGE(LOG_TAG, "Document of the wrong type, root node != root");
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return -1;
        }

        currNode = currNode->xmlChildrenNode;

        for(; currNode != NULL; currNode=currNode->next) {
            if(currNode->type != XML_ELEMENT_NODE)
                continue;

            QLOGV(LOG_TAG, "Parsing the xml of %s",currNode->name);

            xmlNodePtr node = currNode;

            idx = FindRootsChildInRegisteredClients(currNode->name);
            if(idx >= 0) {
                node = node->children;
                while(node != NULL) {
                    //Call the function pointer to populate
                    mParserClients[idx].mParseElem(node, mParserClients[idx].mData);
                    QLOGV(LOG_TAG, "parsed the node moving on to next");
                    node = node->next;
                }
            }
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Caught exception: %s in %s",e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Error in %s",__func__);
    }
    return 0;
}

bool AppsListXmlParser::IsRoot(const xmlChar* root) {
    bool ret = false;
    if (!xmlStrcmp(root, BAD_CAST mXmlRoot.c_str())) {
            ret = true;
    }
    return ret;
}

int8_t AppsListXmlParser::FindRootsChildInRegisteredClients(const xmlChar* child) {
    int8_t ret = -1;
    for (uint8_t i=0; i<MAX_CALLBACKS; i++) {
        //if checks for mParseElem
        if (!xmlStrcmp(child, BAD_CAST mParserClients[i].mXmlChildToParse.c_str())) {
            ret = i;
        }
    }
    return ret;
}
