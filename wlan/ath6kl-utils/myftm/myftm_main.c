/*==========================================================================

                     FTM Main Task Source File

Description
  Unit test component file for regsitering the routines to Diag library
  for BT and FTM commands

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# All rights reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================

                         Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/10   rakeshk  Created a source file to implement routines for
                    registering the callback routines for FM and BT FTM
                    packets
07/06/10   rakeshk  changed the name of FM common header file in inclusion
07/07/10   rakeshk  Removed the sleep and wake in the main thread loop
===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <net/if.h>
#include <stdint.h>
#include <dirent.h>
#include <pwd.h>
#ifndef CONFIG_WLAN_QCLINUX
#ifndef WIN_AP_HOST
#include <cutils/sched_policy.h>
#endif
#endif
#include <ctype.h>
#include <getopt.h>

#include "myftm_dbg.h"
#include "myftm_wlan.h"
#include "myftm_qtip.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif

static char *progname = NULL;
unsigned int g_dbg_level = FTM_DBG_TRACE | FTM_DBG_ERROR;
extern int g_dbg_scpc;

static void usage(void)
{
	printf("\nusage: %s [options] \n"
		"   -n, --nodaemon      do not run as a daemon\n"
		"   -d    show more debug messages (-dd for even more)\n"
#ifdef CONFIG_FTM_BT
		"   -b, --board-type    Board Type\n"
#endif
#ifdef CONFIG_FTM_WLAN
		"   -i <wlan interface>\n"
		"       --interface=<wlan interface>\n"
		"       wlan adapter name (wlan, eth, etc.) default wlan\n"
#endif
		"   -B  <dbs/phya/phyb/sbs> (Phy RF Mode)\n"
		"   -D  <DAC GAIN>\n"
		"   -H  <0/1/2> (RX Mode)\n"
		"   -I  (or) --phyId <0/1> (Phy Id)\n"
		"   -J  (TLV 2.0 Messages)\n"
		"   -N  <BSSID>\n"
		"   -O  <STA Addr>\n"
		"   -o  <BT Addr>\n"
		"   -Q  <Secondary Frequncy>\n"
		"   -U  <0/1> (ShortGuard)\n"
		"   -X  <TX Sta Addr>\n"
		"   -Y  <RX Sta Addr>\n"
		"   --sar <0/1/2/3> \n"
		"   --sarindex <0/1/2/3/4/0xff> \n"
		"   --sarchain <0/1> \n"
		"   --sarcck2glimit <0 - 0xff> \n"
		"   --sarofdm2glimit <0 - 0xff> \n"
		"   --sarofdm5glimit <0 - 0xff> \n"
		"   --dpdflag (enable) \n"
		"   --regdomain <00:01> (Two reg domain values)\n"
		"   --dpdstatus \n"
		"   --aggregation \n"
		"   --channelFreq <set Center freqMHz> \n"
		"   --cbState <set Primay Channel> \n"
		"   --rate <set mcs> \n"
		"   --chainMask <0 - 15> \n"
		"   --txMode <set TxMode> \n"
		"   --powerLevel <set Tx power in dBm> \n"
		"   --payLoadSize <set payload size> \n"
		"   --rateBw <set bandwidth>\n"
		"   --nss <set nss>\n"
		"   --gI <set shortgi>\n"
		"   --dcm <set ofdma dcm>\n"
		"   --ppdutype <set ofdma ppdutype>\n"
		"   --linkdir  <set ofdma link direction>\n"
		"   --toneplan <set ofdma toneplan params>\n"
		"   --fecpad <set prefecpad params>\n"
		"   --ldpc <set extra symbol for ldpc>\n"
		"   --ulofdmat <set ofdma uplink transmmit config>\n"
		"   --dutycl (or) --dutyCycle <set duty cycle>\n"
		"   --regw <set register address>\n"
		"   --regd <read register address>\n"
		"   --regval <write value to register address>\n"
		"   --lowpower <set low power config for phyid>\n"
		"   --lpw_mode <set low power mode>\n"
		"   --phyidmask <set phyid mask>\n"
		"   --lpwfmsk   <set feature mask>\n"
		"   --caltxgain <Gain (RF gain for WCN) for open loop power control mode>\n"
		"   --forcedrxidx <set Forced RX gain index>\n"
		"   --rstdir <set RSSI self test direction, 0: chain0 to chain1 1: chain1 to chain0>\n"
		"   --rst  <RSSI self test (need to enable dbs mode before this test\n"
		"           per cmd myftm -J -B dbs)>\n"
		"   --aifsn <set aifsn number>\n"
		"   --pw_mode_6g <set 6g power mode 0-None 1-VLP 2-LPI 3-SP>\n"
		"   --puncBw <set puncture bandwidth pattern eg:0x1>\n"
		"   --eepromdump \n"
		"   -- cmprsflag <set flag> --eepromwrite fakeBoardData.bin\n"
		"   --eepromerase \n"
		"   --toneplanfile <.txt file name to parse ofdma toneplan params>\n"
		"   --attachmode <set attachmode>\n"
		"   --emlsrflags <set emlsrflags>\n"
		"   --auxchainmask <set auxchainmask>\n"
		"   --auxrate <set auxrate>\n"
		"   --auxratebw <set auxratebw>\n"
		"   --phyrfsubmode <set phyrfsubmode>\n"
		"   --scanprofile <set scanprofile>\n"
		"   --wsiconfig <config mlo parms with delimeter':'> mlonumchips:val:mlochipinfoindex:val:mlochipinfochipid:val:"
		"mlochipinfonumloclinks:val:mlochipinfoloclinksindex:val:mlochipinfolochwlinkid:val:mlochipinfolocvalidmlolinkid:val:mlochipid:val\n"
		"   --wsitriggerdoorbell <trigger doorbell with src and dest link with delimiter ':'> wsisrclink:val:wsidestlink:val\n"
		"   --setTxPowerPerChain <tx_power_chain_0>,<tx_power_chain_1>,.. \n"
		"   --hdllpfeaturemask <set hdllowerpowerfeaturemask>\n"
		"   --lpfeatureparameter <set lowpowerfeatureparameter>\n"
		"   --lpflag <set lowpowerflag>\n"
		"   --efuseread <efuse read arguments delimited by ':'> offset:val:length:val\n"
		"   --efusewrite <efuse write arguments delimited by ':'> offset:val:length:val:content:val\n"
		"   --newxtalcal <new xtal cal proc delimited by ':'> flag:val:capin:val:capout:val\n"
		"   --artslotid  <set Artslotid for caldata commit into Flash partition>\n"
		"   --calDump2File \n"
		"   --calCommit2Flash \n"
		"   --help   display this help and exit \n"
		, progname);
		exit(EXIT_FAILURE);
}

/*===========================================================================
FUNCTION   main

DESCRIPTION
  Initialises the Diag library and registers the PKT table for FM and BT
  and daemonises

DEPENDENCIES
  NIL

RETURN VALUE
  NIL, Error in the event buffer will mean a NULL App version and Zero HW
  version

SIDE EFFECTS
  None

===========================================================================*/

int main(int argc, char *argv[])
{
	int c;
	static struct option options[] = {
		{"help", no_argument, NULL, 'h'},
#ifdef CONFIG_FTM_WLAN
		{"interface", required_argument, NULL, 'i'},
#endif
#ifdef CONFIG_FTM_BT
		{"board-type", required_argument, NULL, 'b'},
#endif
		{"nodaemon", no_argument, NULL, 'n'},
		{"SetWifiEnable", required_argument, NULL, 'e'},
		{"GetWifiEnable", no_argument, NULL, 'E'},
		{"SetWifiMode", required_argument, NULL, 'm'},
		{"SetWlanMode", required_argument, NULL, 'M'},
		{"cbState", required_argument, NULL, 'M'},
		{"GetWifiModeSupport", required_argument, NULL, 'S'},
		{"WlanATSetWifiBandwidth", required_argument, NULL, 'w'},
		{"WlanATSetWifiBand", no_argument, NULL, 'W'}, //Not handled yet in the code//
		{"WlanATSetRate", required_argument, NULL, 'r'},
		{"rate", required_argument, NULL, 'r'},
		{"WlanATGetRate", required_argument, NULL, 'R'},
		{"WlanATSetGainIdx", required_argument, NULL, 'G'},
		{"WlanATSetDacGain", required_argument, NULL, 'D'},
		{"WlanATSetPaCfg", required_argument, NULL, 'C'},
		{"WlanATSetNumPkt", required_argument, NULL, 'j'},
		{"WlanATSetAgg", required_argument, NULL, 'k'},
		{"aggregation", required_argument, NULL, 'k'},
		{"WlanATSetWifiFreq", required_argument, NULL, 'f'},
		{"channelFreq", required_argument, NULL, 'f'},
		{"WlanATSetWifiTxPower", required_argument,NULL, 'p'},
		{"powerLevel", required_argument,NULL, 'p'},
		{"WlanATGetWifiTxPower", no_argument,NULL, 'P'},
		{"WlanATSetWifiAntenna", required_argument, NULL, 'a'},
		{"chainMask", required_argument, NULL, 'a'},
		{"WlanATGetWifiAntenna", no_argument, NULL, 'A'},
		{"WlanATSetWifiPktSize", required_argument, NULL, 's'},
		{"payLoadSize", required_argument, NULL, 's'},
		{"WlanATSetWifiTPC", required_argument, NULL, 'c'},
		{"WlanATSetWifiTX", required_argument, NULL, 't'},
		{"txMode", required_argument, NULL, 't'},
		{"WlanATGetWifiTX", no_argument, NULL, 'T'},
		{"WlanATSetWifiRX", required_argument, NULL, 'x'},
		{"WlanATSetStbc", required_argument, NULL, 'y'},
		{"WlanATSetLdpc", required_argument, NULL, 'z'},
		{"WlanATSetLPreamble", no_argument, NULL, 'l'},
		{"unittest", no_argument, NULL, 'u'},
		{"SCPCcal", required_argument, NULL, 'L'},
		{"qtipserver", no_argument, NULL, 'q'},
		{"WlanATSetPhyParams", required_argument, NULL, 'B'},
		{"WlanATSetWifiRXMode", required_argument, NULL, 'H'},
		{"WlanATSetPhyid", required_argument, NULL, 'I'},
		{"phyId", required_argument, NULL, 'I'},
		{"TLV2.0", no_argument, NULL, 'J'},
		{"WlanATSetBssid", required_argument, NULL, 'N'},
		{"WlanATSetSTAAddr", required_argument, NULL, 'O'},
		{"WlanATSetBTAddr", required_argument, NULL, 'o'},
		{"WlanATSetWifiFreq2", required_argument, NULL, 'Q'},
		{"WlanATSetTXSta", required_argument, NULL, 'X'},
		{"WlanATSetRXSta", required_argument, NULL, 'Y'},
		{"WlanATSetShortGuard", required_argument, NULL, 'U'},
		{"sar", required_argument, NULL, MYFTM_OPT_CMD_SAR},
		{"sarindex", required_argument, NULL, MYFTM_OPT_PRM_SAR_INDEX8},
		{"sarchain", required_argument, NULL, MYFTM_OPT_PRM_SAR_CHAIN},
		{"sarcck2glimit", required_argument, NULL,
				MYFTM_OPT_PRM_SAR_CCK2GLIMIT},
		{"sarofdm2glimit", required_argument, NULL,
				MYFTM_OPT_PRM_SAR_OFDM2GLIMIT},
		{"sarofdm5glimit", required_argument, NULL,
				MYFTM_OPT_PRM_SAR_OFDM5GLIMIT},
		{"dpdflag", no_argument, NULL, MYFTM_OPT_PRM_FLAG_DPD},
		{"regdomain", required_argument, NULL, MYFTM_OPT_CMD_SETREGDMN},
		{"dpdstatus", no_argument, NULL, MYFTM_OPT_CMD_DPDSTATUS},
		{"rateBw", required_argument, NULL, MYFTM_OPT_CMD_SET_RATEBW},
		{"nss", required_argument, NULL, MYFTM_OPT_CMD_SET_NSS},
		{"gI", required_argument, NULL, MYFTM_OPT_CMD_SET_GI},
		{"dcm", required_argument, NULL, MYFTM_OPT_CMD_SET_OFDM_ADCM},
		{"ppdutype", required_argument, NULL, MYFTM_OPT_CMD_SET_OFDM_PPDU_TYPE},
		{"linkdir", required_argument, NULL, MYFTM_OPT_CMD_SET_OFDM_LINKDIR},
		{"toneplan", required_argument, NULL, MYFTM_OPT_CMD_SET_OFDMA_TONEPLAN},
		{"fecpad", required_argument, NULL, MYFTM_OPT_CMD_SET_PREFECPAD},
		{"ldpc", required_argument, NULL, MYFTM_OPT_CMD_SET_LDPCEXTRASYMBOL},
		{"ulofdmat", no_argument, NULL, MYFTM_OPT_CMD_SET_OFDMAUL_TXCONFIG},
		{"dutycl", required_argument, NULL, MYFTM_OPT_CMD_SET_DUTYCYCLE},
		{"dutyCycle", required_argument, NULL, MYFTM_OPT_CMD_SET_DUTYCYCLE},
		{"regw", required_argument, NULL, MYFTM_OPT_CMD_WRITE_REGISTER},
		{"regd", required_argument, NULL, MYFTM_OPT_CMD_READ_REGISTER},
		{"regval", required_argument, NULL, MYFTM_OPT_CMD_WRT_REGISTER_VAL},
		{"lowpower", no_argument, NULL, MYFTM_OPT_CMD_SET_LOWPOWER},
		{"lpw_mode", required_argument, NULL, MYFTM_OPT_CMD_SET_LOWPOWER_MODE},
		{"phyidmask", required_argument, NULL, MYFTM_OPT_CMD_SET_PHYIDMASK},
		{"lpwfmsk", required_argument, NULL, MYFTM_OPT_CMD_SET_LOWPOWER_FEATUREMASK},
		{"caltxgain", required_argument, NULL, MYFTM_OPT_CMD_SET_CALTXGAIN},
		{"forcedrxidx", required_argument, NULL, MYFTM_OPT_CMD_SET_FORCEDRXIDX},
		{"rstdir", required_argument, NULL, MYFTM_OPT_CMD_SET_RSTDIR},
		{"rst", no_argument, NULL, MYFTM_OPT_CMD_RST},
		{"aifsn", required_argument, NULL, MYFTM_OPT_CMD_SET_AIFSN},
		{"pw_mode_6g", required_argument, NULL, MYFTM_OPT_CMD_SET_PW_MODE_6G},
		{"dpdcomplete", no_argument, NULL, MYFTM_OPT_CMD_GET_DPD_COMPLETE},
		{"puncBw", required_argument, NULL, MYFTM_OPT_CMD_SET_PUNC_BW},
		{"skipRxStop", no_argument, NULL, MYFTM_OPT_CMD_SET_SKIP_RX_STOP},
		{"noiseFloorRead", required_argument, NULL, MYFTM_OPT_CMD_SET_NOISEFLOORREAD},
		{"xlnaCtrl", required_argument, NULL, MYFTM_OPT_CMD_SET_XLNACTRL},
		{"getnoisefloor", no_argument, NULL, MYFTM_OPT_CMD_GET_NOISEFLOOR},
		{"getctltpcdata", required_argument, NULL, MYFTM_OPT_CMD_GETCTLTPCFTMDATASTANDALONE},
		{"toneplanfile", required_argument, NULL, MYFTM_OPT_CMD_SET_OFDMA_TONEPLAN2},
		{"eepromdump", no_argument, NULL, MYFTM_OP_EEPROM_DUMP},
		{"eepromwrite", required_argument, NULL, MYFTM_OP_EEPROM_WRITE},
		{"cmprsflag", required_argument, NULL, MYFTM_OP_COMPRESSION},
		{"eepromerase", no_argument, NULL, MYFTM_OP_EEPROM_ERASE},
		{"attachmode", required_argument, NULL, MYFTM_OPT_CMD_SET_ATTACH_MODE},
		{"emlsrflags", required_argument, NULL, MYFTM_OPT_CMD_SET_EMLSR_FLAGS},
		{"auxchainmask", required_argument, NULL, MYFTM_OPT_CMD_SET_AUX_CHAINMASK},
		{"auxrate", required_argument, NULL, MYFTM_OPT_CMD_SET_AUX_RATE},
		{"auxratebw", required_argument, NULL, MYFTM_OPT_CMD_SET_AUX_RATEBW},
		{"phyrfsubmode", required_argument, NULL, MYFTM_OPT_CMD_SET_PHY_RF_SUB_MODE},
		{"scanprofile", required_argument, NULL, MYFTM_OPT_CMD_SET_SCAN_PROFILE},
		{"wsiconfig", required_argument, NULL, MYFTM_OPT_CMD_WSI_CONFIG},
		{"wsitriggerdoorbell", required_argument, NULL, MYFTM_OPT_CMD_WSI_TRIGGERDOORBELL},
		{"setTxPowerPerChain", required_argument, NULL, MYFTM_OPT_CMD_SET_PERCHAIN},
		{"hdllpfeaturemask", required_argument, NULL, MYFTM_OPT_CMD_HDL_LOW_POWER_FEATUREMASK},
		{"lpfeatureparameter", required_argument, NULL, MYFTM_OPT_CMD_LOW_POWER_FEATURE_PARAMETER},
		{"lpflag", required_argument, NULL, MYFTM_OPT_CMD_LOW_POWER_FLAG},
		{"freqband", required_argument, NULL, MYFTM_OP_BAND},
		{"nfdbr", required_argument, NULL, MYFTM_OP_NFDBR},
		{"nfdbm", required_argument, NULL, MYFTM_OP_NFDBM},
		{"minccathres", required_argument, NULL, MYFTM_OP_MINCCATHRES},
		{"cus_ctrl_cal", required_argument, NULL, MYFTM_OP_CUSCTRLCAL},
		{"rxcal_opt", required_argument, NULL, MYFTM_OP_RXCALOPT},
		{"efuseread", required_argument, NULL, MYFTM_OPT_CMD_EFUSEREAD},
		{"efusewrite", required_argument, NULL, MYFTM_OPT_CMD_EFUSEWRITE},
		{"newxtalcal", required_argument, NULL, MYFTM_OPT_CMD_NEWXTALCAL},
		{"artslotid", required_argument, NULL, MYFTM_OPT_FLASH_ARTSLOTID},
		{"calDump2File", no_argument, NULL, MYFTM_OPT_DUMP_CALDATA2FILE},
		{"calCommit2Flash", no_argument, NULL, MYFTM_OPT_COMMIT_CALDATA2FLASH},
		{0, 0, 0, 0}
	};
	int daemonize = 0;
	int int_parm = 0;
	uint32_t t_val = 0;
	WLAN_AT_WIFREQ_STRU freq_parm;
	progname = argv[0];

	printf("Version Flags: 1.0\n");
	WlanATinit();

	while (1) {
		c = getopt_long(argc, argv,
				"hdi:nb:e:Em:S:w:Wr:Rg:f:p:Pqt:Tua:Ax:s:C:c:D"
				":k:G:j:y:z:lL:M:B:H:I:JN:O:o:Q:X:Y:U:rateBw:nss:gI:dcm:ppdutype:linkdir:toneplan:"
				"fecpad:ldpc:dutycl:lpw_mode:phyidmask:lowpower "
				"lpwfmsk:ulofdmat ",options, NULL);

		if (c < 0)
			break;

		switch (c) {
#ifdef CONFIG_FTM_WLAN
			case 'i':
				strlcpy(g_ifname, optarg, IFNAMSIZ);
				break;
#endif
			case 'n':
				daemonize = 0;
				break;
			case 'd':
#ifdef DEBUG
				g_dbg_level = g_dbg_level << 1 | 0x1;
#else
				printf("Debugging disabled, please build with -DDEBUG option. No debug level set\n");
				/* no need to EXIT. comment it out. */
				//exit(EXIT_FAILURE);
#endif
				/* enable scpc code debugging */
				g_dbg_scpc = 1;
				break;
			case 'e':
				printf("SetWifiEnable using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiEnable(int_parm);
				break;
			case 'E':
				printf("GetWifiEnable using athtestcmdlib\n");
				int_parm = WlanATGetWifiEnable();
				printf("WlanATGetWifiEnable return as %d\n", int_parm);
				break;
			case 'm':
				printf("SetWifiMode using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiMode(int_parm);
				break;
			case 'w':
				printf("WlanATSetWifiBand using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiBand(int_parm);
				break;
			case 'f':
				printf("WlanATSetWifiFreq using athtestcmdlib\n");
				int_parm = atoi(optarg);
				freq_parm.value = int_parm;
				freq_parm.offset = 0;
				WlanATSetWifiFreq(&freq_parm);
				break;
			case 'p':
				printf("WlanATSetWifiTxPower using athtestcmdlib\n");
				WlanATSetWifiTxPower(optarg);
				break;
			case 'P':
				printf("WlanATGetWifiTxPower using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATGetWifiTxPower();
				break;
			case 'r':
				printf("WlanATSetRate using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetRate(int_parm);
				break;
			case 'R':
				printf("WlanATGetRate using athtestcmdlib\n");
				WlanATGetRate();
				break;
			case 'a':
				printf("WlanATSetWifiAntenna using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiAntenna(int_parm);
				break;
			case 'A':
				printf("WlanATGetWifiAntenna using athtestcmdlib\n");
				WlanATGetWifiAntenna();
				break;
			case 't':
				printf("WlanATSetWifiTX using athtestcmdlib\n");
				int_parm = atoi(optarg);
				if (0 != WlanATSetWifiTX(int_parm)) {
					printf("WlanATSetWifiTX failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATSetWifiTX done with sucess\n");
				exit(EXIT_SUCCESS);
				break;
			case 'x':
				printf("WlanATSetWifiRX using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiRX(int_parm);
				exit(EXIT_SUCCESS);
				break;
			case 'u':
				printf("unittest using athtestcmdlib\n");
				unittest();
				exit(EXIT_SUCCESS);
				break;
			case 's':
				printf("WlanATSetWifiPktSize using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiPktSize(int_parm);
				break;
			case 'c':
				printf("WlanATSetWifiTPC using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiTPC(int_parm);
				break;
			case 'C':
				printf("WlanATSetPaCfg using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetPaCfg(int_parm);
				break;
			case 'D':
				printf("WlanATSetDacGain using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetDacGain(int_parm);
				break;
			case 'G':
				printf("WlanATSetGainIdx using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetGainIdx(int_parm);
				break;
			case 'j':
				printf("WlanATSetNumPkt using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetNumPkt(int_parm);
				break;
			case 'k':
				printf("WlanATSetAgg using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetAgg(int_parm);
				break;
			case 'y':
				printf("WlanATSetStbc using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetStbc(int_parm);
				break;
			case 'z':
				printf("WlanATSetLdpc using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetLdpc(int_parm);
				break;
			case 'M':
				printf("WlanATSetWlanMode using athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWlanMode(int_parm);
				break;
			case 'l':
				printf("WlanATSetLPreamble using athtestcmdlib\n");
				WlanATSetLPreamble();
				break;
			case 'L':
				printf("SCPC cal request\n");
				int_parm = atoi(optarg);
				if (0 != myftm_wlan_set_scpc_cal(int_parm)) {
					printf("myftm_wlan_set_scpc_cal failed!\n");
					exit(EXIT_FAILURE);
				}
				break;
			case 'q':
				printf("Starting QTIP server.\n");
				qtip();
				break;

			case 'B':
				printf("WlanATSetPhyParams using "
					"athtestcmdlib\n");
				WlanATSetPhyParams(optarg);
				exit(EXIT_SUCCESS);
				break;

			case 'H':
				printf("WlanATSetWifiRXMode using "
					"athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetWifiRXMode(int_parm);
				break;

			case 'I':
				printf("WlanATSetPhyid using "
					"athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetPhyid(int_parm);
				break;

			case 'J':
				printf("using TLV2.0\n");
				tlv2_enabled = 1;
				break;

			case 'N':
				printf("Set Bssid using "
					"athtestcmdlib\n");
				WlanATSetBssid(optarg);
				break;

			case 'O':
				printf("Set STA Addr using "
					"athtestcmdlib\n");
				WlanATSetSTAAddr(optarg);
				break;

			case 'o':
				printf("Set BT using athtestcmdlib\n");
				WlanATSetBTAddr(optarg);
				break;

			case 'Q':
				printf("WlanATSetWifiFreq2 using "
					"athtestcmdlib\n");
				int_parm = atoi(optarg);
				freq_parm.value = int_parm;
				freq_parm.offset = 0;
				WlanATSetWifiFreq2(&freq_parm);
				break;

			case 'U':
				printf("WlanATSetShortGuard using "
					"athtestcmdlib\n");
				int_parm = atoi(optarg);
				WlanATSetShortGuard(int_parm);
				break;

			case 'X':
				printf("Set TX using athtestcmdlib\n");
				WlanATSetTXSta(optarg);
				break;

			case 'Y':
				printf("Set RX using athtestcmdlib\n");
				WlanATSetRXSta(optarg);
				break;

			case MYFTM_OPT_CMD_SAR:
				printf("Command SAR\n");
				int_parm = atoi(optarg);
				WlanATCmdSAR(int_parm);
				break;

			case MYFTM_OPT_PRM_SAR_INDEX8:
				printf("SAR Index %s \n", optarg);
				WlanATCmdSARIndex(optarg);
				break;

			case MYFTM_OPT_PRM_SAR_CHAIN:
				printf("SAR CHAIN \n");
				int_parm = atoi(optarg);
				WlanATCmdSARChain(int_parm);
				break;

			case MYFTM_OPT_PRM_SAR_CCK2GLIMIT:
				printf("SAR CCK2GLIMIT %s \n", optarg);
				WlanATCmdSARCCK2gLimit(optarg);
				break;

			case MYFTM_OPT_PRM_SAR_OFDM2GLIMIT:
				printf("SAR OFDM2GLIMIT %s \n", optarg);
				WlanATCmdSAROFDM2gLimit(optarg);
				break;

			case MYFTM_OPT_PRM_SAR_OFDM5GLIMIT:
				printf("SAR OFDM5GLIMIT %s \n", optarg);
				WlanATCmdSAROFDM5gLimit(optarg);
				break;

			case MYFTM_OPT_PRM_FLAG_DPD:
				printf("Enable DPD Flag\n");
				WlanATCmdFlagDPDEnable();
				break;

			case MYFTM_OPT_CMD_SETREGDMN:
				printf("Command REGDMN\n");
				WlanATCmdSETREGDMN(optarg);
				break;

			case MYFTM_OPT_CMD_DPDSTATUS:
				printf("Command dpdstatus\n");
				WlanATCmdDPDStatus();
				break;

			case MYFTM_OPT_CMD_SET_RATEBW:
				printf("Command rateBw\n");
				WlanATCmdSETRateBW(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_NSS:
				printf("Command nss\n");
				WlanATCmdSETNSS(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_GI:
				printf("Command gI\n");
				WlanATCmdSETGI(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_OFDM_ADCM:
				printf("Command ofdm adcm\n");
				WlanATCmdSETADCM(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_OFDM_PPDU_TYPE:
				printf("Command ofdm ppdu type\n");
				WlanATCmdSETPPDUTYPE(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_OFDM_LINKDIR:
				printf("Command odfm linkdir\n");
				WlanATCmdSETLINKDIR(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_OFDMA_TONEPLAN:
				printf("WlanATCmdSET_TONEPLAN using athtestcmdlib\n");
				if (0 != WlanATCmdSET_TONEPLAN(optarg)) {
					printf("WlanATCmdSET_TONEPLAN failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATCmdSET_TONEPLAN done with success\n");
				exit(EXIT_SUCCESS);
				break;

			case MYFTM_OPT_CMD_SET_PREFECPAD:
				printf("Command prefecpad\n");
				WlanATCmdSET_PREFECPAD(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_LDPCEXTRASYMBOL:
				printf("Command ldpc extrasymbol\n");
				WlanATCmdSET_LDPCEXTRASYMBOL(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_OFDMAUL_TXCONFIG:
				printf("Command odfma uplink Tx config\n");
				if (0 != WlanATCmdSET_OFDMAULTX()) {
					printf("WlanATCmdSET_OFDMAULTX failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATCmdSET_OFDMAULTX done with success\n");
				exit(EXIT_SUCCESS);
				break;

			case MYFTM_OPT_CMD_SET_DUTYCYCLE:
				printf("Command Duty Cycle\n");
				WlanATCmdSET_DUTYCYCLE(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_WRT_REGISTER_VAL:
				printf("Command to write value at register \n");

				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if (t_val >= 0)
					WlanATCmdWRITE_REGVAL(t_val);
				else
					printf("setting WlanATCmdWRITE_REGVAL to 0\n");
				break;

			case MYFTM_OPT_CMD_WRITE_REGISTER:
				printf("Command to write register \n");

				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if ((t_val <= 0) || (0 != WlanATCmdWRITE_REGISTER(t_val))) {
					printf("WlanATCmdWRITE_REGISTER failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATCmdWRITE_REGISTER done with success\n");
				exit(EXIT_SUCCESS);
				break;

			case MYFTM_OPT_CMD_READ_REGISTER:
				printf("Command to read register \n");

				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if ((t_val <= 0) || (0 != WlanATCmdREAD_REGISTER(t_val))) {
					printf("WlanATCmdREAD_REGISTER failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATCmdREAD_REGISTER done with success\n");
				exit(EXIT_SUCCESS);
				break;

			case MYFTM_OPT_CMD_SET_LOWPOWER:
				printf("Command to send LOW POWER config\n");
				if (0 != WlanATCmdSET_LOWPOWER()) {
					printf("WlanATCmdSET_LOWPOWER failed!\n");
					exit(EXIT_FAILURE);
				}
				printf("WlanATCmdSET_LOWPOWER done with success\n");
				exit(EXIT_SUCCESS);
				break;

			case MYFTM_OPT_CMD_SET_LOWPOWER_MODE:
				printf("Command to configured LOW POWER MODE\n");
				WlanATCmdSET_LOWPOWER_MODE(optarg);
				break;

			case MYFTM_OPT_CMD_SET_PHYIDMASK:
				printf("Command to set PHYID MASK\n");
				WlanATCmdSET_PHYIDMASK(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_LOWPOWER_FEATUREMASK:
				printf("Command to set Low Power Feature Mask \n");

				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if (t_val >= 0)
					WlanATCmdSET_LOWEPOWER_FEATUREMASK(t_val);
				else
					printf("setting LOWEPOWER_FEATUREMASK to 0\n");
				break;

			case MYFTM_OPT_CMD_SET_CALTXGAIN:
				printf("Command to set TX Gain(RF gain for WCN)\n");
				WlanATCmdSET_CALTXGAIN(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_FORCEDRXIDX:
				printf("Command to set Forced RX gain index\n");
				WlanATCmdSET_FORCEDRXIDX(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_SET_RSTDIR:
				printf("Command to set RSSI self test direction\n");
				WlanATCmdSET_RSTDIR(atoi(optarg));
				break;

			case MYFTM_OPT_CMD_RST:
				printf("Command to enable RSSI self test, must be placed at end of cmd\n"
					"Please enable dbs first if not yet: -J -B dbs, to avoid FW crash\n");
				WlanATCmd_RST();
				break;

			case MYFTM_OPT_CMD_SET_AIFSN:
				printf("Command to set aifsn number\n");
				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if (t_val < 0 || t_val > 253) {
					fprintf(stderr, "WlanATCmdSET_AIFSN failed!\n");
					exit(EXIT_FAILURE);
				}
				WlanTCmdSET_AIFSN((uint8_t)t_val);
				break;
			case MYFTM_OPT_CMD_SET_PW_MODE_6G:
				printf("Command to set 6G power mode\n");
				errno = 0;
				t_val = (uint32_t)strtol(optarg, NULL, 0);
				if(errno != 0) {
					printf("Argument overflow or underflow occurred\n");
					exit(EXIT_FAILURE);
				}
				if (t_val < 0 || t_val > 3) {
					fprintf(stderr, "WlanATCmdSET6GPwMode failed!\n");
					exit(EXIT_FAILURE);
				}
				WlanATCmdSET6GPwMode((uint8_t)t_val);
				break;
			case MYFTM_OPT_CMD_GET_DPD_COMPLETE:
				printf("Command to get dpd complete\n");
				WlanATCmdGetDpdComplete();
				break;
			case MYFTM_OPT_CMD_SET_SKIP_RX_STOP:
				printf("Command to set skip rx stop\n");
				WlanATSetSkipRxStop(1);
				break;
			case MYFTM_OPT_CMD_SET_PUNC_BW:
				printf("Command to set puncture bandwidth pattern\n");
				t_val = (uint32_t)strtol(optarg, NULL, 16);
				WlanATCmdSet_PUNCBW(t_val);
				break;
			case MYFTM_OPT_CMD_GET_NOISEFLOOR:
				printf("Command to get noise floor\n");
				WlanATCmdGetNoiseFloor();
				break;
			case MYFTM_OPT_CMD_SET_XLNACTRL:
				printf("Command to set xlna ctrl\n");
				WlanATCmdSetxlnaCtrl(optarg);
				break;
			case MYFTM_OPT_CMD_SET_NOISEFLOORREAD:
				printf("Command to set noise floor read\n");
				WlanATCmdSetNoiseFloorRead(optarg);
				break;
			case MYFTM_OPT_CMD_GETCTLTPCFTMDATASTANDALONE:
				printf("Command to get CTL TPC Data:%s\n", optarg);
				WlanATCmdGetCTLTpcData(optarg);
				break;
			case MYFTM_OPT_CMD_SET_OFDMA_TONEPLAN2:
				printf("WlanATCmdSET_TONEPLAN using .txt file to parse ofdma params\n");
				if (0 == WlanATCmdSET_TONEPLAN_FILE(optarg)) {
					printf("WlanATCmdSET_TONEPLAN done with success using toneplan file to parse\n");
				} else {
					printf("WlanATCmdSET_TONEPLAN using file parse failed!\n");
				}
				break;
			case MYFTM_OP_EEPROM_DUMP:
				printf("Command to read EEPROM data ");
				WlanATCmdEepromRead();
				break;
			case MYFTM_OP_EEPROM_WRITE:
				printf("Command to write EEPROM data, optarg = %s\n",optarg);
				WlanATCmdEepromWrite(optarg);
				break;
			case MYFTM_OP_COMPRESSION:
				t_val = atoi(optarg);
				printf("Command to set compression flag %d\n", t_val);
				WlanATCCmdSetCmprsFlag(t_val);
				break;
			case MYFTM_OP_EEPROM_ERASE:
				printf("Command to erase EEPROM data ");
				WlanATCmdEepromErase();
				break;
			case MYFTM_OPT_CMD_SET_ATTACH_MODE:
				printf("Command to set attach mode\n");
				WlanATCmdSetAttachMode(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_EMLSR_FLAGS:
				printf("Command to set emlsr flags\n");
				WlanATCmdSetEmlsrFlags(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_AUX_CHAINMASK:
				printf("Command to set chain mask\n");
				WlanATCmdSetAuxChainMask(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_AUX_RATE:
				printf("Command to set aux rate\n");
				WlanATCmdSetAuxRate(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_AUX_RATEBW:
				printf("Command to set aux rate bw\n");
				WlanATCmdSetAuxRateBW(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_PHY_RF_SUB_MODE:
				printf("Command to set phy rf sub mode: %d\n", atoi(optarg));
				WlanATCmdSetPhyRfSubMode(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_SET_SCAN_PROFILE:
				printf("Command to set scan profile: %d\n", atoi(optarg));
				WlanATCmdSetScanProfile(atoi(optarg));
				break;
			case MYFTM_OPT_CMD_WSI_CONFIG:
				printf("Command to wsi config\n");
				if (0 != WlanATCmdWsiConfig(optarg)) {
					printf("WlanATCmdWsiConfig failed!\n");
					break;
				}
				printf("WlanATCmdWsiConfig done with success\n");
				break;
			case MYFTM_OPT_CMD_WSI_TRIGGERDOORBELL:
				printf("Command to wsi trigger door bell\n");
				if (0 != WlanATCmdWsiTriggerDoorBell(optarg)) {
					printf("WlanATCmdWsiTriggerDoorBell failed!\n");
					break;
				}
				printf("WlanATCmdWsiTriggerDoorBell done with success\n");
				break;
			case MYFTM_OPT_CMD_SET_PERCHAIN:
				printf("Command per chain tx power\n");
				if (0 != WlanATCmdPerChainTxPwr(optarg)) {
					printf("WlanATCmdPerChainTxPwr failed!\n");
					break;
				}
				printf("Command per chain tx power done with success\n");
			case MYFTM_OPT_CMD_HDL_LOW_POWER_FEATUREMASK:
				printf("Command to set hdl low power featuremask\n");
				if (0 != WlanATCmdHdlLowPowerFeatureMask(atoi(optarg))) {
					printf("WlanATCmdHdlLowPowerFeatureMask failed!\n");
					break;
				}
				printf("WlanATCmdHdlLowPowerFeatureMask done with success\n");
				break;
			case MYFTM_OPT_CMD_LOW_POWER_FEATURE_PARAMETER:
				printf("Command to set low power feature parameter\n");
				if (0 != WlanATCmdLowPowerFeatureParameter(atoi(optarg))) {
					printf("WlanATCmdLowPowerFeatureParameter failed!\n");
					break;
				}
				printf("WlanATCmdLowPowerFeatureParameter done with success\n");
				break;
			case MYFTM_OPT_CMD_LOW_POWER_FLAG:
				printf("Command to set low power flag\n");
				if (0 != WlanATCmdLowPowerFlag(atoi(optarg))) {
					printf("WlanATCmdLowPowerFlag failed!\n");
					break;
				}
				printf("WlanATCmdLowPowerFlag done with success\n");
			case MYFTM_OP_BAND:
				printf("Command to set band\n");
				WlanATCmdBand(atoi(optarg));
				break;
			case MYFTM_OP_NFDBR:
				printf("Command to set Nfdbr\n ");
				WlanATCmdNfdbr(atoi(optarg));
				break;
			case MYFTM_OP_NFDBM:
				printf("Command to set Nfdbm\n ");
				WlanATCmdNfdbm(atoi(optarg));
				break;
			case MYFTM_OP_MINCCATHRES:
				printf("Command to set Min CCA threshold\n ");
				WlanATCmdMincca(atoi(optarg));
				break;
			case MYFTM_OP_CUSCTRLCAL:
				printf("Command to set custom cal option\n ");
				WlanATCmdCusCtrlcal(optarg);
				break;
			case MYFTM_OP_RXCALOPT:
				printf("Command to set Rx cal option\n ");
				WlanATCmdRxCalOpt(optarg);
				break;
			case MYFTM_OPT_CMD_EFUSEREAD:
				printf("Command to read Efuse content\n");
				WlanATCmdEfuseRead(optarg);
				break;
			case MYFTM_OPT_CMD_EFUSEWRITE:
				printf("Command to write Efuse content\n");
				WlanATCmdEfuseWrite(optarg);
				break;
			case MYFTM_OPT_CMD_NEWXTALCAL:
				printf("Command to perform New Xtal Calibration\n");
				WlanATCmdNewXtalCalProc(optarg);
			case MYFTM_OPT_FLASH_ARTSLOTID:
				printf("Command to set Artslotid for Flash storage");
				int_parm = atoi(optarg);
				WlanATSetFlashArtSlotId(int_parm);
				break;
			case MYFTM_OPT_DUMP_CALDATA2FILE:
				printf("Command to read merged caldata from Target ");
				WlanATCmdDumpCaldata2File();
				break;
			case MYFTM_OPT_COMMIT_CALDATA2FLASH:
				printf("Command to commit Caldata to Flash memory ");
				WlanATCmdCommitCaldata2Flash();
				break;
			case 'h':
			default:
				usage();
				break;
		}
	}

	if (optind < argc)
		usage();

	if (daemonize && daemon(0, 0)) {
		perror("daemon");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

