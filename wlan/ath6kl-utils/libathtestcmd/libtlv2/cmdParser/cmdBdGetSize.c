/*
*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/
// This is an auto-generated file from input\cmdBdGetSize.s
#include "tlv2Inc.h"
#include "cmdBdGetSize.h"

void* initBDGETSIZEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
	UNUSED(pParmsCommon);
	UNUSED(pParmsOffset);
	UNUSED(pParmDict);
	return(NULL);
}

static BDGETSIZE_OP_FUNC BDGETSIZEOpFunc = NULL;

TLV2_API void registerBDGETSIZEHandler(BDGETSIZE_OP_FUNC fp)
{
	BDGETSIZEOpFunc = fp;
}

A_BOOL BDGETSIZEOp(void *pParms)
{
	UNUSED(pParms);
	if (NULL != BDGETSIZEOpFunc) {
		(*BDGETSIZEOpFunc)(NULL);
	}
	return(TRUE);
}

void* initBDGETSIZERSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
	CMD_BDGETSIZERSP_PARMS  *pBDGETSIZERSPParms = (CMD_BDGETSIZERSP_PARMS *)pParmsCommon;

	if (pParmsCommon == NULL) return (NULL);


	// Populate the parm structure with initial values
	pBDGETSIZERSPParms->status = pParmDict[PARM_STATUS].v.valU8;
	memset(pBDGETSIZERSPParms->pad3, 0, sizeof(pBDGETSIZERSPParms->pad3));
	pBDGETSIZERSPParms->bdSize = pParmDict[PARM_BDSIZE].v.valU32;

	// Make up ParmOffsetTbl
	resetParmOffsetFields();
	fillParmOffsetTbl((A_UINT32)PARM_STATUS, (size_t)(((size_t)&(pBDGETSIZERSPParms->status)) - (size_t)pBDGETSIZERSPParms), pParmsOffset);
	fillParmOffsetTbl((A_UINT32)PARM_PAD3, (size_t)(((size_t)&(pBDGETSIZERSPParms->pad3)) - (size_t)pBDGETSIZERSPParms), pParmsOffset);
	fillParmOffsetTbl((A_UINT32)PARM_BDSIZE, (size_t)(((size_t)&(pBDGETSIZERSPParms->bdSize)) - (size_t)pBDGETSIZERSPParms), pParmsOffset);
	return((void*) pBDGETSIZERSPParms);
}

static BDGETSIZERSP_OP_FUNC BDGETSIZERSPOpFunc = NULL;

TLV2_API void registerBDGETSIZERSPHandler(BDGETSIZERSP_OP_FUNC fp)
{
	BDGETSIZERSPOpFunc = fp;
}

A_BOOL BDGETSIZERSPOp(void *pParms)
{
	CMD_BDGETSIZERSP_PARMS *pBDGETSIZERSPParms = (CMD_BDGETSIZERSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG

	A_PRINTF("BDGETSIZERSPOp: status %u\n", pBDGETSIZERSPParms->status);
	for (i = 0; i < 3 ; i++)
	{
		A_PRINTF("BDGETSIZERSPOp: pad3 %u\n", pBDGETSIZERSPParms->pad3[i]);
	}
	A_PRINTF("BDGETSIZERSPOp: bdSize %u\n", pBDGETSIZERSPParms->bdSize);
#endif //_DEBUG

	if (NULL != BDGETSIZERSPOpFunc) {
		(*BDGETSIZERSPOpFunc)(pBDGETSIZERSPParms);
	}
	return(TRUE);
}
