/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input/cmdTxHandler.s
#ifndef _CMDTXHANDLER_H_
#define _CMDTXHANDLER_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

#if !defined(INTERLEAVED_PKTCNT)
#define INTERLEAVED_PKTCNT   8
#endif //!defined(INTERLEAVED_PKTCNT)

typedef struct tx_parms {
    A_UINT8	phyId;
    A_UINT8	txMode;
    A_UINT16	freq;
    A_UINT16	freq2;
    A_UINT16	chainMask;
    A_UINT8	wlanMode;
    A_UINT8	bandwidth;
    A_UINT8	gI;
    A_UINT8	antenna;
    A_UINT8	enANI;
    A_UINT8	scramblerOff;
    A_UINT8	aifsn;
    A_UINT8	broadcast;
    A_UINT8	agg;
    A_UINT8	shortGuard;
    A_UINT8	dutyCycle;
    A_UINT8	padding;
    A_UINT32	flags;
    A_UINT32	ir;
    A_UINT32	pktSz;
    A_UINT8	dataPattern[40];
    A_UINT16	txNumPackets;
    A_UINT16	txPattern;
    A_UINT8	nPattern;
    A_UINT8	tpcm;
    A_UINT8	gainIdx;
    A_INT8	dacGain;
    A_INT8	dacGainEnd;
    A_INT8	dacGainStep;
    A_UINT8	paConfig;
    A_UINT8	paConfigEnd;
    A_UINT8	paConfigStep;
    A_UINT8	ctrlFlag;
    A_UINT16	rateBitIndex;
    A_UINT32	rateMask[3];
    A_UINT32	rateMask11AC[5];
    A_UINT32	rateMaskAC160;
    A_INT32	pwrGainStart[24];
    A_INT32	pwrGainEnd[24];
    A_INT32	pwrGainStep[24];
    A_INT32	pwrGainStart11AC[40];
    A_INT32	pwrGainEnd11AC[40];
    A_INT32	pwrGainStep11AC[40];
    A_INT32	pwrGainStartAC160[8];
    A_INT32	pwrGainEndAC160[8];
    A_INT32	pwrGainStepAC160[8];
    A_UINT16	txPower;
    A_UINT8	bssid[6];
    A_UINT8	txStation[6];
    A_UINT8	rxStation[6];
    A_INT32	txPowerScaleFactor;
    A_UINT32	puncBwMask;
    A_UINT32	wifiStandard;
    A_UINT32	NSS;
    A_UINT8	rate;
    A_UINT8	ofdmaDcm;
    A_UINT8	ofdmaLinkDir;
    A_UINT8	ofdmaPpduType;
    A_UINT8	preFECPad;
    A_UINT8	ldpcExtraSymbol;
    A_UINT8	scanProfile;
    A_UINT8	mixedPkts;
    A_UINT8	wlanModes[INTERLEAVED_PKTCNT];
    A_UINT8	gIs[INTERLEAVED_PKTCNT];
    A_UINT8	rates[INTERLEAVED_PKTCNT];
    A_UINT8	rateBitIndices[INTERLEAVED_PKTCNT];
    A_UINT16	txPwrs[INTERLEAVED_PKTCNT];
    A_UINT32	pktSizes[INTERLEAVED_PKTCNT];
    A_UINT8	attachMode;
    A_UINT8	emlsrFlags;
    A_UINT16	aux_ChainMask;
    A_UINT8		aux_rate;
    A_UINT8		aux_ratebw;
    A_UINT8	pad[2];
} __ATTRIB_PACK CMD_TX_PARMS;

typedef void (*TX_OP_FUNC)(void *pParms);

// Exposed functions

void* initTXOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL TXOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDTXHANDLER_H_
