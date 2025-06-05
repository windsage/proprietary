/*
* Copyright (c) 2024 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdWSITriggerDoorbell.s
#include "tlv2Inc.h"
#include "cmdWSITriggerDoorbell.h"

void* initWSITRIGGERDOORBELLOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_WSITRIGGERDOORBELL_PARMS  *pWSITRIGGERDOORBELLParms = (CMD_WSITRIGGERDOORBELL_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pWSITRIGGERDOORBELLParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pWSITRIGGERDOORBELLParms->WSISrcLink = pParmDict[PARM_WSISRCLINK].v.valU8;
    pWSITRIGGERDOORBELLParms->WSIDestLink = pParmDict[PARM_WSIDESTLINK].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (A_UINT32)(((A_UINT32)&(pWSITRIGGERDOORBELLParms->phyId)) - (A_UINT32)pWSITRIGGERDOORBELLParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_WSISRCLINK, (A_UINT32)(((A_UINT32)&(pWSITRIGGERDOORBELLParms->WSISrcLink)) - (A_UINT32)pWSITRIGGERDOORBELLParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_WSIDESTLINK, (A_UINT32)(((A_UINT32)&(pWSITRIGGERDOORBELLParms->WSIDestLink)) - (A_UINT32)pWSITRIGGERDOORBELLParms), pParmsOffset);
    return((void*) pWSITRIGGERDOORBELLParms);
}

static WSITRIGGERDOORBELL_OP_FUNC WSITRIGGERDOORBELLOpFunc = NULL;

TLV2_API void registerWSITRIGGERDOORBELLHandler(WSITRIGGERDOORBELL_OP_FUNC fp)
{
    WSITRIGGERDOORBELLOpFunc = fp;
}

A_BOOL WSITRIGGERDOORBELLOp(void *pParms)
{
    CMD_WSITRIGGERDOORBELL_PARMS *pWSITRIGGERDOORBELLParms = (CMD_WSITRIGGERDOORBELL_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("WSITRIGGERDOORBELLOp: phyId %u\n", pWSITRIGGERDOORBELLParms->phyId);
    A_PRINTF("WSITRIGGERDOORBELLOp: WSISrcLink %u\n", pWSITRIGGERDOORBELLParms->WSISrcLink);
    A_PRINTF("WSITRIGGERDOORBELLOp: WSIDestLink %u\n", pWSITRIGGERDOORBELLParms->WSIDestLink);
#endif //_DEBUG

    if (NULL != WSITRIGGERDOORBELLOpFunc) {
        (*WSITRIGGERDOORBELLOpFunc)(pWSITRIGGERDOORBELLParms);
    }
    return(TRUE);
}
