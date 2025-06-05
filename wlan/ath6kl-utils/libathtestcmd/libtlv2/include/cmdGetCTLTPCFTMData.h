/*
* Copyright (c) 2018 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdGetCTLTPCFTMData.s
#ifndef _CMDGETCTLTPCFTMDATA_H_
#define _CMDGETCTLTPCFTMDATA_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct getctltpcftmdata_parms {
    A_UINT16	ctlPowerSetIndex;
    A_UINT8	ctlVerbose;
    A_UINT8	numChain;
    A_UINT32	nss;
    A_UINT8	beamForming;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_GETCTLTPCFTMDATA_PARMS;

typedef struct getctltpcftmdatarsp_parms {
    A_UINT8	status;
    A_UINT8	beamForming;
    A_UINT16	chainMask;
    A_UINT32	nss;
    A_UINT8	numChain;
    A_INT8	ctlChainPower;
    A_INT8	ctlFinalTotalPower;
    A_INT8	r2pPower;
    A_INT8	sarPower;
    A_INT8	ctlExceptionValue;
    A_INT8	ctlExceptionValue2;
    A_INT8	ctlExceptionValueApplied;
    A_INT8	regulatoryPowerLimit;
    A_INT8	regulatoryPSDLimit;
    A_INT8	ctlAntElementGain;
    A_INT8	ctlAddedMargin;
    A_INT8	ctlArrayGain;
    A_INT8	ctlExceptionValueAdjust;
    A_UINT8	hcOffset;
    A_UINT8	heOffset;
} __ATTRIB_PACK CMD_GETCTLTPCFTMDATARSP_PARMS;

typedef void (*GETCTLTPCFTMDATA_OP_FUNC)(void *pParms);
typedef void (*GETCTLTPCFTMDATARSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initGETCTLTPCFTMDATAOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLTPCFTMDATAOp(void *pParms);

void* initGETCTLTPCFTMDATARSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL GETCTLTPCFTMDATARSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDGETCTLTPCFTMDATA_H_
