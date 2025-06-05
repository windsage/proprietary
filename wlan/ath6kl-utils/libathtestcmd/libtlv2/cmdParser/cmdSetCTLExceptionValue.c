/*
* Copyright (c) 2021 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdSetCTLExceptionValue.s
#include "tlv2Inc.h"
#include "cmdSetCTLExceptionValue.h"

void* initSETCTLEXCEPTIONVALUEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_SETCTLEXCEPTIONVALUE_PARMS  *pSETCTLEXCEPTIONVALUEParms = (CMD_SETCTLEXCEPTIONVALUE_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pSETCTLEXCEPTIONVALUEParms->ctlRd = pParmDict[PARM_CTLRD].v.valU8;
    pSETCTLEXCEPTIONVALUEParms->ctlGroup = pParmDict[PARM_CTLGROUP].v.valU8;
    pSETCTLEXCEPTIONVALUEParms->freq = pParmDict[PARM_FREQ].v.valU16;
    pSETCTLEXCEPTIONVALUEParms->ctlExceptionValue = pParmDict[PARM_CTLEXCEPTIONVALUE].v.valS8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CTLRD, (A_UINT32)(((A_UINT32)&(pSETCTLEXCEPTIONVALUEParms->ctlRd)) - (A_UINT32)pSETCTLEXCEPTIONVALUEParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLGROUP, (A_UINT32)(((A_UINT32)&(pSETCTLEXCEPTIONVALUEParms->ctlGroup)) - (A_UINT32)pSETCTLEXCEPTIONVALUEParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FREQ, (A_UINT32)(((A_UINT32)&(pSETCTLEXCEPTIONVALUEParms->freq)) - (A_UINT32)pSETCTLEXCEPTIONVALUEParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CTLEXCEPTIONVALUE, (A_UINT32)(((A_UINT32)&(pSETCTLEXCEPTIONVALUEParms->ctlExceptionValue)) - (A_UINT32)pSETCTLEXCEPTIONVALUEParms), pParmsOffset);
    return((void*) pSETCTLEXCEPTIONVALUEParms);
}

static SETCTLEXCEPTIONVALUE_OP_FUNC SETCTLEXCEPTIONVALUEOpFunc = NULL;

TLV2_API void registerSETCTLEXCEPTIONVALUEHandler(SETCTLEXCEPTIONVALUE_OP_FUNC fp)
{
    SETCTLEXCEPTIONVALUEOpFunc = fp;
}

A_BOOL SETCTLEXCEPTIONVALUEOp(void *pParms)
{
    CMD_SETCTLEXCEPTIONVALUE_PARMS *pSETCTLEXCEPTIONVALUEParms = (CMD_SETCTLEXCEPTIONVALUE_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("SETCTLEXCEPTIONVALUEOp: ctlRd %u\n", pSETCTLEXCEPTIONVALUEParms->ctlRd);
    A_PRINTF("SETCTLEXCEPTIONVALUEOp: ctlGroup %u\n", pSETCTLEXCEPTIONVALUEParms->ctlGroup);
    A_PRINTF("SETCTLEXCEPTIONVALUEOp: freq %u\n", pSETCTLEXCEPTIONVALUEParms->freq);
    A_PRINTF("SETCTLEXCEPTIONVALUEOp: ctlExceptionValue %u\n", pSETCTLEXCEPTIONVALUEParms->ctlExceptionValue);
#endif //_DEBUG

    if (NULL != SETCTLEXCEPTIONVALUEOpFunc) {
        (*SETCTLEXCEPTIONVALUEOpFunc)(pSETCTLEXCEPTIONVALUEParms);
    }
    return(TRUE);
}
