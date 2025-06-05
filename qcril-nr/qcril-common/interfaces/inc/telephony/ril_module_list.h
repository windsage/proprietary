/*
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/
#pragma once
#ifndef RIL_MODULE_LIST_H_INCLUDED
#define RIL_MODULE_LIST_H_INCLUDED
/** RIL_ModuleList
 *  Opaque type used to convey the list of modules that ril can load
 *  on bootup.
 *  Used with RIL_REQUEST_GET_MODULE_LIST
 */
struct RIL_ModuleList;
typedef struct RIL_ModuleList RIL_ModuleList;
extern void rilModuleList_delete(RIL_ModuleList *ml);
extern size_t rilModuleList_getCount(RIL_ModuleList * ml);
extern const char * rilModuleList_getConfigName(RIL_ModuleList * ml);
extern const char *rilModuleList_getName(RIL_ModuleList * ml, size_t idx);

#endif /* RIL_MODULE_LIST_H_INCLUDED */
