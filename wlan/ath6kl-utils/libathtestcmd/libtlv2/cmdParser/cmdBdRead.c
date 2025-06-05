/*
*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/

// This is an auto-generated file from input\cmdBdRead.s
#include "tlv2Inc.h"
#include "cmdBdRead.h"

void* initBDREADOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    CMD_BDREAD_PARMS  *pBDREADParms = (CMD_BDREAD_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);


    // Populate the parm structure with initial values
    pBDREADParms->bdSize = pParmDict[PARM_BDSIZE].v.valU32;
    pBDREADParms->offset = pParmDict[PARM_OFFSET].v.valU32;
    pBDREADParms->size = pParmDict[PARM_SIZE].v.valU32;
    pBDREADParms->bdSource = pParmDict[PARM_BDSOURCE].v.valU32;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_BDSIZE, (size_t)(((size_t)&(pBDREADParms->bdSize)) - (size_t)pBDREADParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFFSET, (size_t)(((size_t)&(pBDREADParms->offset)) - (size_t)pBDREADParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SIZE, (size_t)(((size_t)&(pBDREADParms->size)) - (size_t)pBDREADParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_BDSOURCE, (size_t)(((size_t)&(pBDREADParms->bdSource)) - (size_t)pBDREADParms), pParmsOffset);
    return((void*) pBDREADParms);
}

static BDREAD_OP_FUNC BDREADOpFunc = NULL;

TLV2_API void registerBDREADHandler(BDREAD_OP_FUNC fp)
{
    BDREADOpFunc = fp;
}

A_BOOL BDREADOp(void *pParms)
{
    CMD_BDREAD_PARMS *pBDREADParms = (CMD_BDREAD_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

    A_PRINTF("BDREADOp: bdSize %u\n", pBDREADParms->bdSize);
    A_PRINTF("BDREADOp: offset %u\n", pBDREADParms->offset);
    A_PRINTF("BDREADOp: size %u\n", pBDREADParms->size);
    A_PRINTF("BDREADOp: bdSource %u\n", pBDREADParms->bdSource);
#endif //_DEBUG

    if (NULL != BDREADOpFunc) {
        (*BDREADOpFunc)(pBDREADParms);
    }
    return(TRUE);
}

void* initBDREADRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    CMD_BDREADRSP_PARMS  *pBDREADRSPParms = (CMD_BDREADRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);


    // Populate the parm structure with initial values
    pBDREADRSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    memset(pBDREADRSPParms->pad3, 0, sizeof(pBDREADRSPParms->pad3));
    pBDREADRSPParms->offset = pParmDict[PARM_OFFSET].v.valU32;
    pBDREADRSPParms->size = pParmDict[PARM_SIZE].v.valU32;
    memset(pBDREADRSPParms->data4k, 0, sizeof(pBDREADRSPParms->data4k));

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (size_t)(((size_t)&(pBDREADRSPParms->status)) - (size_t)pBDREADRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_PAD3, (size_t)(((size_t)&(pBDREADRSPParms->pad3)) - (size_t)pBDREADRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFFSET, (size_t)(((size_t)&(pBDREADRSPParms->offset)) - (size_t)pBDREADRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SIZE, (size_t)(((size_t)&(pBDREADRSPParms->size)) - (size_t)pBDREADRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DATA4K, (size_t)(((size_t)&(pBDREADRSPParms->data4k)) - (size_t)pBDREADRSPParms), pParmsOffset);
    return((void*) pBDREADRSPParms);
}

static BDREADRSP_OP_FUNC BDREADRSPOpFunc = NULL;

TLV2_API void registerBDREADRSPHandler(BDREADRSP_OP_FUNC fp)
{
    BDREADRSPOpFunc = fp;
}

A_BOOL BDREADRSPOp(void *pParms)
{
    CMD_BDREADRSP_PARMS *pBDREADRSPParms = (CMD_BDREADRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

    A_PRINTF("BDREADRSPOp: status %u\n", pBDREADRSPParms->status);
    for (i = 0; i < 3 ; i++)
    {
        A_PRINTF("BDREADRSPOp: pad3 %u\n", pBDREADRSPParms->pad3[i]);
    }
    A_PRINTF("BDREADRSPOp: offset %u\n", pBDREADRSPParms->offset);
    A_PRINTF("BDREADRSPOp: size %u\n", pBDREADRSPParms->size);
    for (i = 0; i < 8 ; i++) // can be modified to print up to 4096 entries
    {
        A_PRINTF("BDREADRSPOp: data4k 0x%x\n", pBDREADRSPParms->data4k[i]);
    }
#endif //_DEBUG

    if (NULL != BDREADRSPOpFunc) {
        (*BDREADRSPOpFunc)(pBDREADRSPParms);
    }
    return(TRUE);
}
