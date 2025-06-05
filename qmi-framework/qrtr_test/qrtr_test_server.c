/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            Test Application for CBSP MPROC APT for Socket Test

GENERAL DESCRIPTION
  Contains service implementation of Socket API Test.

EXTERNALIZED FUNCTIONS
  None

  ---------------------------------------------------------------------------
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <errno.h>
#include <getopt.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <linux/qrtr.h>
#include <sys/types.h>

#include "libqrtr.h"

#define LOGW(fmt, ...) do { fprintf(stderr, "W|qrtr_test_server: " fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE(fmt, ...) do { fprintf(stderr, "E|qrtr_test_server: " fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE_errno(fmt, ...) do { fprintf(stderr, "E|qrtr_test_server: " fmt ": %s\n", ##__VA_ARGS__, strerror(errno)); } while (0)

/*Constant Decleration*/
#define SOCKET_SUCCESS 0
#define SOCKET_FAILURE 1

#define UDP_MAX_DATA_SIZE 65535

static char *progname;
static int service_id = 0xF; /*ping service*/
static int instance_id = 0;
static int version = 1;
static int data_size = UDP_MAX_DATA_SIZE;
static int iteration = INT_MAX;
static int print_mod = 1;
static int lbserv = 1;
static pthread_t service_pthread;
static int service_fd = -1;

int service_loop(void)
{
	int buf_len;
	void *buf;
	int i;

	buf_len = (data_size < sizeof(struct qrtr_ctrl_pkt)) ? sizeof(struct qrtr_ctrl_pkt) : data_size;
	buf = malloc(buf_len);
	if (!buf) {
		LOGE_errno("malloc(): failed with size:%d\n", buf_len);
		return SOCKET_FAILURE;
	}

	for (i = 0; i < iteration; i++) {
		struct sockaddr_qrtr sq;
		struct qrtr_packet pkt;
		socklen_t sl;
		size_t len;
		int rc;

		rc = qrtr_poll(service_fd, -1);
		if (rc < 0) {
			LOGE_errno("qrtr_poll(): iteration:%d", i);
			continue;
		}

		sl = sizeof(sq);
		len = recvfrom(service_fd, buf, buf_len, MSG_DONTWAIT, (struct sockaddr *)&sq, &sl);
		if (len < 0) {
			LOGE_errno("recvfrom():");
			break;
		}
		if (len == 0) {
			if (sl == sizeof(sq))
				printf("received tx_resume on for addr[0x%08x:0x%08x]\n", sq.sq_node, sq.sq_port);
			else
				LOGE("Error POLLIN with no data to read itr:%d\n", i);
			continue;
		}
		rc = qrtr_decode(&pkt, buf, len, &sq);
		if (rc < 0) {
			LOGE("qrtr_decode(): %d", rc);
			break;
		}
		switch (pkt.type) {
			case QRTR_TYPE_DATA:
				//printf("[svc] got Data\n");
				break;
			case QRTR_TYPE_BYE:
				//fprintf(stderr, "[svc] got bye\n");
				i--; /*To accommodate new data request*/
				continue;
			case QRTR_TYPE_DEL_CLIENT:
				//fprintf(stderr, "[svc] got del_client\n");
				i--; /*To accommodate new data request*/
				continue;
			default:
				//fprintf(stderr, "[svc] got type %d\n", pkt.type);
				i--; /*To accommodate new data request*/
				continue;
		}
		if ((i % print_mod) == 0)
			printf("Received %lu bytes from addr[0x%08x:0x%08x] itr:%d\n", len, sq.sq_node, sq.sq_port, i);

		if (lbserv) {
			sl = sizeof(sq);
			len = sendto(service_fd, pkt.data, pkt.data_len, MSG_DONTWAIT, (struct sockaddr *)&sq, sl);
			if (len < 0) {
				LOGE_errno("sendto() addr[0x%08x:0x%08x]", sq.sq_node, sq.sq_port);
				break;
			}
			if ((i % print_mod) == 0)
				printf("Sent %lu bytes to addr[0x%08x:0x%08x] itr:%d\n", len, sq.sq_node, sq.sq_port, i);
		}
	}
	free(buf);

	return SOCKET_SUCCESS;
}

/**
service_main: this routine will create the socket, send new server packet to ns and finally do main recv loop
@priv: NULL

return:	priv
 */
void *service_main(void *priv)
{
	int rc;

	service_fd = qrtr_open(0);
	if (service_fd < 0) {
		LOGE_errno("qrtr_open()");
		return priv;
	}

	if (qrtr_new_server(service_fd, service_id, version, instance_id) < 0) {
		LOGE_errno("qrtr_new_server()");
		goto exit;
	}
	printf("Advertised fd:%d as service[0x%x:0x%x]\n", service_fd, service_id,
	       instance_id << 8 | version);

	rc = service_loop();
	if (rc != SOCKET_SUCCESS)
		LOGE("service_loop failed %d\n", rc);

exit:
	qrtr_close(service_fd);
	return priv;
}

void usage(void)
{
	printf("Usage for %s:\n"
	       "-e, --instance_id:\t instance id\n"
	       "-v, --version:\t version of service\n"
	       "-n, --data_size:\t data size\n"
	       "-i, --iteration:\t no of iteration\n"
	       "-l, --noloopback:\tSpecify if client should not wait for server loopback\n"
	       "-h, --help:\t usage help\n"
	       "e.g. qrtr_test_server -e <instance_id> -n <data_size> -i <iteration> -l\n",
	       progname);
}

int main(int argc, char *argv[])
{
	struct option longopts[] =
	{
		{ "instance_id",	1,		NULL,	'e'},
		{ "version",		1,		NULL,	'v'},
		{ "data_size",		1,		NULL,	'n'},
		{ "iteration",		1,		NULL,	'i'},
		{ "noloopback",		0,		NULL,	'l'},
		{ "help",		0,		NULL,	'h'},
	};
	int opt;

	progname = argv[0];
	if (getuid() != 0) {
		printf("%s: You must be root to run\n", progname);
		goto fail;
	}
	while ((opt = getopt_long(argc, argv, "e:v:n:i:lh", longopts, NULL)) != -1) {
		switch (opt) {
		case 'e':
			instance_id = atol(optarg);
			printf("instance_id:%d\n", instance_id);
			break;
		case 'v':
			version = atol(optarg);
			printf("version:%d\n", version);
			break;
		case 'n':
			data_size = atol(optarg);
			printf("data_size:%d\n", data_size);
			if (data_size > UDP_MAX_DATA_SIZE) {
				printf("data_size too big, n < %d\n", UDP_MAX_DATA_SIZE);
				goto fail;
			}
			break;
		case 'i':
			iteration = atol(optarg);
			printf("iteration:%d\n", iteration);
			break;
		case 'l':
			lbserv = 0;
			printf("loopback flag:%d\n", lbserv);
			break;
		case 'h':
		default:
			usage();
			exit(0);
		};
	}

	if (iteration < 100)
		print_mod = 1;
	else
		print_mod = 100;

	if (pthread_create(&service_pthread, NULL, service_main, NULL)){
		LOGE_errno("pthread creation failed");
		goto fail;
	}
	printf("service thread started\n");
	pthread_join(service_pthread, NULL);

	printf("%s: Success: Exiting...\n", progname);
	return SOCKET_SUCCESS;

fail:
	printf("%s: Failed: Exiting...\n", progname);
	return SOCKET_FAILURE;
}
