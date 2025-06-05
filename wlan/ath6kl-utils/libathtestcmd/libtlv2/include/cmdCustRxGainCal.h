/*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// This is an auto-generated file from input\cmdCustRxGainCal.s
#ifndef _CMDCUSTRXGAINCAL_H_
#define _CMDCUSTRXGAINCAL_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct cust_rxgaincalproc_parms {
    A_UINT8     phyId;
    A_UINT8     band;
    A_UINT16    freq;
    A_UINT8     chainIdx;
    A_UINT8     rxGainCalCtrl;
    A_INT8      rxNFCalPowerDBr;
    A_INT8      rxNFCalPowerDBm;
    A_INT8      minCcaThreshold;
    A_UINT8     pad[3];
} __ATTRIB_PACK CMD_CUST_RXGAINCALPROC_PARMS;

typedef struct cust_rxgaincalprocrsp_parms {
    A_UINT8     phyId;
    A_UINT8     band;
    A_UINT16    freq;
    A_UINT8     chainIdx;
    A_UINT8     status;
    A_INT8      rxNFCalPowerDBr;
    A_INT8      rxNFCalPowerDBm;
    A_INT8      minCcaThreshold;
    A_UINT8     pad[3];
} __ATTRIB_PACK CMD_CUST_RXGAINCALPROCRSP_PARMS;

typedef void (*CUST_RXGAINCALPROC_OP_FUNC)(void *pParms);
typedef void (*CUST_RXGAINCALPROCRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initCUST_RXGAINCALPROCOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL CUST_RXGAINCALPROCOp(void *pParms);

void* initCUST_RXGAINCALPROCRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL CUST_RXGAINCALPROCRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDCUSTRXGAINCAL_H_
