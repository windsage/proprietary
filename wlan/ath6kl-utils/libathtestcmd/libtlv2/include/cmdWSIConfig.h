/*
* Copyright (c) 2024 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdWSIConfig.s
#ifndef _CMDWSICONFIG_H_
#define _CMDWSICONFIG_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct wsiconfig_parms {
    A_UINT8	phyId;
    A_UINT8	mloNumChips;
    A_UINT8	mloChipInfoIndex;
    A_UINT8	mloChipInfoChipId;
    A_UINT8	mloChipInfoNumLocLinks;
    A_UINT8	mloChipInfoLocLinksIndex;
    A_UINT8	mloChipInfoLocHwLinkId;
    A_UINT8	mloChipInfoLocValidMloLinkId;
    A_UINT16	mloChipId;
    A_UINT8	pad[2];
} __ATTRIB_PACK CMD_WSICONFIG_PARMS;

typedef void (*WSICONFIG_OP_FUNC)(void *pParms);

// Exposed functions

void* initWSICONFIGOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL WSICONFIGOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDWSICONFIG_H_
