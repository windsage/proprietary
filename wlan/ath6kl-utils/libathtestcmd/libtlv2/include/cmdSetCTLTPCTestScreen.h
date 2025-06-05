/*
* Copyright (c) 2018 - 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input/cmdSetCTLTPCTestScreen.s
#ifndef _CMDSETCTLTPCTESTSCREEN_H_
#define _CMDSETCTLTPCTESTSCREEN_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct setctltpctestscreen_parms {
    A_UINT8	ctlRd;
    A_UINT8	ctlGroup;
    A_UINT16	freq;
    A_UINT16	rateBitIndex;
    A_UINT16	ctlDebugFlag;
    A_UINT8	ctlNonOfdmaType;
    A_UINT16	ctlEbw;
    A_UINT8	wds;
} __ATTRIB_PACK CMD_SETCTLTPCTESTSCREEN_PARMS;

typedef struct setctltpctestscreenrsp_parms {
    A_UINT8	status;
    A_UINT16	ctlPowerSetSize;
    A_UINT8	pad[1];
} __ATTRIB_PACK CMD_SETCTLTPCTESTSCREENRSP_PARMS;

typedef void (*SETCTLTPCTESTSCREEN_OP_FUNC)(void *pParms);
typedef void (*SETCTLTPCTESTSCREENRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initSETCTLTPCTESTSCREENOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SETCTLTPCTESTSCREENOp(void *pParms);

void* initSETCTLTPCTESTSCREENRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SETCTLTPCTESTSCREENRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDSETCTLTPCTESTSCREEN_H_
