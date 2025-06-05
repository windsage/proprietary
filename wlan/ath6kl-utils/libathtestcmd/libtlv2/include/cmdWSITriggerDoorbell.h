/*
* Copyright (c) 2024 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdWSITriggerDoorbell.s
#ifndef _CMDWSITRIGGERDOORBELL_H_
#define _CMDWSITRIGGERDOORBELL_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct wsitriggerdoorbell_parms {
    A_UINT8	phyId;
    A_UINT8	WSISrcLink;
    A_UINT8	WSIDestLink;
    A_UINT8	pad[1];
} __ATTRIB_PACK CMD_WSITRIGGERDOORBELL_PARMS;

typedef void (*WSITRIGGERDOORBELL_OP_FUNC)(void *pParms);

// Exposed functions

void* initWSITRIGGERDOORBELLOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL WSITRIGGERDOORBELLOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDWSITRIGGERDOORBELL_H_
