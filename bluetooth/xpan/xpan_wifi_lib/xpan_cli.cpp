/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <inc/xpan_wifi_hal.h>
#include <cutils/properties.h>

struct wifi_drv_ops fn;
static wifi_handle wifiHandle_ = NULL;
static struct wifi_callbacks dummy_cb;
static xpan_wifi_data *data = NULL;

static int parse_hex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}


int hex_byte(const char *str)
{
	int res1, res2;

	res1 = parse_hex(str[0]);
	if (res1 < 0)
		return -1;
	res2 = parse_hex(str[1]);
	if (res2 < 0)
		return -1;
	return (res1 << 4) | res2;
}

int parse_mac_address(const char *arg, unsigned char *addr)
{
	int i;
	const char *pos = arg;

	if (strlen(arg) != 17)
		goto fail;

	for (i = 0; i < ETH_ALEN; i++) {
		int val;

		val = hex_byte(pos);
		if (val < 0)
			goto fail;
		addr[i] = val;
		if (i + 1 < ETH_ALEN) {
			pos += 2;
			if (*pos != ':')
				goto fail;
			pos++;
		}
	}

	return 0;

fail:
	ALOGE("Invalid MAC address %s (expected format xx:xx:xx:xx:xx:xx)",
	      arg);
	return -1;
}

int str_starts(const char *str, const char *start)
{
	return strncmp(str, start, strlen(start)) == 0;
}

void dummy_cb_acs(int freq, acs_status status)
{
	if (status == ACS_COMPLETED) {
		ALOGI("ACS Completed with frequency %d", freq);
		printf("ACS Completed");
	} else {
		ALOGI("ACS started");
		printf("ACS Started");
	}
}

void dummy_cb_twt(twt_event_response twt_event)
{
	ALOGI("twt_event_type: %d", twt_event.event_type);
	ALOGI("twt_wake_dur_us: %d", twt_event.wake_dur_us);
	ALOGI("twt_wake_int_us: %d", twt_event.wake_int_us);
}

void dummy_cb_ps(ap_power_save_data data)
{
	ALOGI("Cookie Id %llu", data.cookie);
	ALOGI("Power save bi multiplier %d", data.power_save_bi_multiplier);
	ALOGI("Power save next tsf %llu",data.next_tsf);
}

void dummy_cb_switch(transport_switch_type type, transport_switch_status status)
{
	ALOGI("Received switch_type: %d", type);
	ALOGI("Recieved switch_status: %d", status);
}

void dummy_cb_avail(uint16_t cookie, xpan_ap_availability_status status)
{
	ALOGI("Cookie Id %llu", cookie);
	ALOGI("Received ap_availability_status: %d", status);
}

void dummy_cb_ch_switch(channel_params chan_params)
{
	ALOGI("Received frequency: %d", chan_params.freq);
	ALOGI("Received bandwidth: %d", chan_params.bw);
	ALOGI("Received tsf: %d", chan_params.tsf);
}

static int xpan_cli_add_iface(int argc, char *argv[])
{
	char ifname[IFNAMSIZ+1] = {0};
	if (argc >= 1) {
		//set iface info in data
		strlcpy(ifname, argv[0], sizeof(ifname));
	}
	if (fn.wifi_create_ap_iface(data, strlen(ifname) ? ifname : NULL)
			== XPAN_WIFI_STATUS_SUCCESS)
		return 0;
	else
		return -1;
}

static void get_xpan_iface(char *ifname)
{
	char buf[PROPERTY_VALUE_MAX] = {};
	property_get("vendor.wifi.lohs.sap.interface", buf, "");
	strlcpy(ifname, buf, IFNAMSIZ);
}

static int xpan_cli_del_iface(int argc, char *argv[])
{
	if (fn.wifi_delete_ap_iface(data) == XPAN_WIFI_STATUS_SUCCESS)
		return 0;
	else
		return -1;
}

static int xpan_cli_get_iface(int argc, char *argv[])
{
	/* TODO: Get the interface here */
	ALOGI("Created xpan interface");
	return 0;
}

static int xpan_cli_do_acs(int argc, char *argv[])
{
	int i= 0, size = argc;
	int freq_list[size];

	if (argc < 1) {
		printf("Invalid command.Requires at least 1 frequency");
		return -1;
	}

	while (argc) {
		freq_list[i] = atoi(argv[i]);

		if (freq_list[i] == 0)
			return -1;

		argc--;
		i++;
	}
	argv = &argv[size - 1];
	if (fn.wifi_do_acs(data, freq_list, size) == XPAN_WIFI_STATUS_SUCCESS)
		return 0;

		return -1;
}

static int xpan_cli_monitor_iface(int argc, char *argv[])
{
	fn.xpan_wifi_event_loop(data);
	return 0;
}

static int xpan_cli_enable_stats(int argc, char *argv[])
{
	int interval;

	if (argc < 1)
		interval = 6000;
	else
		interval = atoi(argv[0]);

	if (fn.set_xpan_wifi_stats_enabled(data, true, interval) == XPAN_WIFI_STATUS_SUCCESS)
		return 0;

	return -1;
}

static int xpan_cli_set_usecase_params(int argc, char *argv[])
{
	xpan_usecase_params params;

	if (argc < 1) {
		params.mode = XPAN_USECASE_NONE;
	} else {
	while (argc) {
		if(strcasecmp("gaming", argv[0]) == 0) {
			params.mode = XPAN_USECASE_GAMING;
			argv = &argv[1];
			argc--;
			continue;
		}
		else if (strcasecmp("lossless",argv[0]) == 0) {
			params.mode = XPAN_USECASE_LOSSLESS_STREAMING;
			argv = &argv[1];
			argc--;
			continue;
		}
		else if (strcasecmp("vbc",argv[0])== 0) {
			params.mode = XPAN_USECASE_GAMING_VBC;
			argv = &argv[1];
			argc--;
			continue;
		}

		if (argv && (strcasecmp("si",argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				params.xpan_gaming_vbc_si = atoi(argv[0]);
			argv = &argv[1];
			argc--;
			continue;
		}

		if (argv && (strcasecmp("offset", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				params.xpan_right_earbud_offset = atoi(argv[0]);
			argv = &argv[1];
			argc--;
			continue;
		}

		if (argv && (strcasecmp("left_mac", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				parse_mac_address(argv[0], params.left_mac_addr);
			argv = &argv[1];
			argc--;
			continue;
		}

		if (argv && (strcasecmp("right_mac", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				parse_mac_address(argv[0], params.right_mac_addr);
			argv = &argv[1];
			argc--;
			continue;
		}
	}

	}

	if (fn.set_xpan_usecase_params(data, &params) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("set_usecase_params successful");
		return 0;
	}

	return -1;
}

static int xpan_cli_set_ap_power_save(int argc, char *argv[])
{
	bool set;
	uint64_t dummy_cookie = 0;
	uint64_t *cookie = &dummy_cookie;

	if (argc < 1)
		return -1;

	if (atoi(argv[0]))
		set = true;
	else
		set = false;

	if (fn.set_xpan_ap_power_save(data, set, cookie) == XPAN_WIFI_STATUS_SUCCESS)
		return 0;

	return -1;
}

static int xpan_cli_switch_transport(int argc, char *argv[])
{
	transport_switch_type switch_type;
	transport_switch_status switch_status;

	if (argc < 1)
		return -1;

	while (argc) {
		if (argv && strcasecmp("type", argv[0]) == 0) {
			argv = &argv[1];
			argc--;
			if (argv && strcasecmp("bt", argv[0]) == 0) {
				switch_type = TRANSPORT_SWITCH_TYPE_NON_WLAN;
			} else if (argv && strcasecmp("wlan", argv[0]) == 0) {
				switch_type = TRANSPORT_SWITCH_TYPE_WLAN;
			} else {
				return -1;
			}
			argv = &argv[1];
			argc--;
		}
		if (argv && strcasecmp("status", argv[0]) == 0) {
			argv = &argv[1];
			argc--;
			if (argv && strcasecmp("req", argv[0]) == 0) {
				switch_status = TRANSPORT_SWITCH_STATUS_REQUEST;
			} else if (argv && strcasecmp("rej", argv[0]) == 0) {
				switch_status = TRANSPORT_SWITCH_STATUS_REJECTED;
			} else if (argv && strcasecmp("comp", argv[0]) == 0) {
				switch_status = TRANSPORT_SWITCH_STATUS_COMPLETED;
			} else {
				return -1;
			}
			argv = &argv[1];
			argc--;
		}
	}

	if (fn.xpan_audio_transport_switch(data, switch_type, switch_status) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("Invoking audio_transport_switch successful");
		return 0;
	}

	return -1;
}

static int xpan_cli_set_ap_available(int argc, char *argv[])
{
	uint32_t duration;
	uint16_t dummy_cookie = 0;
	uint16_t *cookie = &dummy_cookie;

	if (argc < 1) {
		ALOGE("%s: Expected atleast 1 argument", __func__);
		return -1;
	}
	duration = atoi(argv[0]);

	if (fn.set_xpan_ap_available(data, duration, cookie) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("Invoking AP availability successful, cookie = %hu", cookie);
		return 0;
	}

	ALOGE("%s: Failed to set AP availability", __func__);
	return -1;
}

static int xpan_cli_cancel_ap_available(int argc, char *argv[])
{
	uint16_t cookie;

	if (argc < 1) {
		ALOGE("%s: Expected atleast 1 argument", __func__);
		return -1;
	}
	cookie = atoi(argv[0]);

	if (fn.cancel_xpan_ap_available(data, cookie) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("Cancelling AP availability successful");
		return 0;
	}

	ALOGE("%s: Failed to cancel AP availability", __func__);
	return -1;
}

static int xpan_cli_set_peer_audio_pair_location(int argc, char *argv[])
{
	xpan_audio_pair_params params;

	if (argc < 1) {
		ALOGE("%s: Invalid Arguments.Required atleast 1 argument", __func__);
		return -1;
	} else {
		while (argc) {
			if (argv && (strcasecmp("set_id",argv[0]) == 0)) {
				argv = &argv[1];
				argc--;
				if (argv)
					params.xpan_peer_set_id = atoi(argv[0]);
				argv = &argv[1];
				argc--;
				continue;
			}

			if (argv && (strcasecmp("left_mac", argv[0]) == 0)) {
				argv = &argv[1];
				argc--;
				if (argv)
					parse_mac_address(argv[0], params.left_mac_addr);
				argv = &argv[1];
				argc--;
				continue;
			}

			if (argv && (strcasecmp("right_mac", argv[0]) == 0)) {
				argv = &argv[1];
				argc--;
				if (argv)
					parse_mac_address(argv[0], params.right_mac_addr);
				argv = &argv[1];
				argc--;
				continue;
			}
			argc--;
		}
	}

	if (fn.set_xpan_peer_audio_pair_location(data, &params) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("set_xpan_peer_audio_pair_location successful");
		return 0;
	}

	ALOGE("Failed to set peer audio pair location");
	return -1;
}

static int xpan_cli_register_iface(int argc, char *argv[])
{
	u8 mac[ETH_ALEN];
	iface_type type = IFACE_TYPE_STA;

	memset(mac, 0, sizeof(mac));
	if (argc < 2) {
		ALOGE("%s: Invalid number of args:%d received, expected args:2", __func__, argc);
		return -1;
	} else {
	while (argc) {
		if (argv && (strcasecmp("mac", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				parse_mac_address(argv[0], mac);
			argv = &argv[1];
			argc--;
			continue;
		}

		if (argv && (strcasecmp("type", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				type = (iface_type)atoi(argv[0]);
			argv = &argv[1];
			argc--;
			continue;
		}
		argc--;
	}
	}

	if (fn.register_iface(data, mac, type) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("register_iface successful");
		return 0;
	}

	ALOGE("Failed to register iface");
	return -1;
}

static int xpan_cli_set_usecase_sta(int argc, char *argv[])
{
	mac_addr mac;
	memset(mac, 0, sizeof(mac_addr));

	if (argc < 2) {
		ALOGE("%s: Invalid number of args:%d received, expected args:2", __func__, argc);
		return -1;
	}

	while (argc) {
		if (argv && strcasecmp("mac", argv[0]) == 0) {
			argv = &argv[1];
			argc--;
			if (argv)
				parse_mac_address(argv[0], mac);
			argv = &argv[1];
			argc--;
			break;
		} else {
			argc--;
		}
	}

	if (fn.register_iface(data, mac, IFACE_TYPE_STA) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("register_iface successful");
		data->cmd_iface = IFACE_TYPE_STA;
		if (xpan_cli_set_usecase_params(argc, argv) < 0)
			ALOGE("Failed to set STA usecase");
		fn.unregister_iface(data, IFACE_TYPE_STA);
		return 0;
	}

	ALOGE("Failed to set sta usecae");
	return -1;
}

static int xpan_cli_unregister_iface(int argc, char *argv[])
{
	iface_type type;

	if (argc < 1) {
		ALOGE("%s: Invalid number of args:%d received, expected args:1", __func__, argc);
		return -1;
	} else {
	while (argc) {
		if (argv && (strcasecmp("type", argv[0]) == 0)) {
			argv = &argv[1];
			argc--;
			if (argv)
				type = (iface_type)atoi(argv[0]);
			argv = &argv[1];
			argc--;
			continue;
		}
		argc--;
	}
	}

	if (fn.unregister_iface(data, type) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("unregister_iface successful");
		return 0;
	}

	ALOGE("Failed to unregister iface");
	return -1;
}

static int xpan_cli_enable_sta_stats(int argc, char *argv[])
{
	mac_addr mac;
	memset(mac, 0, sizeof(mac_addr));

	if (argc < 2) {
		ALOGE("%s: Invalid number of args:%d received, expected args:2", __func__, argc);
		return -1;
	}

	while (argc) {
		if (argv && strcasecmp("mac", argv[0]) == 0) {
			argv = &argv[1];
			argc--;
			if (argv)
				parse_mac_address(argv[0], mac);
			argv = &argv[1];
			argc--;
			break;
		} else {
			argc--;
		}
	}

	if (fn.register_iface(data, mac, IFACE_TYPE_STA) == XPAN_WIFI_STATUS_SUCCESS) {
		ALOGI("register_iface successful");
		data->cmd_iface = IFACE_TYPE_STA;
		if (xpan_cli_enable_stats(argc, argv) < 0)
			ALOGE("Failed to enable stats");
		fn.unregister_iface(data, IFACE_TYPE_STA);
		return 0;
	}

	ALOGE("Failed to enable stats for STA");
	return -1;
}

struct xpan_cli_cmd
{
	const char *cmd;
	int (*handler)(int argc, char *argv[]);
	const char *usage;
};

static const struct xpan_cli_cmd xpan_cli_commands[] = {
	{ "ADD_INTERFACE", xpan_cli_add_iface,
	  "<iface name> = Add a new interface for SAP " },
	{ "DEL_INTERFACE", xpan_cli_del_iface,
	  "<iface name> = Delete the interface for SAP " },
	{ "GET_INTERFACE", xpan_cli_get_iface,
	  "Get the interface for SAP " },
	{ "DO_ACS", xpan_cli_do_acs,
	  "<freq> = Frequency list to perform ACS" },
	{ "MONITOR_INTERFACE", xpan_cli_monitor_iface,
	  "Monitor the events for the interface" },
	{ "ENABLE_STATS", xpan_cli_enable_stats,
	  "<interval> = Enable xpan stats collection with the given interval" },
	{ "SET_USECASE_PARAMS", xpan_cli_set_usecase_params,
	  "[use_case = gaming/lossless/vbc] = Set xpan usecase parameters."
	  "Supported types: gaming,lossless,vbc"
	  "[si = <vbc_si>] [offset = <earbud_offset>] [left_mac = <mac_addr>]"
	  "[right_mac = <mac_addr>]"},
	{ "SET_AP_POWER_SAVE", xpan_cli_set_ap_power_save,
	  "<enable = 1/disable = 0> set ap in power save mode"},
	{ "SWITCH_TRANSPORT", xpan_cli_switch_transport,
	  "[type = BT/WLAN] = Transport to which the switch should be made"
	  "[status = req/rej/comp] = Status of transport switch"},
	{ "SET_XPAN_AP_AVAILABLE", xpan_cli_set_ap_available,
	  "<duration = duration (ms)> set ap in high availability mode for given duration"},
	{ "CANCEL_XPAN_AP_AVAILABLE", xpan_cli_cancel_ap_available,
	  "<cookie = cookie id> cancel ap availability set already for given cookie id"},
	{ "SET_XPAN_PEER_AUDIO_PAIR_LOCATION", xpan_cli_set_peer_audio_pair_location,
	  "Set audio pair parameters."
	  "[set_id = <EB_SET_ID>] [left_mac = <mac_addr>] [right_mac = <mac_addr>]"},
	{ "REGISTER_IFACE", xpan_cli_register_iface,
	  "[mac = MAC address of STA/AP] = Register the STA/AP interface with MAC"
	  "[type = STA/AP iface type] = Type of the interface"},
	{ "UNREGISTER_IFACE", xpan_cli_unregister_iface,
	  "[type = STA/AP iface type] = Type of the interface to unregister the iface"},
	{ "SET_USECASE_STA_PARAMS", xpan_cli_set_usecase_sta,
	  "[mac = MAC address of STA/AP] [use_case = gaming/lossless/vbc] [si = <vbc_si>] "
	  "[offset = <earbud_offset>] [left_mac = <mac_addr>] [right_mac = <mac_addr>] "
	  "= Set xpan usecase parameters. Supported types: gaming,lossless,vbc"},
	{ "ENABLE_STA_STATS", xpan_cli_enable_sta_stats,
	  "[mac = MAC address of STA/AP]"
	  "<interval> = Enable xpan stats collection with the given interval" },

	{ NULL, NULL, NULL}
};

static int xpan_request(int argc, char *argv[])
{
	const struct xpan_cli_cmd *cmd, *match = NULL;
	int count;
	int ret = 0;

	if (argc == 0)
		return -1;

	count = 0;
	cmd = xpan_cli_commands;
	while (cmd->cmd) {
		if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0)
		{
			match = cmd;
			if (strcasecmp(cmd->cmd, argv[0]) == 0) {
				/* we have an exact match */
				count = 1;
				break;
			}
			count++;
		}
		cmd++;
	}
	if (count > 1) {
		printf("Ambiguous command '%s'; possible commands:", argv[0]);
		cmd = xpan_cli_commands;
		while (cmd->cmd) {
			if (strncasecmp(cmd->cmd, argv[0],
					strlen(argv[0])) == 0) {
				printf(" %s", cmd->cmd);
			}
			cmd++;
		}
		printf("\n");
		ret = 1;
	} else if (count == 0) {
		printf("Unknown command '%s'\n", argv[0]);
		ret = 1;
	} else {
		ret = match->handler(argc - 1, &argv[1]);
	}

	return ret;
}

/*
 * Prints command usage, lines are padded with the specified string.
 */
static void print_cmd_help(const struct xpan_cli_cmd *cmd, const char *pad)
{
	char c;
	size_t n;

	printf("%s%s ", pad, cmd->cmd);
	for (n = 0; (c = cmd->usage[n]); n++) {
		printf("%c", c);
		if (c == '\n')
			printf("%s", pad);
	}
	printf("\n");
}


static void print_help(const char *cmd)
{
	int n;
	printf("commands:\n");
	for (n = 0; xpan_cli_commands[n].cmd; n++) {
		if (cmd == NULL || str_starts(xpan_cli_commands[n].cmd, cmd))
			print_cmd_help(&xpan_cli_commands[n], "  ");
	}
}

static void usage(void)
{
	printf("xpan_cli"
	       "[command..]\n"
	       "  -h = help (show this usage text)\n"
	       "  default interface: %s\n", XPAN_DEFAULT_SAP_IFACE);
	print_help(NULL);
}


int main(int argc, char *argv[])
{
	int c, ret;
	char ifname[IFNAMSIZ+1];

	for (;;) {
		c = getopt(argc, argv, "h");
		if (c < 0)
			break;
		switch (c) {
		case 'h' :
			usage();
			break;
		default :
			usage();
			return -1;
		}
	}

	//initialise xpan library
	if (init_xpan_wifi_lib_function_table(&fn)){
		ALOGI("Failed to initialise function table");
		::exit(-1);
	}

	data = (xpan_wifi_data*)fn.init_xpan_wifi_lib(&wifiHandle_);
	if(!data) {
		printf("Xpan data not present\n");
		::exit(-1);
	}

	get_xpan_iface(ifname);
	if (strlen(ifname) > 0) {
		data->ifinfo = (iface_info *)malloc(sizeof(iface_info));
		if (!data->ifinfo) {
			printf("XPAN memory not allocated\n");
			::exit(-1);
		}
		strlcpy(data->ifinfo->ifname, ifname, sizeof(data->ifinfo->ifname));
		data->ifinfo->if_index = if_nametoindex(ifname);
	}
	data->cmd_iface = IFACE_TYPE_SAP;

	dummy_cb.cb_wifi_acs_results = dummy_cb_acs;
	dummy_cb.cb_wifi_twt_event = dummy_cb_twt;
	dummy_cb.cb_ap_power_save_event = dummy_cb_ps;
	dummy_cb.cb_wifi_audio_transport_switch = dummy_cb_switch;
	dummy_cb.cb_xpan_ap_availability = dummy_cb_avail;
	dummy_cb.cb_channel_switch_started = dummy_cb_ch_switch;


	//register for callbacks
	fn.register_callbacks(data, &dummy_cb);

	ret = xpan_request(argc-optind, &argv[optind]);
	printf("STATUS: %s (%d)\n", (ret < 0) ? "FAILURE" : "SUCCESS", ret);

	return 0;
}
