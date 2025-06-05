/* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// This is an auto-generated file from input/cmdNoiseFloorRead.s
#ifndef _CMDNOISEFLOORREAD_H_
#define _CMDNOISEFLOORREAD_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct noisefloorread_parms {
    A_UINT16	freq;
    A_UINT16	chainMask;
    A_UINT8	phyId;
    A_UINT8	noiseFloorRead;
    A_UINT8	rate;
    A_UINT8	xlnaCtrl;
} __ATTRIB_PACK CMD_NOISEFLOORREAD_PARMS;

typedef struct noisefloorreadrsp_parms {
    A_UINT32	nfValues[256];
    A_UINT16	nfValuesLength;
    A_UINT8	phyId;
    A_UINT8	pad[1];
} __ATTRIB_PACK CMD_NOISEFLOORREADRSP_PARMS;

typedef void (*NOISEFLOORREAD_OP_FUNC)(void *pParms);
typedef void (*NOISEFLOORREADRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initNOISEFLOORREADOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL NOISEFLOORREADOp(void *pParms);

void* initNOISEFLOORREADRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL NOISEFLOORREADRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDNOISEFLOORREAD_H_
