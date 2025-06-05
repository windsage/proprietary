/******************************************************************************
  @file  XmlParser.h
  @brief  xmlparser module to parse xml files and notify registered clients

  xmlparser module to parse xml files and notify registered clients

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef __XMLPARSE_H__
#define __XMLPARSE_H__

#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <mutex>

using namespace std;

#define MAX_CALLBACKS 6

typedef void  (*ParseElem)(xmlNodePtr, void *);


class AppsListNode {
public:
    ParseElem mParseElem;
    string mXmlChildToParse;
    void *mData;
    bool mRegistered;
    AppsListNode(){
        mParseElem = NULL;
        mXmlChildToParse = "";
        mData = NULL;
        mRegistered = false;
    }
    ~AppsListNode(){
    }
};

class AppsListXmlParser {
private:
    AppsListNode mParserClients[MAX_CALLBACKS];
    string mXmlRoot;
    static mutex mMutex; /* Thread safe protection */
public:
    AppsListXmlParser();
    ~AppsListXmlParser();
    int8_t Register(const string &xmlRoot, const string &xmlChildToParse,
                 void (*parseElem)(xmlNodePtr, void *), void *data);
    bool DeRegister(int8_t idx);
    int8_t Parse(const string &xmlFName);
private:
    bool IsRoot(const xmlChar* root);
    int8_t FindRootsChildInRegisteredClients(const xmlChar* child);
};
#endif
