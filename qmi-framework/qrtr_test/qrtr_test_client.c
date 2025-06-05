/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            Test Application for CBSP MPROC APT for Socket Test

GENERAL DESCRIPTION
  Contains client implementation of Socket API Test.

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
#include <linux/qrtr.h>
#include <sys/types.h>

#include "libqrtr.h"

#define LOGW(fmt, ...) do { fprintf(stderr, "W|qrtr_test_client: " fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE(fmt, ...) do { fprintf(stderr, "E|qrtr_test_client: " fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE_errno(fmt, ...) do { fprintf(stderr, "E|qrtr_test_client: " fmt ": %s\n", ##__VA_ARGS__, strerror(errno)); } while (0)

#define SOCKET_SUCCESS		0
#define SOCKET_FAILURE		1
#define SOCKET_BUSY_ERR		2
#define SOCKET_SERVICE_ERR	3
#define MAX_SERVICE_NUM		20

static char* progname;
static pthread_t recv_thread;
static int data_size = 1024;
static int is_rx_thread_schedule = 0;
static int iteration = 10;
static int lbserv = 1;
static int checksum = 0;
static int print_mod = 100;
static int rx_done_flag = 0;
static int client_fd = -1;

struct qrtr_service_info {
	uint32_t service;
	uint16_t instance;
	struct sockaddr_qrtr sq;
};

struct qrtr_service_list {
	struct qrtr_service_info info[MAX_SERVICE_NUM];
	int count;
};
static struct qrtr_service_list service_list;

int qrtr_client_sendto(int fd, struct qrtr_service_info *info, void *buf) {

	struct sockaddr_qrtr sq;
	int len;
	int ret = SOCKET_SUCCESS;

	sq.sq_family = AF_QIPCRTR;
	sq.sq_node = info->sq.sq_node;
	sq.sq_port = info->sq.sq_port;

	len = sendto(fd, buf, data_size, MSG_DONTWAIT, (struct sockaddr *)&sq, sizeof(sq));
	if ((len < 0) && (errno == EAGAIN)) {
		LOGE_errno("Remote port addr[%08x:%08x] flow controlled", sq.sq_node, sq.sq_port);
		ret = SOCKET_BUSY_ERR;
	} else if ((len < 0) && (errno == ENODEV || errno == EHOSTUNREACH)) {
		LOGE_errno("Remote service gone");
		ret = SOCKET_SERVICE_ERR;
	} else if (len < 0) {
		LOGE_errno("sendto()");
		ret = SOCKET_FAILURE;
	}

	return ret;
}

int checksum_check(void *buf, int len)
{
	uint32_t calc_sum, msg_sum, sum;
	uint8_t data;
	uint8_t *ptr;
	int sum_size;
	int i;

	sum_size = (len >= sizeof(sum)) ? sizeof(sum) : len;
	ptr = (uint8_t *)buf + sum_size;
	len = len - sum_size;

	sum = 0;
	calc_sum = 0;
	for (i = 0; i < len; i++) {
		data = *(ptr + i);
		sum = sum ^ data;
	}
	memcpy(&msg_sum, buf, sum_size);
	memcpy(&calc_sum, &sum, sum_size);
	if (calc_sum != msg_sum) {
		printf("Checksum does not match msg_csum:0x%x calculated_csum:0x%x\n", msg_sum, calc_sum);
		return 0;
	}
	return 1;
}

void fill_tx_buf(void *buf, int len)
{
	uint32_t sum;
	uint8_t data;
	uint8_t *ptr;
	int sum_size;
	int i;

	sum_size = (len >= sizeof(sum)) ? sizeof(sum) : len;
	ptr = (uint8_t *)buf + sum_size;
	len = len - sum_size;

	sum = 0;
	for (i = 0; i < len; i++) {
		data = (uint8_t)(rand() & 0xff);
		*(ptr + i) = data;
		sum = sum ^ data;
	}
	memcpy(buf, &sum, sum_size);
}

/**
@brief - This routine will keep poll for any incoming data
 */
void *recv_loop(void *data)
{
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	socklen_t sl;
	int buf_len;
	size_t len;
	void *buf;
	int rc;
	int i;

	is_rx_thread_schedule = 1;
	if (client_fd < 0)
		return NULL;

	buf_len = (data_size < sizeof(struct qrtr_ctrl_pkt)) ? sizeof(struct qrtr_ctrl_pkt) : data_size;
	buf = malloc(buf_len);
	if (!buf) {
		LOGE_errno("malloc(): failed with size:%d\n", buf_len);
		return NULL;
	}

	for (i = 0; i < iteration; i++) {
		rc = qrtr_poll(client_fd, -1);
		if (rc < 0) {
			LOGE_errno("qrtr_poll(): iteration:%d", i);
			continue;
		}

		sl = sizeof(sq);
		len = recvfrom(client_fd, buf, buf_len, MSG_DONTWAIT, (struct sockaddr *)&sq, &sl);
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
		if (pkt.type != QRTR_TYPE_DATA)
			i--;

		if (checksum) {
			rc = checksum_check(buf, len);
			if ((i % print_mod) == 0)
				printf("RX: i:%d | %lu bytes from addr[0x%08x:0x%08x] validated:%d\n",
					i, len, sq.sq_node, sq.sq_port, rc);
		} else {
			if ((i % print_mod) == 0)
				printf("RX: i:%d | %lu bytes from addr[0x%08x:0x%08x]\n",
					i, len, sq.sq_node, sq.sq_port);
		}

		rx_done_flag = 1;
	}
	free(buf);

	return data;
}

/**
@brief - Lookup services with matching Service and Instance IDs. Fill the provided service_list struct with address
         information received from the ns.
*/
static int service_lookup(unsigned int service, unsigned int instance,
			  unsigned int version,
			  struct qrtr_service_list *service_list)
{
	struct qrtr_ctrl_pkt buf;
	struct sockaddr_qrtr sq;
	struct qrtr_packet pkt;
	socklen_t sl;
	size_t len;
	int sock;
	int rc;
	int i = 0;

	printf("sending lookup for service:%d instance:%d\n", service, instance);
	sock = qrtr_open(0);
	if (sock < 0) {
		LOGE_errno("qrtr_open(): lookup_socket");
		return 0;
	}

	rc = qrtr_new_lookup(sock, service, version, instance);
	if (rc < 0) {
		LOGE_errno("qrtr_new_lookup()");
		goto exit;
	}

	printf("  Service Instance Node  Port\n");
	memset(&pkt, 0, sizeof(pkt));
	sl = sizeof(sq);
	while ((len = recvfrom(sock, &buf, sizeof(buf), 0, (struct sockaddr *)&sq, &sl)) > 0) {
		rc = qrtr_decode(&pkt, &buf, sizeof(buf), &sq);
		if (pkt.type != QRTR_TYPE_NEW_SERVER)
			continue;
		/* Special end packet for lookup list */
		if (!pkt.service && !pkt.instance && !pkt.node && !pkt.port)
			break;

		printf("%9d %8d %4d %5d\n", pkt.service, pkt.instance, pkt.node, pkt.port);
		if (i < MAX_SERVICE_NUM) {
			service_list->info[i].service = pkt.service;
			service_list->info[i].instance = pkt.instance;
			service_list->info[i].sq.sq_node = pkt.node;
			service_list->info[i].sq.sq_port = pkt.port;
			i++;
		}
	}
	service_list->count = i;

exit:
	qrtr_close(sock);
	return i;
}

struct qrtr_service_info *find_service_by_node(struct qrtr_service_list *list, int node)
{
	int i;

	if (!list->count)
		return NULL;

	/* If no node id is specified, return first service in list */
	if (node < 0)
		return &list->info[0];

	for (i = 0; i < list->count; i++) {
		if (list->info[i].sq.sq_node == node)
			return &list->info[i];

	}
	printf("No service with Node ID:%d, using first service from lookup\n", node);
	return &list->info[i];
}

void usage(void)
{
	printf("Usage for %s:\n"
	       "-s, --service_id:    Service id of test service to connect to\n"
	       "-e, --instance_id:   Instance id of test service to connect to\n"
	       "-v, --version:       Version of test service to connect to\n"
	       "-n, --data_size:     Data size of payload\n"
	       "-i, --iteration:     Number of iterations\n"
	       "-l, --noloopback:    Specify if client should not wait for server loopback\n"
	       "-c, --checksum:      validate received buffer contents with checksum\n"
	       "-r, --node:          Node id to try and connect to\n"
	       "-h, --help:          Usage help\n"
	       "e.g. qrtr_test_client -s <service_id> -e <instance_id> -n <data_size> -i <iteration> -r <node id> -l -v\n",
	       progname);
}

/**
Driver routine to start client
*/
int main (int argc, char *argv[]) {
	struct qrtr_service_info *service_info;
	struct option longopts[] =
	{
		{ "service_id", 1, NULL, 's'},
		{ "instance_id", 1, NULL, 'e'},
		{ "version", 1, NULL, 'v'},
		{ "data_size", 1, NULL, 'n'},
		{ "iteration", 1, NULL, 'i'},
		{ "noloopback", 0, NULL, 'l'},
		{ "checksum", 0, NULL, 'c'},
		{ "node", 1, NULL, 'r'},
		{ "help", 0, NULL, 'h'},
	};
	int service_id = 0xF;
	int instance_id = 0;
	int version = 1;
	int node_id = -1;
	int wait_count;
	int count;
	void *buf;
	int opt;
	int rc;
	int i;

	progname = argv[0];
	if (getuid() != 0) {
		printf("%s: You must be root to run\n", progname);
		return 0;
	}
	while ((opt = getopt_long(argc, argv, "s:e:v:n:i:lcr:h", longopts, NULL)) != -1) {
		switch (opt) {
		case 's':
			service_id = atol(optarg);
			printf("service id:%d\n", service_id);
			break;
		case 'e':
			instance_id = atol(optarg);
			printf("instance id:%d\n", instance_id);
			break;
		case 'v':
			version = atol(optarg);
			printf("version:%d\n", version);
			break;
		case 'n':
			data_size = atol(optarg);
			printf("data_size:%d\n", data_size);
			break;
		case 'i':
			iteration = atol(optarg);
			printf("iteration:%d\n", iteration);
			break;
		case 'l':
			lbserv = 0;
			printf("loopback flag:%d\n", lbserv);
			break;
		case 'c':
			checksum = 1;
			printf("checksum flag:%d\n", checksum);
			break;
		case 'r':
			node_id = atol(optarg);
			printf("node_id to connect:%d\n", node_id);
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

	memset(&service_list, 0, sizeof(service_list));
	count = service_lookup(service_id, instance_id, version, &service_list);
	if (!count) {
		LOGE("No matching servers found\n");
		goto fail;
	}
	service_info = find_service_by_node(&service_list, node_id);
	if (!service_info) {
		LOGE("No matching server by node\n");
		goto fail;
	}

	client_fd = qrtr_open(0);
	if (client_fd < 0) {
		LOGE_errno("qrtr_open(): lookup_socket");
		goto fail;
	}

	printf("Connect to SRV[0x%x:0x%x] ADDR[0x%x:0x%x]\n",
		service_info->service, service_info->instance,
		service_info->sq.sq_node, service_info->sq.sq_port);

	/*reset the schedule flag*/
	is_rx_thread_schedule = 0;

	/*Create thread to receive the data if expecting server loopback*/
	if (lbserv) {
		if (pthread_create(&recv_thread, NULL, recv_loop, NULL)) {
			LOGE_errno("pthread creation failed");
			qrtr_close(client_fd);
			goto fail;
		}

		while (!is_rx_thread_schedule) {
			printf("%s: wait till RX thread get scheduled\n", progname);
			usleep(20000);/*sleep for 2000000 microsec*/
		}
	}

	buf = malloc(data_size);
	if (!buf) {
		LOGE_errno("malloc(): failed with size:%d", data_size);
		goto fail;
	}
	fill_tx_buf(buf, data_size);

	for(i = 0; i < iteration; i++) {
		rx_done_flag = 0;
		wait_count = 0;

		rc = qrtr_client_sendto(client_fd, service_info, buf);
		if (rc == SOCKET_BUSY_ERR) {
			/* wait a little and keep retrying */
			usleep(1000);
			i--;
			continue;
		} else if (rc != SOCKET_SUCCESS){
			LOGE("qrtr_client_sento() failed:%d, iteration:%d\n", rc, i);
			goto cancel;
		}
		if ((i % print_mod) == 0)
			printf("TX: i:%d | success\n", i);

		while (lbserv && !rx_done_flag && wait_count < 100) {
			usleep(200000); /*sleep for 200000 microsec*/
			wait_count++;
		}
		if (wait_count == 100) {
			LOGE("timeout socket no rx from server iteration:%d\n", i);
			goto cancel;
		}
		if (lbserv & ((i % print_mod) == 0))
			printf("RX: i:%d | success\n", i);
	}

	/* This cleanup is improper, should add pipe to notify rx thread that it is done */
	if (lbserv)
		pthread_join(recv_thread, NULL);
	qrtr_close(client_fd);
	free(buf);
	printf("%s: Success: Exiting...\n", progname);
	return 0;

cancel:
	qrtr_close(client_fd);
	free(buf);
fail:
	printf("%s: Failed: Exiting...\n", progname);
	return 0;
}
