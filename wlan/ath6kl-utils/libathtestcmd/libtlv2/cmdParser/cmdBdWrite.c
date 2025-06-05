/*
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
*/

// This is an auto-generated file from input\cmdBdWrite.s

#include "tlv2Inc.h"
#include "cmdBdWrite.h"

void* initBDWRITEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    CMD_BDWRITE_PARMS  *pBDWRITEParms = (CMD_BDWRITE_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);


    // Populate the parm structure with initial values
    pBDWRITEParms->offset = pParmDict[PARM_OFFSET].v.valU32;
    pBDWRITEParms->size = pParmDict[PARM_SIZE].v.valU32;
    memset(pBDWRITEParms->data4k, 0, sizeof(pBDWRITEParms->data4k));

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_OFFSET, (size_t)(((size_t)&(pBDWRITEParms->offset)) - (size_t)pBDWRITEParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SIZE, (size_t)(((size_t)&(pBDWRITEParms->size)) - (size_t)pBDWRITEParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DATA4K, (size_t)(((size_t)&(pBDWRITEParms->data4k)) - (size_t)pBDWRITEParms), pParmsOffset);
    return((void*) pBDWRITEParms);
}

static BDWRITE_OP_FUNC BDWRITEOpFunc = NULL;

TLV2_API void registerBDWRITEHandler(BDWRITE_OP_FUNC fp)
{
    BDWRITEOpFunc = fp;
}

A_BOOL BDWRITEOp(void *pParms)
{
    CMD_BDWRITE_PARMS *pBDWRITEParms = (CMD_BDWRITE_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

    A_PRINTF("BDWRITEOp: offset %u\n", pBDWRITEParms->offset);
    A_PRINTF("BDWRITEOp: size %u\n", pBDWRITEParms->size);
    for (i = 0; i < 8 ; i++) // can be modified to print up to 4096 entries
    {
        A_PRINTF("BDWRITEOp: data4k 0x%x\n", pBDWRITEParms->data4k[i]);
    }
#endif //_DEBUG

    if (NULL != BDWRITEOpFunc) {
        (*BDWRITEOpFunc)(pBDWRITEParms);
    }
    return(TRUE);
}
