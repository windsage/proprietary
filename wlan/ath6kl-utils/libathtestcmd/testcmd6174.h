/*
*
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/

/*
 * 2014-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * Copyright (c) 2006 Atheros Communications Inc.
 * All rights reserved.
 *
// The software source and binaries included in this development package are
// licensed, not sold. You, or your company, received the package under one
// or more license agreements. The rights granted to you are specifically
// listed in these license agreement(s). All other rights remain with Atheros
// Communications, Inc., its subsidiaries, or the respective owner including
// those listed on the included copyright notices.  Distribution of any
// portion of this package must be in strict compliance with the license
// agreement(s) terms.
// </copyright>
//
// <summary>
// Wifi driver for AR6002
// </summary>
//
*
*/

#ifndef _TESTCMD6174_H
#define _TESTCMD6174_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATH_MAC_LEN
#define ATH_MAC_LEN 6
#endif

#ifndef MAX_DATA_LEN
#define MAX_DATA_LEN 74
#endif

#ifndef MAX_RU_INDEX
#define MAX_RU_INDEX 8
#endif

#ifndef MAX_11BE_DATA_LEN
#define MAX_11BE_DATA_LEN 148
#endif

#ifndef MAX_BE_RU_INDEX
#define MAX_BE_RU_INDEX 16
#endif

#ifndef MAX_CLI_VAL
#define MAX_CLI_VAL 5000
#endif

#define PS160_LEN 16
#define MAX_FILENAME_LEN       255
#define BOARD_DATA_SZ_MAX       184320


#define VIRTUAL_FLASH_PARTITION "/tmp/virtual_art.bin"
#define FLASH_BASE_CALDATA_OFFSET_SOC_0 0x1000
#define CALDATA_SEG_SIZE (150 * 1024)
#define WRITE_ART "/lib/compress_vart.sh write_caldata"

//define flash read/write macros to fetch configuartions from ftm.conf file
#define FTM_MAX_NUM_SLOTS 8
#define FTM_MAX_SLOT_SIZE 8
#define FTM_CFG_FILE_PATH "/lib/firmware/ftm.conf"
#define BOARD_NAME_PATH "/tmp/sysinfo/board_name"
#define BOARD_NAME_LEN 64

typedef struct __attribute__((__packed__)) {
	uint8_t version;                        // Make it 684 bytes
	uint8_t bandwidth;                      // 1 - 74 (depends on RU allocation indices)
	uint8_t AllocIndx[MAX_RU_INDEX];        // 1 - 8 RU allocation indices (depends on bandwidth)
	uint8_t AllocRUs[MAX_RU_INDEX];         // 1 - 8 RU allocation indices (depends on bandwidth)
	uint8_t MCS[MAX_DATA_LEN];              // MCS 0-11
	uint8_t FEC[MAX_DATA_LEN];              // LDPC or BCC
	uint8_t NSS[MAX_DATA_LEN];              // Spatial streams 1-8 (NA for HK defaults to 1), default = 1
	uint8_t boost[MAX_DATA_LEN];            // Default = 0, TonePlanWizard float * 8
	uint8_t ID[MAX_DATA_LEN];               // 1..255 simulated AID
	// Newer fields
	uint16_t payloadLen[MAX_DATA_LEN];      // Default = 1500, V2 only
	uint16_t packetExt[MAX_DATA_LEN];
	uint16_t nHE_LTF_Symbol[MAX_DATA_LEN];
	uint8_t ltfGI[MAX_DATA_LEN];
	uint8_t Rsvd1[MAX_DATA_LEN];            // Reserved for future use, V2 only
	uint8_t Rsvd2[MAX_DATA_LEN];            // Reserved for future use, V2 only
} TLV2_OFDMATONEPLAN_PARMS;

typedef struct {  // 2122
	uint8_t version;               // Version 201, if last digit is 1, then EHT
	uint8_t bandwidth;              // 1 - 148 (depends on RU allocation indices)
	uint16_t AllocIndx[MAX_BE_RU_INDEX];         // 1 - 16 RU allocation indices (depends on bandwidth), 320/20=16
	uint8_t AllocRUs[MAX_BE_RU_INDEX];           // 1 - 8 RU allocation (depends on bandwidth)
	uint8_t MCS[MAX_11BE_DATA_LEN];  // MCS 0-13, 9 * 8(160Mhz) = 72 + 2 (80Mhz center tone) = 74. 148
	uint8_t FEC[MAX_11BE_DATA_LEN];  // LDPC or BCC
	uint8_t NSS[MAX_11BE_DATA_LEN];  // Spatial streams 1-8 (NA for HK defaults to 1), default = 1
	uint8_t boost[MAX_11BE_DATA_LEN];           // Default = 0, TonePlanWizard float * 8
	uint8_t ID[MAX_11BE_DATA_LEN];              // 1..255 simulated AID
	uint16_t payloadLen[MAX_11BE_DATA_LEN];     // Default = 1500, V2 only
	uint16_t packetExt[MAX_11BE_DATA_LEN];
	uint16_t nHE_LTF_Symbol[MAX_11BE_DATA_LEN];
	uint8_t ltfGI[MAX_11BE_DATA_LEN];
	uint8_t ps160[PS160_LEN];
	uint8_t Rsvd1[MAX_11BE_DATA_LEN-PS160_LEN]; // Reserved for future use
	uint8_t Rsvd2[MAX_11BE_DATA_LEN];           // Reserved for future use
} TLV2_OFDMATONEPLANEHT_PARMS;        // 11BE only

typedef struct __attribute__((__packed__)) {
	uint16_t freq;
	uint16_t rateBitIndex;
	uint8_t ctlRd;
	uint8_t ctlGroup;
	uint8_t ctlNonOfdmaType;
	int8_t ctlChainPower;
	int8_t ctlFinalTotalPower;
	int8_t r2pPower;
	int8_t sarPower;
	int8_t ctlExceptionValue;
	int8_t ctlExceptionValue2;
	int8_t ctlExceptionValueApplied;
	int8_t ctlExceptionValueAdjust;
	uint8_t hcOffset;
	uint8_t heOffset;
	int8_t regulatoryPowerLimit;
	int8_t regulatoryPSDLimit;
	int8_t ctlAntElementGain;
	int8_t ctlAddedMargin;
	int8_t ctlArrayGain;
	uint8_t beamForming;
	uint8_t numChain;
	uint32_t nss;
	uint16_t ctlEbw;
} TLV2_CTLTPCFTMDATASTANDALONE_RSP_PARMS;

typedef struct _cmdParmeter{
	int isTxStart;
	int isRxStart;

	int freq;
	int bandWidth;
	int rate;
	uint8_t bssid[ATH_MAC_LEN];

	double txPwr;
	int32_t tpcm;
	int32_t paConfig;
	int32_t gainIdx;
	int32_t dacGain;
	int32_t numPkt;
	int32_t agg;
	int32_t pktLen0;
	uint32_t broadcast;
	uint8_t txStation[ATH_MAC_LEN];
	uint8_t rxStation[ATH_MAC_LEN];
	int aifs;
	uint8_t pw_mode_6g;
	uint8_t skip_rx_stop;

	uint32_t rxPkt;
	int32_t  rxRssi;
	uint32_t rxCrcError;
	uint32_t rxSecError;
	uint32_t expectedPkts;
	uint8_t addr[ATH_MAC_LEN];

	int longpreamble;
	int shortguard;
	uint32_t antenna;
	uint32_t chain;
	uint32_t ani;

	int errCode;
	char errString[256];

	int pwr_backoff;
	int stbc;
	int ldpc;
	int wmode;
	int scpc_cal;

	/* NART Command */
	/* buffer to save NART response data */
	uint8_t nart_rsp_buf[2048];
	uint32_t phyid;
	int freq2;
	uint8_t bt_addr[ATH_MAC_LEN];
	uint32_t sar_index;
	uint32_t sar_chain;
	uint32_t sar_cck2glimit;
	uint32_t sar_ofdm2glimit;
	uint32_t sar_ofdm5glimit;
	uint32_t enable_dpd_flag;
	uint8_t rateBw;
	uint8_t nss;
	uint8_t gi;
	uint8_t ofdmadcm;
	uint8_t ofdmappdutype;
	uint8_t ofdmalinkdir;
	uint8_t fecpad;
	uint8_t ldpc_exsymbol;
	uint8_t duty_cycle;
	uint32_t regval;
	TLV2_OFDMATONEPLAN_PARMS toneplan;
	TLV2_OFDMATONEPLANEHT_PARMS be_toneplan;
	uint8_t lopwr_mode;
	uint8_t phyid_mask;
	uint32_t lpwr_fwmask;
	uint8_t wifistandard;
	uint8_t rstDir;
	uint32_t calTxGain;
	uint32_t forcedRXIdx;
        uint32_t puncBw;
	uint8_t noiseFloorRead;
	uint8_t xlnaCtrl;
	int8_t xlnaCtrlValid;
	uint8_t supportMaxBW;
	uint16_t ctlNumTXChain;
	uint32_t powerSetIndex;
	uint8_t respStatus;
	uint8_t ctlRd;
	uint8_t ctlGroup;
	uint8_t ctlEbw;
	uint8_t ctlNonOfdmaType;
	uint16_t rateBitIndex;
	uint8_t verbose_log;
	uint8_t Cmprsflag;
	uint8_t attachMode;
	uint32_t emlsrFlags;
	uint16_t chainMask;
	uint8_t auxRate;
	uint8_t auxRateBW;
	uint8_t phyRfSubMode;
	uint8_t scanProfile;
	int16_t perchainTxPower[8];
	uint32_t lpwrHdlFeatureMask;
	uint32_t lpwrFeatureParam;
	uint8_t lpwrFlag;
	uint32_t band;
	int32_t nfdbr;
	int32_t nfdbm;
	int32_t mincca;
	uint8_t caltype;
	uint8_t Calopt;
        uint8_t ArtSlotId;
}_CMD_PARM;


typedef struct _cmdParmeterValid {
	bool phyRfSubModeByUser;
	bool attachModeByUser;
	bool scanProfileByUser;
} _CMD_PARM_VALID;

struct ftm_config
{
    uint32_t board_id[FTM_MAX_NUM_SLOTS]; /* create caldata file with board id extension */
    uint32_t slot_size[FTM_MAX_SLOT_SIZE]; /* caldata size for the each slot */
    uint32_t slot_offset[FTM_MAX_SLOT_SIZE]; /* caldata offset for the each slot */
    uint8_t slot_id[FTM_MAX_NUM_SLOTS]; /* slot unique id for identifying each slot */
    uint8_t total_num_slots; /* maximum number of slots to support */
};

extern struct ftm_config ftm_cfg;

//OFDMA Support Parameters
#define TOTAL_MANDATORY_INPUTS 13
#define TOTAL_AX_POSSIBLE_INPUTS 13
#define TOTAL_BE_POSSIBLE_INPUTS 14

#define IGNORE_TONE 10

// This Field mask will ensure these fields are present in tone plan file
typedef enum {
	VER=0x1,
	BANDWIDTH=0x2,
	ALLOC_INDX=0x4,
	ALLOC_RU=0x8,
	MCS=0x10,
	FEC=0x20,
	NSS=0x40,
	BOOST=0x80,
	ID=0x100,
	PAYLOAD=0x200,
	PACKET_EXT=0x400,
	LTF_SYMBOL=0x800,
	LTF_GI=0x1000,
	PS160=0x2000
}FIELD_MASK;

enum {
        CAL_TYPE_XTAL = 0,
        CAL_TYPE_OPC = 1,
        CAL_TYPE_RX_GAIN = 2
};

enum {
        CUST_RXGAINCAL_INIT = 0,
        CUST_GET_TARGET_PARAMS = 1,
        CUST_SET_TARGET_PARAMS = 2
};

int qca6174ApiInit();
void qca6174init();
void qca6174ApiCleanup(void);
void qca6174ChannelSet(int channel);
void qca6174FreqSet(uint32_t freq);
void qca6174RateSet(int rate);
void qca6174TxPowerSet(char *);

/** @breif Enable long preamble */
void qca6174SetLongPreamble(int enable);

/** @breif Set the interval between frames in aifs number
 *  @param slot aifs slot 0->SIFS, 1->PIFS, 2->DIFS, ... 253 */
void qca6174SetAifsNum(int slot);
void qca6174SetPwMode6G(int pw_mode_6g);
void qca6174SetSkipRxStop(uint8_t skip_rx_stop);
void qca6174SetAntenna(int antenna);
void qca6174SetChain(int chain);
void qca6174SetBssid(char *mac);
void qca6174SetTxStation(char *mac);
void qca6174SetRxStation(char *mac);
void qca6174SetAddr(char *mac);
void qca6174TxPcmSet(int txpwr);
void qca6174SetPaCfg(int val);
void qca6174SetDacGain(int val);
void qca6174SetGainIdx(int val);
void qca6174SetNumPkt(int val);
void qca6174SetAgg(int val);
void qca6174SetLdpc(int val);
void qca6174SetStbc(int val);
void qca6174SetWlanMode(int val);
void qca6174SetLPreamble();

void qca6174TxPacketSizeSet(int size);
void qca6174ShortGuardSet(int enable);
void qca6174SetBandWidth(int width);

int qca6174TxSineStart(void);
int qca6174Tx99Start(void);
int qca6174TxFrameStart(void);
int qca6174TxCWStart(void);
int qca6174TxStop(void);
int qca6174Tx100Start(void);
int qca6174TxCLPCPktStart(void);
#ifdef NOT_USE_CLITLVCMD_IF_MODULE
int qca6174RxPacketStart(void);
#else
int qca6174RxPacketStart(char *rx_type);
#endif
int qca6174RxPacketStop(void);
uint32_t qca6174RxGetErrorFrameNum(void);
uint32_t qca6174RxGetGoodFrameNum(void);
const char *qca6174GetErrorString(void);
void qca6174_compute_checksum(uint8_t *ptr_6320_eeprom);
int qca6174_clear_scpc_done();
void qca6174_enable_scpc_cal(int val);
int qca6174_get_scpc_cal();
int qca6174_eeprom_block_read(void* buf, uint32_t offset, uint32_t length);
int qca6174_eeprom_write_item(void* buf, uint32_t offset, uint8_t length);

void qca6174Freq2Set(uint32_t freq);
void qca6174SetPhyid(int);
void qca6174SetPhyParams(char *);
void qca6174SetBssid(char *);
void qca6174SetTxStation(char *);
void qca6174SetRxStation(char *);
void qca6174SetAddr(char *);
void qca6174SetBTAddr(char *);
void qca6174CmdSAR(uint32_t );
void qca6174CmdSARIndex(char *);
void qca6174CmdSARChain(uint32_t );
void qca6174CmdSARCCK2gLimit(char *);
void qca6174CmdSAROFDM2gLimit(char *);
void qca6174CmdSAROFDM5gLimit(char *);
void qca6174CmdFlagDPDEnable();
void qca6174CmdSETREGDMN(char *val);
int qca6174CmdDPDStatus();
void qca6174CmdRateBW(uint8_t val);
void qca6174CmdNSS(uint8_t val);
void qca6174CmdGI(uint8_t val);
void qca6174CmdADCM(uint8_t val);
void qca6174CmdPPDUTYPE(uint8_t val);
void qca6174CmdLINKDIR(uint8_t val);
int qca6174Cmd_TONEPLAN(char *val);
int qca6174Cmd_BE_TONEPLAN(char *val);
void qca6174Cmd_PREFECPAD(uint8_t val);
void qca6174Cmd_LDPCEXTRASYMBOL(uint8_t val);
void qca6174Cmd_DUTYCYCLE(uint8_t val);
int qca6174Cmd_OFDMAUL_TX(void);
void qca6174CmdRegWriteValue(uint32_t val);
int qca6174CmdRegWrite(uint32_t reg);
int qca6174CmdRegRead(uint32_t reg);
int qca6174Cmd_LOWPOWER(void);
void qca6174Set_LOWPOWER_MODE(char *val);
void qca6174Set_PHYIDMASK(uint8_t val);
void qca6174Set_LOWPOWER_FEATUREMASK(uint32_t mask);
void qca6174Set_CALTXGAIN(uint32_t val);
void qca6174Set_FORCEDRXIDX(uint32_t val);
void qca6174Set_RSTDIR(uint8_t val);
void handleRstRSP(void *parms);
void qca6174RssiSelfTest(void);
int qca6174GetDpdComplete(void);
void qca6174SetPuncBw(int);
void qca6174SetxlnaCtrl(char *val);
void qca6174SetNoiseFloorRead(char *val);
int qca6174GetNoiseFloor();
int qcaGetCTLTpcData(char *val);
int qca6174Cmd_AX_TONEPLAN_FILE(char *val);
int qca6174Cmd_BE_TONEPLAN_FILE(char *val);
int qca6174EepromRead();
int qca6174EepromWrite(char *val);
void qca6174SetCmprsFlag(int);
uint8_t eepromRead(uint32_t bdsize,uint32_t offset,uint16_t blksize);
uint8_t eepromWrite(uint32_t offset, uint16_t blksize, uint16_t *data, uint8_t enablePrint);
uint8_t checkEepromSize();
static bool writeBinFile(char *fileName, void *buf, uint32_t *length, uint32_t length2Read);
static bool readBinFile(char *fileName, void *buf, uint32_t *length, uint32_t length2Read);
int qca6174EepromErase();
void qca6174SetAttachMode(uint8_t val);
void qca6174EmlsrFlags(uint32_t val);
void qca6174SetAuxChainMask(uint16_t val);
void qca6174SetAuxRate(uint8_t val);
void qca6174SetAuxRateBW(uint8_t val);
void qca6174SetPhyRfSubMode(uint8_t val);
void qca6174SetScanProfile(uint8_t val);
int qca6174WsiConfig(char* val);
int qca6174WsiTriggerDoorBell(char* val);
int qca6174SetPerChainTxPower(char *val);
int qca6174Set_lowpower_hdl_featuremask(uint32_t val);
int qca6174Set_lowpower_featureparameter(uint32_t val);
int qca6174Set_lowpower_flag(uint8_t val);
int qca6174Band(int band_val);
int qca6174Nfdbr(int nfdbr_val);
int qca6174Nfdbm(int nfdbm_val);
int qca6174Mincca(int mincca_val);
int qca6174CusCtrlcal(char *val);
int qca6174RxCalOpt(char *val);
int qcaEfuseRead(char* val);
int qcaEfuseWrite(char* val);
int qcaNewXtalCalProc(char* val);
void qca6174SetFlashArtSlotId(int val);
int qca6174DumpCaldata2File();
int qca6174CommitCaldata2Flash();
int parse_ftm_cfg_file(FILE *fp, char *board_name);
int check_ftm_cfg_file();
uint8_t checkFlashSize();
void flash_bdread();
void caldata_commit2flash();
void caldata_dump2file();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

