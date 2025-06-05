/*
* Copyright (c) 2021 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdSetCTLExceptionValue.s
#ifndef _CMDSETCTLEXCEPTIONVALUE_H_
#define _CMDSETCTLEXCEPTIONVALUE_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct setctlexceptionvalue_parms {
    A_UINT8	ctlRd;
    A_UINT8	ctlGroup;
    A_UINT16	freq;
    A_INT8	ctlExceptionValue;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_SETCTLEXCEPTIONVALUE_PARMS;

typedef void (*SETCTLEXCEPTIONVALUE_OP_FUNC)(void *pParms);

// Exposed functions

void* initSETCTLEXCEPTIONVALUEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SETCTLEXCEPTIONVALUEOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDSETCTLEXCEPTIONVALUE_H_
