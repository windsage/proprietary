/*
* Copyright (c) 2021 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdGetCTLSupportedInfo.s
#ifndef _CMDGETCTLSUPPORTEDINFO_H_
#define _CMDGETCTLSUPPORTEDINFO_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct getctlsupportedinfo_parms {
    A_UINT8	ctlRd;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_GETCTLSUPPORTEDINFO_PARMS;

typedef struct getctlsupportedinforsp_parms {
    A_UINT8	status;
    A_UINT8	supportMaxBW;
    A_UINT16	ctlNumTXChain;
} __ATTRIB_PACK CMD_GETCTLSUPPORTEDINFORSP_PARMS;

typedef void (*GETCTLSUPPORTEDINFO_OP_FUNC)(void *pParms);
typedef void (*GETCTLSUPPORTEDINFORSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initGETCTLSUPPORTEDINFOOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLSUPPORTEDINFOOp(void *pParms);

void* initGETCTLSUPPORTEDINFORSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLSUPPORTEDINFORSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDGETCTLSUPPORTEDINFO_H_
