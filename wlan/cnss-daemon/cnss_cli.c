/*
 * Copyright (c) 2019, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "cnss_cli.h"
#include "wlfw_qmi_client.h"
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_CMD_LEN 256
#define MAX_NUM_OF_PARAMS 2
#define MAX_CNSS_CMD_LEN 32

/* Below QDSS File sizes are in MB */
#define MIN_QDSS_FILE_SIZE 1
#define MAX_QDSS_FILE_SIZE 8

#ifdef USE_GLIB
#include <glib.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

static char *cnss_cmd[][2] = {
	{"qdss_trace_start", ""},
	{"qdss_trace_stop", "<Hex base number: e.g. 0x3f>"},
	{"qdss_trace_load_config", ""},
	{"quit", ""}
};

void help(void)
{
	int i = 0;

	fprintf(stderr, "Supported Command:\n");
	for (i = 0; i < (int)ARRAY_SIZE(cnss_cmd); i++)
		fprintf(stderr, "%s %s\n", cnss_cmd[i][0], cnss_cmd[i][1]);
}

static void usage(char *progname)
{
	fprintf(stderr, "%s Usage: [options]\n"
		"   -i along with any other option is mandatory\n"
		"   Sample Usage IPQ: cnsscli -i qcn9000_pci0 --qdss_start, cnsscli -i qcn9000_pci0 --qdss_stop 0x3f\n"
		"   Sample Usage MCC: cnss_cli -i mcc --qdss_start, cnss_cli -i mcc --qdss_stop 0x3f\n\n"
		"   -i, --interface            Interface selected, like integrated qcnXXXX_pciX\n"
		"   --enable_qdss_tracing      Enable automatic starting of QDSS after FW ready\n"
		"   --qdss_start               Do qdss_trace_load_config followed by qdss_trace_start\n"
		"   --qdss_stop                Do qdss_trace_stop with option number\n"
		"   --qdss_file_size           Set QDSS trace data file size between 1MB to 8MB\n"
		"   --enable_daemon_support    Enable daemon support in kernel for given interface\n"
		"   --enable_cold_boot_support Enable cold boot support in kernel for given interface\n"
		"   --enable_hds_support       Enable HDS bin download for given interface\n"
		"   --qmi_timeout              Set QMI timeout value in msec\n"
		"   --enable_regdb_support       Enable REGDB bin download for given interface\n"
		"   -h, --help                 Display this help text\n",
		progname);
}

static int send_cmd_to_daemon(char *cmd_buffer, char *resp_buffer)
{
	int ret = 0;
	int sockfd;
	unsigned int len;
	struct sockaddr_un cl_addr;
	struct sockaddr_un sv_addr;

	if ((strlen(CNSS_USER_CLIENT) > (sizeof(cl_addr.sun_path) - 1)) ||
	    (strlen(CNSS_USER_SERVER) > (sizeof(sv_addr.sun_path) - 1))) {
		fprintf(stderr, "Invalid client/server path %s %s\n",
			CNSS_USER_CLIENT, CNSS_USER_SERVER);
		return -EINVAL;
	}

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Failed to connect to cnss-daemon\n");
		return sockfd;
	}

	memset(&cl_addr, 0, sizeof(cl_addr));
	cl_addr.sun_family = AF_UNIX;
	strlcpy(cl_addr.sun_path, CNSS_USER_CLIENT,
		(sizeof(cl_addr.sun_path) - 1));

	memset(&sv_addr, 0, sizeof(sv_addr));
	sv_addr.sun_family = AF_UNIX;
	strlcpy(sv_addr.sun_path, CNSS_USER_SERVER,
		(sizeof(sv_addr.sun_path) - 1));

	if (bind(sockfd, (struct sockaddr *)&cl_addr, sizeof(cl_addr)) < 0) {
		fprintf(stderr, "Fail to bind client socket %s\n",
			strerror(errno));
		ret = -errno;
		goto out;
	}

#ifdef UJAIL_SUPPORT
	if (lchmod(cl_addr.sun_path,  S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
		fprintf(stderr, "Fail to change client permission for path: %s error: %s\n",
				cl_addr.sun_path, strerror(errno));
	}
#endif
	if (sendto(sockfd, cmd_buffer, CNSS_CLI_MAX_PAYLOAD, 0,
		   (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
		fprintf(stderr, "Failed to send: Error: %s\n",
			strerror(errno));
		ret = -errno;
		goto out;
	}

	/* Send to socket was success, wait for response from daemon */
	if (recvfrom(sockfd, resp_buffer, CNSS_CLI_MAX_PAYLOAD, 0,
		     (struct sockaddr *)&sv_addr, (socklen_t *)&len) <= 0) {
		fprintf(stderr,
			"Failed to get response Error: %s, retrying\n",
			strerror(errno));
		ret = -errno;
	}

out:
	remove(CNSS_USER_CLIENT);
	close(sockfd);
	return ret;
}

int main(int argc, char **argv)
{
	int c;
	char *cmd_buffer = NULL;
	char *resp_buffer = NULL;
	enum cnss_cli_cmd_type type = CNSS_CLI_CMD_NONE;
	struct cnss_cli_msg_hdr *hdr = NULL;
	struct cnss_cli_msg_hdr *resp_hdr = NULL;
	void *cnss_cli_data = NULL;
	struct cnss_cli_config_param_data config_param;
	uint32_t value = 0;
	uint32_t qdss_mode = 0;
	char *interface_name = NULL;
	char *progname = NULL;
	static struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"interface", required_argument, NULL, 'i'},
		{"enable_qdss_tracing", required_argument, NULL, 'q'},
		{"qdss_start", no_argument, NULL, 's'},
		{"qdss_stop", required_argument, NULL, 'x'},
		{"qdss_file_size", required_argument, NULL, 'z'},
		{"enable_daemon_support", required_argument, NULL, 'd'},
		{"enable_cold_boot_support", required_argument, NULL, 'c'},
		{"enable_hds_support", required_argument, NULL, 'H'},
		{"qmi_timeout", required_argument, NULL, 't'},
		{"enable_regdb_support", required_argument, NULL, 'R'},
		{"qdss_streaming_mode", required_argument, NULL, 'm'},
		{0, 0, 0, 0}
	};

	progname = argv[0];

	if (argc < 2) {
		usage(progname);
		goto out;
	}

	while (1) {
		c = getopt_long(argc, argv, "hi:sx:d:c:z:H:t:R:q:m:", options,
				NULL);

		if (c < 0)
			break;

		switch (c) {
		case 'i':
			interface_name = optarg;
			break;
		case 's':
			type = QDSS_TRACE_CONFIG_AND_START;
			break;
		case 'x':
			type = QDSS_TRACE_STOP;
			value = strtoull(optarg, NULL, 0);
			break;
		case 'z':
			type = QDSS_TRACE_DATA_FILE_SIZE;
			value = strtol(optarg, NULL, 0);
			break;
		case 'd':
			type = DAEMON_SUPPORT;
			value = strtol(optarg, NULL, 0);
			break;
		case 'c':
			type = COLD_BOOT_SUPPORT;
			value = strtol(optarg, NULL, 0);
			break;
		case 'H':
			type = HDS_SUPPORT;
			value = strtol(optarg, NULL, 0);
			break;
		case 't':
			type = WLFW_QMI_TIMEOUT;
			value = strtol(optarg, NULL, 0);
			break;
		case 'R':
			type = REGDB_SUPPORT;
			value = strtol(optarg, NULL, 0);
			break;
		case 'q':
			type = QDSS_SUPPORT;
			value = strtol(optarg, NULL, 0);
			break;
		case 'm':
			qdss_mode = strtoull(optarg, NULL, 0);
			break;
		case 'h':
		default:
			usage(progname);
			goto out;
		}
		if ((type != CNSS_CLI_CMD_NONE) && interface_name)
			break;
	}

	if (optind < argc) {
		usage(progname);
		goto out;
	}

	if (!interface_name) {
		fprintf(stderr, "\nMandatory interface option missing!!!");
		goto out;
	}

	switch (type) {
	case QDSS_TRACE_CONFIG_AND_START:
               /* No Value to send for this cmd */
		if (qdss_mode)
			config_param.value = qdss_mode;
		else
			config_param.value = 0;
		break;

	case QDSS_TRACE_DATA_FILE_SIZE:
		if (value < MIN_QDSS_FILE_SIZE || value > MAX_QDSS_FILE_SIZE) {
			fprintf(stderr,
				"\nInvalid value %d, Supported values <1-8>\n",
				value);
			goto out;
		}
		/* If value in valid range, fall-thru */
	case QDSS_TRACE_STOP:
	case WLFW_QMI_TIMEOUT:
	case DAEMON_SUPPORT:
	case COLD_BOOT_SUPPORT:
	case HDS_SUPPORT:
	case REGDB_SUPPORT:
	case QDSS_SUPPORT:
		config_param.value = value;
		break;
	default:
		usage(progname);
		goto out;
	}

	cmd_buffer = calloc(1, CNSS_CLI_MAX_PAYLOAD);
	if (!cmd_buffer) {
		fprintf(stderr, "Failed to allocate cmd_buffer buffer\n");
		goto out;
	}

	resp_buffer = calloc(1, CNSS_CLI_MAX_PAYLOAD);
	if (!resp_buffer) {
		fprintf(stderr, "Failed to allocate resp_buffer\n");
		free(cmd_buffer);
		goto out;
	}

	hdr = (struct cnss_cli_msg_hdr *)cmd_buffer;
	hdr->type = type;
	hdr->len = sizeof(config_param);
	snprintf(hdr->interface, sizeof(hdr->interface),
		 "%s", interface_name);

	cnss_cli_data = (char *)hdr + sizeof(struct cnss_cli_msg_hdr);
	memcpy(cnss_cli_data, &config_param, sizeof(config_param));

	if (send_cmd_to_daemon(cmd_buffer, resp_buffer)) {
		fprintf(stderr,
			"Failed to send command type: %d interface: %s\n",
			type, interface_name);
		goto free_buf;
	}

	/* Check if received a valid response from the daemon */
	resp_hdr = (struct cnss_cli_msg_hdr *)resp_buffer;
	if (hdr->type != resp_hdr->type)
		fprintf(stderr,
			"Invalid response req type:%d, resp_type:%d\n",
			hdr->type, resp_hdr->type);
	else if (resp_hdr->resp_status == -ENODEV)
		fprintf(stderr, "Invalid interface name %s\n\n",
			interface_name);
	else if (resp_hdr->resp_status)
		fprintf(stderr,
			"Response code %d from daemon\n",
			resp_hdr->resp_status);

free_buf:
	free(resp_buffer);
	free(cmd_buffer);
out:
	/* Always return 0 */
	return 0;
}
