/*
* Copyright (c) 2018 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdSetCTLTPCTestScreen.s
#include "tlv2Inc.h"
#include "cmdSetCTLTPCTestScreen.h"

void* initSETCTLTPCTESTSCREENOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_SETCTLTPCTESTSCREEN_PARMS  *pSETCTLTPCTESTSCREENParms = (CMD_SETCTLTPCTESTSCREEN_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pSETCTLTPCTESTSCREENParms->ctlRd = pParmDict[PARM_CTLRD].v.valU8;
    pSETCTLTPCTESTSCREENParms->ctlGroup = pParmDict[PARM_CTLGROUP].v.valU8;
    pSETCTLTPCTESTSCREENParms->freq = pParmDict[PARM_FREQ].v.valU16;
    pSETCTLTPCTESTSCREENParms->rateBitIndex = pParmDict[PARM_RATEBITINDEX].v.valU16;
    pSETCTLTPCTESTSCREENParms->ctlDebugFlag = pParmDict[PARM_CTLDEBUGFLAG].v.valU16;
    pSETCTLTPCTESTSCREENParms->ctlNonOfdmaType = pParmDict[PARM_CTLNONOFDMATYPE].v.valU8;
    pSETCTLTPCTESTSCREENParms->ctlEbw = pParmDict[PARM_CTLEBW].v.valU16;
    pSETCTLTPCTESTSCREENParms->wds = pParmDict[PARM_WDS].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CTLRD, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->ctlRd)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLGROUP, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->ctlGroup)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FREQ, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->freq)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RATEBITINDEX, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->rateBitIndex)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLDEBUGFLAG, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->ctlDebugFlag)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLNONOFDMATYPE, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->ctlNonOfdmaType)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEBW, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->ctlEbw)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_WDS, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENParms->wds)) - (A_UINT32)pSETCTLTPCTESTSCREENParms), pParmsOffset);
    return((void*) pSETCTLTPCTESTSCREENParms);
}

static SETCTLTPCTESTSCREEN_OP_FUNC SETCTLTPCTESTSCREENOpFunc = NULL;

TLV2_API void registerSETCTLTPCTESTSCREENHandler(SETCTLTPCTESTSCREEN_OP_FUNC fp)
{
    SETCTLTPCTESTSCREENOpFunc = fp;
}

A_BOOL SETCTLTPCTESTSCREENOp(void *pParms)
{
    CMD_SETCTLTPCTESTSCREEN_PARMS *pSETCTLTPCTESTSCREENParms = (CMD_SETCTLTPCTESTSCREEN_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("SETCTLTPCTESTSCREENOp: ctlRd %u\n", pSETCTLTPCTESTSCREENParms->ctlRd);
    A_PRINTF("SETCTLTPCTESTSCREENOp: ctlGroup %u\n", pSETCTLTPCTESTSCREENParms->ctlGroup);
    A_PRINTF("SETCTLTPCTESTSCREENOp: freq %u\n", pSETCTLTPCTESTSCREENParms->freq);
    A_PRINTF("SETCTLTPCTESTSCREENOp: rateBitIndex %u\n", pSETCTLTPCTESTSCREENParms->rateBitIndex);
    A_PRINTF("SETCTLTPCTESTSCREENOp: ctlDebugFlag %u\n", pSETCTLTPCTESTSCREENParms->ctlDebugFlag);
    A_PRINTF("SETCTLTPCTESTSCREENOp: ctlNonOfdmaType %u\n", pSETCTLTPCTESTSCREENParms->ctlNonOfdmaType);
    A_PRINTF("SETCTLTPCTESTSCREENOp: ctlEbw %u\n", pSETCTLTPCTESTSCREENParms->ctlEbw);
    A_PRINTF("SETCTLTPCTESTSCREENOp: wds %u\n", pSETCTLTPCTESTSCREENParms->wds);
#endif //_DEBUG

    if (NULL != SETCTLTPCTESTSCREENOpFunc) {
        (*SETCTLTPCTESTSCREENOpFunc)(pSETCTLTPCTESTSCREENParms);
    }
    return(TRUE);
}

void* initSETCTLTPCTESTSCREENRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_SETCTLTPCTESTSCREENRSP_PARMS  *pSETCTLTPCTESTSCREENRSPParms = (CMD_SETCTLTPCTESTSCREENRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pSETCTLTPCTESTSCREENRSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pSETCTLTPCTESTSCREENRSPParms->ctlPowerSetSize = pParmDict[PARM_CTLPOWERSETSIZE].v.valU16;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENRSPParms->status)) - (A_UINT32)pSETCTLTPCTESTSCREENRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLPOWERSETSIZE, (A_UINT32)(((A_UINT32)&(pSETCTLTPCTESTSCREENRSPParms->ctlPowerSetSize)) - (A_UINT32)pSETCTLTPCTESTSCREENRSPParms), pParmsOffset);
    return((void*) pSETCTLTPCTESTSCREENRSPParms);
}

static SETCTLTPCTESTSCREENRSP_OP_FUNC SETCTLTPCTESTSCREENRSPOpFunc = NULL;

TLV2_API void registerSETCTLTPCTESTSCREENRSPHandler(SETCTLTPCTESTSCREENRSP_OP_FUNC fp)
{
    SETCTLTPCTESTSCREENRSPOpFunc = fp;
}

A_BOOL SETCTLTPCTESTSCREENRSPOp(void *pParms)
{
    CMD_SETCTLTPCTESTSCREENRSP_PARMS *pSETCTLTPCTESTSCREENRSPParms = (CMD_SETCTLTPCTESTSCREENRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("SETCTLTPCTESTSCREENRSPOp: status %u\n", pSETCTLTPCTESTSCREENRSPParms->status);
    A_PRINTF("SETCTLTPCTESTSCREENRSPOp: ctlPowerSetSize %u\n", pSETCTLTPCTESTSCREENRSPParms->ctlPowerSetSize);
#endif //_DEBUG

    if (NULL != SETCTLTPCTESTSCREENRSPOpFunc) {
        (*SETCTLTPCTESTSCREENRSPOpFunc)(pSETCTLTPCTESTSCREENRSPParms);
    }
    return(TRUE);
}
