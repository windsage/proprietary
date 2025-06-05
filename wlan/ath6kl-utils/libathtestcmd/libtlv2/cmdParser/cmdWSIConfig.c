/*
* Copyright (c) 2024 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdWSIConfig.s
#include "tlv2Inc.h"
#include "cmdWSIConfig.h"

void* initWSICONFIGOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_WSICONFIG_PARMS  *pWSICONFIGParms = (CMD_WSICONFIG_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pWSICONFIGParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pWSICONFIGParms->mloNumChips = pParmDict[PARM_MLONUMCHIPS].v.valU8;
    pWSICONFIGParms->mloChipInfoIndex = pParmDict[PARM_MLOCHIPINFOINDEX].v.valU8;
    pWSICONFIGParms->mloChipInfoChipId = pParmDict[PARM_MLOCHIPINFOCHIPID].v.valU8;
    pWSICONFIGParms->mloChipInfoNumLocLinks = pParmDict[PARM_MLOCHIPINFONUMLOCLINKS].v.valU8;
    pWSICONFIGParms->mloChipInfoLocLinksIndex = pParmDict[PARM_MLOCHIPINFOLOCLINKSINDEX].v.valU8;
    pWSICONFIGParms->mloChipInfoLocHwLinkId = pParmDict[PARM_MLOCHIPINFOLOCHWLINKID].v.valU8;
    pWSICONFIGParms->mloChipInfoLocValidMloLinkId = pParmDict[PARM_MLOCHIPINFOLOCVALIDMLOLINKID].v.valU8;
    pWSICONFIGParms->mloChipId = pParmDict[PARM_MLOCHIPID].v.valU16;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->phyId)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLONUMCHIPS, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloNumChips)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFOINDEX, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoIndex)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFOCHIPID, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoChipId)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFONUMLOCLINKS, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoNumLocLinks)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFOLOCLINKSINDEX, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoLocLinksIndex)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFOLOCHWLINKID, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoLocHwLinkId)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPINFOLOCVALIDMLOLINKID, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipInfoLocValidMloLinkId)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MLOCHIPID, (A_UINT32)(((A_UINT32)&(pWSICONFIGParms->mloChipId)) - (A_UINT32)pWSICONFIGParms), pParmsOffset);
    return((void*) pWSICONFIGParms);
}

static WSICONFIG_OP_FUNC WSICONFIGOpFunc = NULL;

TLV2_API void registerWSICONFIGHandler(WSICONFIG_OP_FUNC fp)
{
    WSICONFIGOpFunc = fp;
}

A_BOOL WSICONFIGOp(void *pParms)
{
    CMD_WSICONFIG_PARMS *pWSICONFIGParms = (CMD_WSICONFIG_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("WSICONFIGOp: phyId %u\n", pWSICONFIGParms->phyId);
    A_PRINTF("WSICONFIGOp: mloNumChips %u\n", pWSICONFIGParms->mloNumChips);
    A_PRINTF("WSICONFIGOp: mloChipInfoIndex %u\n", pWSICONFIGParms->mloChipInfoIndex);
    A_PRINTF("WSICONFIGOp: mloChipInfoChipId %u\n", pWSICONFIGParms->mloChipInfoChipId);
    A_PRINTF("WSICONFIGOp: mloChipInfoNumLocLinks %u\n", pWSICONFIGParms->mloChipInfoNumLocLinks);
    A_PRINTF("WSICONFIGOp: mloChipInfoLocLinksIndex %u\n", pWSICONFIGParms->mloChipInfoLocLinksIndex);
    A_PRINTF("WSICONFIGOp: mloChipInfoLocHwLinkId %u\n", pWSICONFIGParms->mloChipInfoLocHwLinkId);
    A_PRINTF("WSICONFIGOp: mloChipInfoLocValidMloLinkId %u\n", pWSICONFIGParms->mloChipInfoLocValidMloLinkId);
    A_PRINTF("WSICONFIGOp: mloChipId %u\n", pWSICONFIGParms->mloChipId);
#endif //_DEBUG

    if (NULL != WSICONFIGOpFunc) {
        (*WSICONFIGOpFunc)(pWSICONFIGParms);
    }
    return(TRUE);
}
