/*
* Copyright (c) 2018 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdGetCTLTPCFTMDataStandalone.s
#ifndef _CMDGETCTLTPCFTMDATASTANDALONE_H_
#define _CMDGETCTLTPCFTMDATASTANDALONE_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

#define CTL_ROW_DATA_SIZE    512
typedef struct getctltpcftmdatastandalone_parms {
    A_UINT8	ctlRd;
    A_UINT8	ctlGroup;
    A_UINT8	ctlVerbose;
    A_UINT8	ctlNonOfdmaType;
    A_UINT16	freq;
    A_UINT16	rateBitIndex;
    A_UINT32	nss;
    A_UINT16	ctlEbw;
    A_UINT8	beamForming;
    A_UINT8	numChain;
    A_INT8	ctlExceptionValue;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_GETCTLTPCFTMDATASTANDALONE_PARMS;

typedef struct getctltpcftmdatastandalonersp_parms {
    A_UINT8	status;
    A_UINT8	pad3[3];
    A_UINT32	numRows;
    A_UINT32	eachRowSize;
    A_UINT8	ctlRowData[CTL_ROW_DATA_SIZE];
} __ATTRIB_PACK CMD_GETCTLTPCFTMDATASTANDALONERSP_PARMS;

typedef void (*GETCTLTPCFTMDATASTANDALONE_OP_FUNC)(void *pParms);
typedef void (*GETCTLTPCFTMDATASTANDALONERSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initGETCTLTPCFTMDATASTANDALONEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLTPCFTMDATASTANDALONEOp(void *pParms);

void* initGETCTLTPCFTMDATASTANDALONERSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLTPCFTMDATASTANDALONERSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDGETCTLTPCFTMDATASTANDALONE_H_
