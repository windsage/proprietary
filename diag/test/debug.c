/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Debug Application for Diag-router

GENERAL DESCRIPTION
  Contains main implementation of Diagnostic Services Debug Application.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#ifdef FEATURE_LE_DIAG
#include <fcntl.h>
#endif
#include "errno.h"
#include "event.h"
#include "msg.h"
#include "log.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "diag_lsm.h"
#include "ts_linux.h"
#include <limits.h>
#include "diag_lsm_comm.h"
#include "../src/diag_lsm_msg_i.h"
#include "../src/diag_lsm_log_i.h"
#include "../src/diag_lsm_event_i.h"

#define DIAG_DEBUG_IOCTL(_req, _buf, _len, ioctl) do { \
	if (diag_lsm_comm_ioctl(diag_fd_dbg, _req, _buf, _len)) \
		printf("debug-diag: Error in calling "ioctl"\n"); \
} while (0)

#define CONSOLE_PRINT_LIMIT   		62

struct diag_cmd_dbg {
        unsigned int first;
        unsigned int last;
        int periph_id;
};

static void usage(void)
{
	fprintf(stderr,
		"User space application for diag interface\n"
		"\n"
		"usage: debug-diag [-hdprsm]\n"
		"\n"
		"options:\n"
		"   -h   show parameter usage\n"
		"   -d   < dynamic debug mask to enable diag-router logs >\n"
		"        < 0x0  - disable logs>\n"
		"        < 0x1  - Information logs>\n"
		"        < 0x2  - USB Rx logs>\n"
		"        < 0x4  - MHI logs>\n"
		"        < 0x8  - ctrl logs>\n"
		"        < 0x10 - cmd logs>\n"
		"        < 0x20 - data logs>\n"
		"        < 0x40 - watch logs>\n"
		"        < 0x80 - MUX logs>\n"
		"        < 0x100 - PCIe logs>\n"
		"        < 0x200 - DCI logs>\n"
		"        < 0xFFFFFFFF - All logs>\n"
		"   -p   < pid of diag apps client >\n"
		"   -r   print registration table\n"
		"   -s   diag stats\n"
		"   -m   < Enabled masks info >\n"
		"        < 0  - All masks>\n"
		"        < 1  - F3 msgs>\n"
		"        < 2  - Log codes>\n"
		"        < 3  - Event masks>\n"
	       );

	exit(1);
}

void diag_print_enbld_msg_masks( void)
{
	int i, j, cnt = 0;
	unsigned int range = 0;
	uint32_t ssid_first, ssid_last;
	uint32_t *print_ptr;

	printf("\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("\t\tEnabled MSG SSIDs INFO Table\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("-");
	printf("\n");
	printf("  Sr. No.\t    SSID\t       Legacy Mask\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");

	for (i = 0; i < MSG_MASK_TBL_CNT; i++) {

		ssid_first = enabled_msg_mask_arr[i].ssid_first;
		ssid_last = enabled_msg_mask_arr[i].ssid_last;
		print_ptr = enabled_msg_mask_arr[i].ptr;

		for (j = ssid_first; j <= ssid_last; j++) {
			if (print_ptr != NULL) {
				if (*print_ptr) {
					cnt++;
					printf("  %3d\t\t%6d\t\t\t0x%08X \n", cnt, j, *print_ptr);
				}
				print_ptr++;
			}
		}

	}

	printf("\n  Total Enabled SSIDs : %d\n\n", cnt);
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n\n");

}

void diag_print_enbld_log_masks(void)
{
	int i, j, copy_len, cnt = 0;
	unsigned int num_items;
	uint8 equip_id;
	byte mask[MAX_ITEMS_PER_EQUIP_ID];

	printf("\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("\t\tLOG MASK INFO\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("  Equipment ID\t       Last Item Supported\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("-");
	printf("\n");

	for (i = 0; i < MAX_EQUIP_ID; i++) {
		equip_id = enabled_log_mask_arr[i].equip_id;
		num_items = enabled_log_mask_arr[i].num_items;
		printf("     %3d \t\t 0x%04X (%4d)\n", equip_id , num_items, num_items);
	}

	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("\t     LOG Codes Enabled \n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("     Sr. No.\t        Log Code\t\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("-");
	printf("\n");

	for (i = 0; i < MAX_EQUIP_ID; i++) {

		equip_id = enabled_log_mask_arr[i].equip_id;
		num_items = enabled_log_mask_arr[i].num_items;
		memcpy(mask, enabled_log_mask_arr[i].mask, sizeof(enabled_log_mask_arr[i].mask));

		copy_len = LOG_ITEMS_TO_SIZE(num_items);
		if (copy_len > MAX_ITEMS_PER_EQUIP_ID)
			copy_len = MAX_ITEMS_PER_EQUIP_ID;

		for (j = 0; j < copy_len; j++) {
			if (mask[j]) {
				int cpy, mcpy, k, pos;
				cpy = mask[j];
				mcpy = j * 8;
				for (k = 1, pos = 0; pos < 8; k = k << 1, pos++) {
					if (cpy & k) {
						cnt++;
						printf("     %4d\t\t 0x%X%03X \n", cnt, equip_id, mcpy + pos);
					}
				}
			}
		}
	}

	printf("\n  Total Log Masks Enabled : %d\n\n", cnt);
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n\n");
}

void diag_print_enbld_event_masks(void)
{
	int i, j, len, cnt = 0;

	printf("\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");
	printf("\t\tEVENT IDs ENABLED\n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("-");
	printf("\n");
	printf("  Sr. No.\t   Event ID\t \n");
	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n");

    len = sizeof(enabled_event_mask_arr) / sizeof(enabled_event_mask_arr[0]);
	for (i = 0; i < len; i++) {
		if (enabled_event_mask_arr[i]) {
			int cpy, mcpy, k, pos;
			cpy = enabled_event_mask_arr[i];
			mcpy = i * 8;
			for (k = 1, pos = 0; pos < 8; k = k << 1, pos++) {
				if (cpy & k) {
					cnt++;
					printf(" %4d\t\t 0x%04X (%d)\n", cnt, mcpy + pos, mcpy + pos);
				}
			}
		}
	}

	printf(" \nTotal Event Masks Set : - %d \n\n", cnt);

	for (i = 0; i < CONSOLE_PRINT_LIMIT; i++)
		printf("=");
	printf("\n\n");
}

int main(int argc, char **argv)
{
	int c, err, apps_pid = 0;
	int diag_fd_dbg = DIAG_INVALID_HANDLE;
	uint32_t dyn_debug_mask = 0;
	uint8_t mask_type;
	boolean bInit_Success = FALSE;
	struct debug_mask_with_pid {
		uint32_t mask;
		int pid;
	} debug_info;
	(void)argc;
	(void)argv;

	diag_fd_dbg = open("/dev/diag", O_RDWR);
	if (diag_fd_dbg > 0) {
		printf("Diag kernel support is present so exiting...\n");
		close(diag_fd_dbg);
		exit(0);
	} else {
		bInit_Success = Diag_LSM_Init(NULL);
		if (!bInit_Success) {
			DIAG_LOGE("\nDiag_LSM_Init() failed. Exiting...\n");
			exit(0);
		}
		diag_fd_dbg = diag_fd;
	}

	for (;;) {
		c = getopt(argc, argv, "hd:p:rsm:");
		if (c < 0)
			break;
		switch (c) {
		case 'd':
			dyn_debug_mask = to_integer(optarg);
			DIAG_DEBUG_IOCTL(DIAG_IOCTL_UPDATE_DYN_DBG_MASK, &dyn_debug_mask,
					 sizeof(dyn_debug_mask),
					 "DIAG_IOCTL_UPDATE_DYN_DBG_MASK");
			break;
		case 'p':
			apps_pid = atoi(optarg);
			debug_info.mask = dyn_debug_mask;
			debug_info.pid = apps_pid;
			DIAG_DEBUG_IOCTL(DIAG_IOCTL_UPDATE_DYN_DBG_MASK, &debug_info,
					 sizeof(struct debug_mask_with_pid),
					 "DIAG_IOCTL_UPDATE_DYN_DBG_MASK");
			break;
		case 'r':
			DIAG_DEBUG_IOCTL(DIAG_IOCTL_QUERY_REG_TABLE, NULL, 0,
						"DIAG_IOCTL_QUERY_REG_TABLE");
			break;
		case 's':
			DIAG_DEBUG_IOCTL(DIAG_IOCTL_QUERY_DEBUG_ALL, NULL, 0,
						"DIAG_IOCTL_QUERY_DEBUG_ALL");
			break;
		case 'm':
			mask_type = atoi(optarg);
			switch (mask_type) {
			case 0:
				diag_print_enbld_msg_masks();
				printf("\n");
				diag_print_enbld_log_masks();
				printf("\n");
				diag_print_enbld_event_masks();
				printf("\n");
				break;
			case 1:
				diag_print_enbld_msg_masks();
				break;
			case 2:
				diag_print_enbld_log_masks();
				break;
			case 3:
				diag_print_enbld_event_masks();
				break;
			default:
				printf("debug-diag: Invalid argument entered, Exiting...\n");
				usage();
				break;
			}
			break;
		default:
		case 'h':
			usage();
			break;
		}
	}

exit:
	if (diag_fd_dbg != DIAG_INVALID_HANDLE)
		Diag_LSM_DeInit();
	diag_fd_dbg = DIAG_INVALID_HANDLE;

	return 0;
}
