/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input\cmdCustRxGainCal.s
#include "tlv2Inc.h"
#include "cmdCustRxGainCal.h"

void* initCUST_RXGAINCALPROCOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
	A_PRINTF_ALWAYS(" anak not rsp\n");
    CMD_CUST_RXGAINCALPROC_PARMS  *pCUST_RXGAINCALPROCParms = (CMD_CUST_RXGAINCALPROC_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);


    // Populate the parm structure with initial values
    pCUST_RXGAINCALPROCParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pCUST_RXGAINCALPROCParms->band = pParmDict[PARM_BAND].v.valU8;
    pCUST_RXGAINCALPROCParms->freq = pParmDict[PARM_FREQ].v.valU16;
    pCUST_RXGAINCALPROCParms->chainIdx = pParmDict[PARM_CHAINIDX].v.valU8;
    pCUST_RXGAINCALPROCParms->rxGainCalCtrl = pParmDict[PARM_RXGAINCALCTRL].v.valU8;
    pCUST_RXGAINCALPROCParms->rxNFCalPowerDBr = pParmDict[PARM_RXNFCALPOWERDBR].v.valS8;
    pCUST_RXGAINCALPROCParms->rxNFCalPowerDBm = pParmDict[PARM_RXNFCALPOWERDBM].v.valS8;
    pCUST_RXGAINCALPROCParms->minCcaThreshold = pParmDict[PARM_MINCCATHRESHOLD].v.valS8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->phyId)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_BAND, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->band)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FREQ, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->freq)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAINIDX, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->chainIdx)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RXGAINCALCTRL, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->rxGainCalCtrl)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RXNFCALPOWERDBR, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->rxNFCalPowerDBr)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RXNFCALPOWERDBM, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->rxNFCalPowerDBm)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MINCCATHRESHOLD, (size_t)(((size_t)&(pCUST_RXGAINCALPROCParms->minCcaThreshold)) - (size_t)pCUST_RXGAINCALPROCParms), pParmsOffset);
    return((void*) pCUST_RXGAINCALPROCParms);
}

static CUST_RXGAINCALPROC_OP_FUNC CUST_RXGAINCALPROCOpFunc = NULL;

TLV2_API void registerCUST_RXGAINCALPROCHandler(CUST_RXGAINCALPROC_OP_FUNC fp)
{
    CUST_RXGAINCALPROCOpFunc = fp;
}

A_BOOL CUST_RXGAINCALPROCOp(void *pParms)
{
    CMD_CUST_RXGAINCALPROC_PARMS *pCUST_RXGAINCALPROCParms = (CMD_CUST_RXGAINCALPROC_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

    A_PRINTF("CUST_RXGAINCALPROCOp: phyId %u\n", pCUST_RXGAINCALPROCParms->phyId);
    A_PRINTF("CUST_RXGAINCALPROCOp: band %u\n", pCUST_RXGAINCALPROCParms->band);
    A_PRINTF("CUST_RXGAINCALPROCOp: freq %u\n", pCUST_RXGAINCALPROCParms->freq);
    A_PRINTF("CUST_RXGAINCALPROCOp: chainIdx %u\n", pCUST_RXGAINCALPROCParms->chainIdx);
    A_PRINTF("CUST_RXGAINCALPROCOp: rxGainCalCtrl %u\n", pCUST_RXGAINCALPROCParms->rxGainCalCtrl);
    A_PRINTF("CUST_RXGAINCALPROCOp: rxNFCalPowerDBr %u\n", pCUST_RXGAINCALPROCParms->rxNFCalPowerDBr);
    A_PRINTF("CUST_RXGAINCALPROCOp: rxNFCalPowerDBm %u\n", pCUST_RXGAINCALPROCParms->rxNFCalPowerDBm);
    A_PRINTF("CUST_RXGAINCALPROCOp: minCcaThreshold %u\n", pCUST_RXGAINCALPROCParms->minCcaThreshold);
#endif //_DEBUG

    if (NULL != CUST_RXGAINCALPROCOpFunc) {
        (*CUST_RXGAINCALPROCOpFunc)(pCUST_RXGAINCALPROCParms);
    }
    return(TRUE);
}

void* initCUST_RXGAINCALPROCRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
	A_PRINTF_ALWAYS(" anak rsp\n");
    CMD_CUST_RXGAINCALPROCRSP_PARMS  *pCUST_RXGAINCALPROCRSPParms = (CMD_CUST_RXGAINCALPROCRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);


    // Populate the parm structure with initial values
    pCUST_RXGAINCALPROCRSPParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pCUST_RXGAINCALPROCRSPParms->band = pParmDict[PARM_BAND].v.valU8;
    pCUST_RXGAINCALPROCRSPParms->freq = pParmDict[PARM_FREQ].v.valU16;
    pCUST_RXGAINCALPROCRSPParms->chainIdx = pParmDict[PARM_CHAINIDX].v.valU8;
    pCUST_RXGAINCALPROCRSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBr = pParmDict[PARM_RXNFCALPOWERDBR].v.valS8;
    pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBm = pParmDict[PARM_RXNFCALPOWERDBM].v.valS8;
    pCUST_RXGAINCALPROCRSPParms->minCcaThreshold = pParmDict[PARM_MINCCATHRESHOLD].v.valS8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->phyId)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_BAND, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->band)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FREQ, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->freq)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAINIDX, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->chainIdx)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->status)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RXNFCALPOWERDBR, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBr)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RXNFCALPOWERDBM, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBm)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MINCCATHRESHOLD, (size_t)(((size_t)&(pCUST_RXGAINCALPROCRSPParms->minCcaThreshold)) - (size_t)pCUST_RXGAINCALPROCRSPParms), pParmsOffset);
    return((void*) pCUST_RXGAINCALPROCRSPParms);
}

static CUST_RXGAINCALPROCRSP_OP_FUNC CUST_RXGAINCALPROCRSPOpFunc = NULL;

TLV2_API void registerCUST_RXGAINCALPROCRSPHandler(CUST_RXGAINCALPROCRSP_OP_FUNC fp)
{
    CUST_RXGAINCALPROCRSPOpFunc = fp;
}

A_BOOL CUST_RXGAINCALPROCRSPOp(void *pParms)
{
    CMD_CUST_RXGAINCALPROCRSP_PARMS *pCUST_RXGAINCALPROCRSPParms = (CMD_CUST_RXGAINCALPROCRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

    A_PRINTF("CUST_RXGAINCALPROCRSPOp: phyId %u\n", pCUST_RXGAINCALPROCRSPParms->phyId);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: band %u\n", pCUST_RXGAINCALPROCRSPParms->band);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: freq %u\n", pCUST_RXGAINCALPROCRSPParms->freq);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: chainIdx %u\n", pCUST_RXGAINCALPROCRSPParms->chainIdx);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: status %u\n", pCUST_RXGAINCALPROCRSPParms->status);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: rxNFCalPowerDBr %u\n", pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBr);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: rxNFCalPowerDBm %u\n", pCUST_RXGAINCALPROCRSPParms->rxNFCalPowerDBm);
    A_PRINTF("CUST_RXGAINCALPROCRSPOp: minCcaThreshold %u\n", pCUST_RXGAINCALPROCRSPParms->minCcaThreshold);
#endif //_DEBUG

    if (NULL != CUST_RXGAINCALPROCRSPOpFunc) {
        (*CUST_RXGAINCALPROCRSPOpFunc)(pCUST_RXGAINCALPROCRSPParms);
    }
    return(TRUE);
}
