/*
* Copyright (c) 2021 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdGetCTLSupportedInfo.s
#include "tlv2Inc.h"
#include "cmdGetCTLSupportedInfo.h"

void* initGETCTLSUPPORTEDINFOOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_GETCTLSUPPORTEDINFO_PARMS  *pGETCTLSUPPORTEDINFOParms = (CMD_GETCTLSUPPORTEDINFO_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pGETCTLSUPPORTEDINFOParms->ctlRd = pParmDict[PARM_CTLRD].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CTLRD, (A_UINT32)(((A_UINT32)&(pGETCTLSUPPORTEDINFOParms->ctlRd)) - (A_UINT32)pGETCTLSUPPORTEDINFOParms), pParmsOffset);
    return((void*) pGETCTLSUPPORTEDINFOParms);
}

static GETCTLSUPPORTEDINFO_OP_FUNC GETCTLSUPPORTEDINFOOpFunc = NULL;

TLV2_API void registerGETCTLSUPPORTEDINFOHandler(GETCTLSUPPORTEDINFO_OP_FUNC fp)
{
    GETCTLSUPPORTEDINFOOpFunc = fp;
}

A_BOOL GETCTLSUPPORTEDINFOOp(void *pParms)
{
    CMD_GETCTLSUPPORTEDINFO_PARMS *pGETCTLSUPPORTEDINFOParms = (CMD_GETCTLSUPPORTEDINFO_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("GETCTLSUPPORTEDINFOOp: ctlRd %u\n", pGETCTLSUPPORTEDINFOParms->ctlRd);
#endif //_DEBUG

    if (NULL != GETCTLSUPPORTEDINFOOpFunc) {
        (*GETCTLSUPPORTEDINFOOpFunc)(pGETCTLSUPPORTEDINFOParms);
    }
    return(TRUE);
}

void* initGETCTLSUPPORTEDINFORSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_GETCTLSUPPORTEDINFORSP_PARMS  *pGETCTLSUPPORTEDINFORSPParms = (CMD_GETCTLSUPPORTEDINFORSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pGETCTLSUPPORTEDINFORSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pGETCTLSUPPORTEDINFORSPParms->supportMaxBW = pParmDict[PARM_SUPPORTMAXBW].v.valU8;
    pGETCTLSUPPORTEDINFORSPParms->ctlNumTXChain = pParmDict[PARM_CTLNUMTXCHAIN].v.valU16;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (A_UINT32)(((A_UINT32)&(pGETCTLSUPPORTEDINFORSPParms->status)) - (A_UINT32)pGETCTLSUPPORTEDINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SUPPORTMAXBW, (A_UINT32)(((A_UINT32)&(pGETCTLSUPPORTEDINFORSPParms->supportMaxBW)) - (A_UINT32)pGETCTLSUPPORTEDINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLNUMTXCHAIN, (A_UINT32)(((A_UINT32)&(pGETCTLSUPPORTEDINFORSPParms->ctlNumTXChain)) - (A_UINT32)pGETCTLSUPPORTEDINFORSPParms), pParmsOffset);
    return((void*) pGETCTLSUPPORTEDINFORSPParms);
}

static GETCTLSUPPORTEDINFORSP_OP_FUNC GETCTLSUPPORTEDINFORSPOpFunc = NULL;

TLV2_API void registerGETCTLSUPPORTEDINFORSPHandler(GETCTLSUPPORTEDINFORSP_OP_FUNC fp)
{
    GETCTLSUPPORTEDINFORSPOpFunc = fp;
}

A_BOOL GETCTLSUPPORTEDINFORSPOp(void *pParms)
{
    CMD_GETCTLSUPPORTEDINFORSP_PARMS *pGETCTLSUPPORTEDINFORSPParms = (CMD_GETCTLSUPPORTEDINFORSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("GETCTLSUPPORTEDINFORSPOp: status %u\n", pGETCTLSUPPORTEDINFORSPParms->status);
    A_PRINTF("GETCTLSUPPORTEDINFORSPOp: supportMaxBW %u\n", pGETCTLSUPPORTEDINFORSPParms->supportMaxBW);
    A_PRINTF("GETCTLSUPPORTEDINFORSPOp: ctlNumTXChain %u\n", pGETCTLSUPPORTEDINFORSPParms->ctlNumTXChain);
#endif //_DEBUG

    if (NULL != GETCTLSUPPORTEDINFORSPOpFunc) {
        (*GETCTLSUPPORTEDINFORSPOpFunc)(pGETCTLSUPPORTEDINFORSPParms);
    }
    return(TRUE);
}
