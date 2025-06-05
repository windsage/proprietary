/*
* Copyright (c) 2018 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdGetCTLTPCFTMData.s
#include "tlv2Inc.h"
#include "cmdGetCTLTPCFTMData.h"

void* initGETCTLTPCFTMDATAOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_GETCTLTPCFTMDATA_PARMS  *pGETCTLTPCFTMDATAParms = (CMD_GETCTLTPCFTMDATA_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pGETCTLTPCFTMDATAParms->ctlPowerSetIndex = pParmDict[PARM_CTLPOWERSETINDEX].v.valU16;
    pGETCTLTPCFTMDATAParms->ctlVerbose = pParmDict[PARM_CTLVERBOSE].v.valU8;
    pGETCTLTPCFTMDATAParms->numChain = pParmDict[PARM_NUMCHAIN].v.valU8;
    pGETCTLTPCFTMDATAParms->nss = pParmDict[PARM_NSS].v.valU32;
    pGETCTLTPCFTMDATAParms->beamForming = pParmDict[PARM_BEAMFORMING].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CTLPOWERSETINDEX, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATAParms->ctlPowerSetIndex)) - (A_UINT32)pGETCTLTPCFTMDATAParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLVERBOSE, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATAParms->ctlVerbose)) - (A_UINT32)pGETCTLTPCFTMDATAParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_NUMCHAIN, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATAParms->numChain)) - (A_UINT32)pGETCTLTPCFTMDATAParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_NSS, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATAParms->nss)) - (A_UINT32)pGETCTLTPCFTMDATAParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_BEAMFORMING, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATAParms->beamForming)) - (A_UINT32)pGETCTLTPCFTMDATAParms), pParmsOffset);
    return((void*) pGETCTLTPCFTMDATAParms);
}

static GETCTLTPCFTMDATA_OP_FUNC GETCTLTPCFTMDATAOpFunc = NULL;

TLV2_API void registerGETCTLTPCFTMDATAHandler(GETCTLTPCFTMDATA_OP_FUNC fp)
{
    GETCTLTPCFTMDATAOpFunc = fp;
}

A_BOOL GETCTLTPCFTMDATAOp(void *pParms)
{
    CMD_GETCTLTPCFTMDATA_PARMS *pGETCTLTPCFTMDATAParms = (CMD_GETCTLTPCFTMDATA_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("GETCTLTPCFTMDATAOp: ctlPowerSetIndex %u\n", pGETCTLTPCFTMDATAParms->ctlPowerSetIndex);
    A_PRINTF("GETCTLTPCFTMDATAOp: ctlVerbose %u\n", pGETCTLTPCFTMDATAParms->ctlVerbose);
    A_PRINTF("GETCTLTPCFTMDATAOp: numChain %u\n", pGETCTLTPCFTMDATAParms->numChain);
    A_PRINTF("GETCTLTPCFTMDATAOp: nss %u\n", pGETCTLTPCFTMDATAParms->nss);
    A_PRINTF("GETCTLTPCFTMDATAOp: beamForming %u\n", pGETCTLTPCFTMDATAParms->beamForming);
#endif //_DEBUG

    if (NULL != GETCTLTPCFTMDATAOpFunc) {
        (*GETCTLTPCFTMDATAOpFunc)(pGETCTLTPCFTMDATAParms);
    }
    return(TRUE);
}

void* initGETCTLTPCFTMDATARSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_GETCTLTPCFTMDATARSP_PARMS  *pGETCTLTPCFTMDATARSPParms = (CMD_GETCTLTPCFTMDATARSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pGETCTLTPCFTMDATARSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pGETCTLTPCFTMDATARSPParms->beamForming = pParmDict[PARM_BEAMFORMING].v.valU8;
    pGETCTLTPCFTMDATARSPParms->chainMask = pParmDict[PARM_CHAINMASK].v.valU16;
    pGETCTLTPCFTMDATARSPParms->nss = pParmDict[PARM_NSS].v.valU32;
    pGETCTLTPCFTMDATARSPParms->numChain = pParmDict[PARM_NUMCHAIN].v.valU8;
    pGETCTLTPCFTMDATARSPParms->ctlChainPower = pParmDict[PARM_CTLCHAINPOWER].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlFinalTotalPower = pParmDict[PARM_CTLFINALTOTALPOWER].v.valS8;
    pGETCTLTPCFTMDATARSPParms->r2pPower = pParmDict[PARM_R2PPOWER].v.valS8;
    pGETCTLTPCFTMDATARSPParms->sarPower = pParmDict[PARM_SARPOWER].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlExceptionValue = pParmDict[PARM_CTLEXCEPTIONVALUE].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlExceptionValue2 = pParmDict[PARM_CTLEXCEPTIONVALUE2].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlExceptionValueApplied = pParmDict[PARM_CTLEXCEPTIONVALUEAPPLIED].v.valS8;
    pGETCTLTPCFTMDATARSPParms->regulatoryPowerLimit = pParmDict[PARM_REGULATORYPOWERLIMIT].v.valS8;
    pGETCTLTPCFTMDATARSPParms->regulatoryPSDLimit = pParmDict[PARM_REGULATORYPSDLIMIT].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlAntElementGain = pParmDict[PARM_CTLANTELEMENTGAIN].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlAddedMargin = pParmDict[PARM_CTLADDEDMARGIN].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlArrayGain = pParmDict[PARM_CTLARRAYGAIN].v.valS8;
    pGETCTLTPCFTMDATARSPParms->ctlExceptionValueAdjust = pParmDict[PARM_CTLEXCEPTIONVALUEADJUST].v.valS8;
    pGETCTLTPCFTMDATARSPParms->hcOffset = pParmDict[PARM_HCOFFSET].v.valU8;
    pGETCTLTPCFTMDATARSPParms->heOffset = pParmDict[PARM_HEOFFSET].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->status)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_BEAMFORMING, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->beamForming)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAINMASK, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->chainMask)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_NSS, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->nss)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_NUMCHAIN, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->numChain)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLCHAINPOWER, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlChainPower)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLFINALTOTALPOWER, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlFinalTotalPower)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_R2PPOWER, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->r2pPower)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SARPOWER, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->sarPower)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEXCEPTIONVALUE, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlExceptionValue)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEXCEPTIONVALUE2, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlExceptionValue2)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEXCEPTIONVALUEAPPLIED, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlExceptionValueApplied)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_REGULATORYPOWERLIMIT, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->regulatoryPowerLimit)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_REGULATORYPSDLIMIT, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->regulatoryPSDLimit)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLANTELEMENTGAIN, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlAntElementGain)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLADDEDMARGIN, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlAddedMargin)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLARRAYGAIN, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlArrayGain)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEXCEPTIONVALUEADJUST, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->ctlExceptionValueAdjust)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_HCOFFSET, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->hcOffset)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_HEOFFSET, (A_UINT32)(((A_UINT32)&(pGETCTLTPCFTMDATARSPParms->heOffset)) - (A_UINT32)pGETCTLTPCFTMDATARSPParms), pParmsOffset);
    return((void*) pGETCTLTPCFTMDATARSPParms);
}

static GETCTLTPCFTMDATARSP_OP_FUNC GETCTLTPCFTMDATARSPOpFunc = NULL;

TLV2_API void registerGETCTLTPCFTMDATARSPHandler(GETCTLTPCFTMDATARSP_OP_FUNC fp)
{
    GETCTLTPCFTMDATARSPOpFunc = fp;
}

A_BOOL GETCTLTPCFTMDATARSPOp(void *pParms)
{
    CMD_GETCTLTPCFTMDATARSP_PARMS *pGETCTLTPCFTMDATARSPParms = (CMD_GETCTLTPCFTMDATARSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("GETCTLTPCFTMDATARSPOp: status %u\n", pGETCTLTPCFTMDATARSPParms->status);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: beamForming %u\n", pGETCTLTPCFTMDATARSPParms->beamForming);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: chainMask %u\n", pGETCTLTPCFTMDATARSPParms->chainMask);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: nss %u\n", pGETCTLTPCFTMDATARSPParms->nss);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: numChain %u\n", pGETCTLTPCFTMDATARSPParms->numChain);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlChainPower %u\n", pGETCTLTPCFTMDATARSPParms->ctlChainPower);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlFinalTotalPower %u\n", pGETCTLTPCFTMDATARSPParms->ctlFinalTotalPower);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: r2pPower %u\n", pGETCTLTPCFTMDATARSPParms->r2pPower);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: sarPower %u\n", pGETCTLTPCFTMDATARSPParms->sarPower);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlExceptionValue %u\n", pGETCTLTPCFTMDATARSPParms->ctlExceptionValue);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlExceptionValue2 %u\n", pGETCTLTPCFTMDATARSPParms->ctlExceptionValue2);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlExceptionValueApplied %u\n", pGETCTLTPCFTMDATARSPParms->ctlExceptionValueApplied);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: regulatoryPowerLimit %u\n", pGETCTLTPCFTMDATARSPParms->regulatoryPowerLimit);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: regulatoryPSDLimit %u\n", pGETCTLTPCFTMDATARSPParms->regulatoryPSDLimit);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlAntElementGain %u\n", pGETCTLTPCFTMDATARSPParms->ctlAntElementGain);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlAddedMargin %u\n", pGETCTLTPCFTMDATARSPParms->ctlAddedMargin);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlArrayGain %u\n", pGETCTLTPCFTMDATARSPParms->ctlArrayGain);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: ctlExceptionValueAdjust %u\n", pGETCTLTPCFTMDATARSPParms->ctlExceptionValueAdjust);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: hcOffset %u\n", pGETCTLTPCFTMDATARSPParms->hcOffset);
    A_PRINTF("GETCTLTPCFTMDATARSPOp: heOffset %u\n", pGETCTLTPCFTMDATARSPParms->heOffset);
#endif //_DEBUG

    if (NULL != GETCTLTPCFTMDATARSPOpFunc) {
        (*GETCTLTPCFTMDATARSPOpFunc)(pGETCTLTPCFTMDATARSPParms);
    }
    return(TRUE);
}
