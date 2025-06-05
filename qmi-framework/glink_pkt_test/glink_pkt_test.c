/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            Test Application for MPROC GLINK PKT for Userspace

GENERAL DESCRIPTION
  Contains userspace test code for glink pkt driver.

EXTERNALIZED FUNCTIONS
  None

  ---------------------------------------------------------------------------
  Copyright (c) 2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>
#include <sys/mman.h>

#define GLINK_PKT_IOCTL_MAGIC (0xC3)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
#define MAX_NAME_LEN 32
#define MAX_SIZE (4 * 4096)
#define GLINK_CNTL_DEV   "/dev/glink_pkt_ctrl_cdsp"
#define GLINK_DATA_DEV   "/dev/glink_pkt_data_cdsp"

/**
 * Unit test assertion for logging test cases.
 *
 * @a lval
 * @b rval
 * @cmp comparison operator
 *
 * Assertion fails if (@a cmp @b) is not true which then
 * logs the function and line number where the error occurred
 * along with the values of @a and @b.
 *
 * Assumes that the following local variable exist:
 * @failed - set to true if test fails
 */
#define UT_ASSERT_INT(a, cmp, b) \
	{ \
	int a_tmp = (a); \
	int b_tmp = (b); \
	if (!((a_tmp)cmp(b_tmp))) { \
		printf("%s:%d Fail: " #a "(%d) " #cmp " " #b "(%d)\n", \
				__func__, __LINE__, a_tmp, b_tmp); \
		failed = 1; \
		break; \
	} \
	}

#define UT_ASSERT_PTR(a, cmp, b) \
	{ \
	void *a_tmp = (a); \
	void *b_tmp = (b); \
	if (!((a_tmp)cmp(b_tmp))) { \
		printf("%s:%d Fail: " #a "(%p) " #cmp " " #b "(%p)\n", \
				__func__, __LINE__, a_tmp, b_tmp); \
		failed = 1; \
		break; \
	} \
	}

/*
 * LOOPBACK COMMAND TYPE
 */
enum request_type {
	OPEN = 1,
	CLOSE,
	QUEUE_RX_INTENT_CONFIG,
	TX_CONFIG,
	RX_DONE_CONFIG,
};

/*
 * struct req_hdr - Loopback command request header
 * @req_id:	Identifier of the request command.
 * @req_type:	Type of the request command.
 * @req_size:	Size of the request command.
 */
struct req_hdr {
	uint32_t req_id;
	uint32_t req_type;
	uint32_t req_size;
};

/*
 * struct open_req - Loopback command open request
 * @delay_ms:	Time delay to process open request.
 * @name_len:	Length of the channel name.
 * @ch_name:	Channel name.
 */
struct open_req {
	uint32_t delay_ms;
	uint32_t name_len;
	char ch_name[MAX_NAME_LEN];
};

/*
 * struct close_req - Loopback command close request
 * @delay_ms:	Time delay to process close request.
 * @name_len:	Length of the channel name.
 * @ch_name:	Channel name.
 */
struct close_req {
	uint32_t delay_ms;
	uint32_t name_len;
	char ch_name[MAX_NAME_LEN];
};

/*
 * struct queue_rx_intent_config_req - queue rx intent request
 * @num_intents:	Number of intents to be queued.
 * @intent_size:	Size of the intent.
 * @random_delay:	Random delay to process the request.
 * @delay_ms:		Time delay to process close request.
 * @name_len:		Length of the channel name.
 * @ch_name:		Channel name.
 */
struct queue_rx_intent_config_req {
	uint32_t num_intents;
	uint32_t intent_size;
	uint32_t random_delay;
	uint32_t delay_ms;
	uint32_t name_len;
	char ch_name[MAX_NAME_LEN];
};

enum transform_type {
	NO_TRANSFORM = 0,
	PACKET_COUNT,
	CHECKSUM,
};

/*
 * struct tx_config_req - Tx data configuration request
 * @random_delay:	Random delay to process the request.
 * @delay_ms:		Time delay to process close request.
 * @echo_count:		Echo count for Tx data.
 * @transform_type:	Type of Tx data.
 * @name_len:		Length of the channel name.
 * @ch_name:		Channel name.
 */
struct tx_config_req {
	uint32_t random_delay;
	uint32_t delay_ms;
	uint32_t echo_count;
	uint32_t transform_type;
	uint32_t name_len;
	char ch_name[MAX_NAME_LEN];
};

/*
 * struct rx_done_config_req - Rx done data configuration request
 * @random_delay:	Random delay to process the request.
 * @delay_ms:		Time delay to process close request.
 * @name_len:		Length of the channel name.
 * @ch_name:		Channel name.
 */
struct rx_done_config_req {
	uint32_t random_delay;
	uint32_t delay_ms;
	uint32_t name_len;
	char ch_name[MAX_NAME_LEN];
};

/*
 * union req_payload - Loppback request payload
 * @open:		Open request.
 * @close:		Close request.
 * @q_rx_int_conf:	Queue rx intent config request.
 * @tx_conf:		Tx config request.
 * @rx_done_conf:	Rx donr config request.
 */
union req_payload {
	struct open_req open;
	struct close_req close;
	struct queue_rx_intent_config_req q_rx_int_conf;
	struct tx_config_req tx_conf;
	struct rx_done_config_req rx_done_conf;
};

/*
 * struct req - Loopback request structure
 * @hdr:	Loopback request header.
 * @payload:	Loopback request payload.
 */
struct req {
	struct req_hdr hdr;
	union req_payload payload;
};

/**
 * Struct resp - Loopback response structure
 * @req_id:	Loopback request identifier.
 * @req_type:	Loopback request command type.
 * @response:	Loopback command response.
 */
struct resp {
	uint32_t req_id;
	uint32_t req_type;
	uint32_t response;
};

static unsigned int request_id;

static char *data_device_nm = GLINK_DATA_DEV;
static char *cntl_device_nm = GLINK_CNTL_DEV;

static int cntl_fd;
static int data_fd;

static int iterations = 30;
static unsigned int resp_size = 12;
static const char *test_data = "hello glink!";
static unsigned int test_data_len = 12;
static char rx_test_data[12];
static int pattern = 0xAA;
static const char *data_ch_name = "LOOPBACK_DATA_CDSP";
static struct req pkt;
static struct resp resp_pkt;
typedef int (*test_func)();
static int basic_test(void);
static pthread_t getsigs_pthread;
static pthread_t setsigs_pthread;
static int set_sigs;
static int stop_signal_test = 0;
test_func do_test = basic_test;

/**
 * send_open_cmd() - Send open command to loopback server
 * @fd:	File descriptor of loopback server control port.
 *
 * This function is used to send the open command request
 * to Loopback server over the control port @fd.
 *
 * Return: 0 for success and -1 for failure.
 */
int send_open_cmd(int fd)
{
	int ret;
	int failed = 0;

	do {

		/* prepare OPEN command */
		pkt.hdr.req_id = request_id++;
		pkt.hdr.req_type = OPEN;
		pkt.hdr.req_size = sizeof(struct open_req);
		pkt.payload.open.delay_ms = 0;
                snprintf(pkt.payload.open.ch_name, sizeof(pkt.payload.open.ch_name), "%s", data_ch_name);

		pkt.payload.open.name_len = strlen(data_ch_name);;

		ret = write(fd, &pkt, sizeof(struct req));
		UT_ASSERT_INT(ret, ==, sizeof(struct req));

		ret = read(fd, &resp_pkt, sizeof(struct resp));
		UT_ASSERT_INT(ret, ==, sizeof(struct resp));
	} while (0);

	ret = failed ? -1 : 0;
	return ret;
}

/**
 * send_tx_config_cmd() - Send tx config command to loopback server
 * @fd:	File descriptor of loopback server control port.
 *
 * This function is used to send the tx config command request
 * to Loopback server over the control port @fd.
 *
 * Return: 0 for success and -1 for failure.
 */
int send_tx_config_cmd(int fd)
{
	int ret;
	int failed = 0;

	do {
		/* prepare TX_CONFIG command */
		pkt.hdr.req_id = request_id++;
		pkt.hdr.req_type = TX_CONFIG;
		pkt.hdr.req_size = sizeof(struct tx_config_req);
		pkt.payload.tx_conf.random_delay = 0;
		pkt.payload.tx_conf.delay_ms = 0;
		pkt.payload.tx_conf.echo_count = 1;
		pkt.payload.tx_conf.transform_type = 0;
                snprintf(pkt.payload.tx_conf.ch_name, sizeof(pkt.payload.tx_conf.ch_name), "%s", data_ch_name);
		pkt.payload.tx_conf.name_len = strlen(data_ch_name);;
                printf("Channel info: %s %d\n", pkt.payload.tx_conf.ch_name, pkt.payload.tx_conf.name_len);

		ret = write(fd, &pkt, sizeof(struct req));
		UT_ASSERT_INT(ret, ==, sizeof(struct req));

		ret = read(fd, &resp_pkt, sizeof(struct resp));
		UT_ASSERT_INT(ret, ==, sizeof(struct resp));
	} while (0);

	ret = failed ? -1 : 0;
	return ret;
}

/**
 * send_rx_done_config_cmd() - Send rx done config command to loopback server
 * @fd:	File descriptor of loopback server control port.
 *
 * This function is used to send the rx done config command request
 * to Loopback server over the control port @fd.
 *
 * Return: 0 for success and -1 for failure.
 */
int send_rx_done_config_cmd(int fd)
{
	int ret;
	int failed = 0;

	do {
		/* prepare RX_DONE_CONFIG command */
		pkt.hdr.req_id = request_id++;
		pkt.hdr.req_type = RX_DONE_CONFIG;
		pkt.hdr.req_size = sizeof(struct rx_done_config_req);
		pkt.payload.rx_done_conf.random_delay = 0;
		pkt.payload.rx_done_conf.delay_ms = 0;
                snprintf(pkt.payload.rx_done_conf.ch_name, sizeof(pkt.payload.rx_done_conf.ch_name), "%s", data_ch_name);
		pkt.payload.rx_done_conf.name_len = strlen(data_ch_name);;

		ret = write(fd, &pkt, sizeof(struct req));
		UT_ASSERT_INT(ret, ==, sizeof(struct req));

		ret = read(fd, &resp_pkt, sizeof(struct resp));
		UT_ASSERT_INT(ret, ==, sizeof(struct resp));
	} while (0);

	ret = failed ? -1 : 0;
	return ret;
}

/**
 * send_close_cmd() - Send close command to loopback server
 * @fd:	File descriptor of loopback server control port.
 *
 * This function is used to send the close command request
 * to Loopback server over the control port @fd.
 *
 * Return: 0 for success and -1 for failure.
 */
int send_close_cmd(int fd)
{
	int ret;
	int failed = 0;

	do {
		/* prepare CLOSE command */
		pkt.hdr.req_id = request_id++;
		pkt.hdr.req_type = CLOSE;
		pkt.hdr.req_size = sizeof(struct close_req);
		pkt.payload.close.delay_ms = 0;
                snprintf(pkt.payload.close.ch_name, sizeof(pkt.payload.close.ch_name), "%s", data_ch_name);
		pkt.payload.close.name_len = strlen(data_ch_name);;

		ret = write(fd, &pkt, sizeof(struct req));
		UT_ASSERT_INT(ret, ==, sizeof(struct req));

		ret = read(fd, &resp_pkt, sizeof(struct resp));
		UT_ASSERT_INT(ret, ==, sizeof(struct resp));
	} while (0);

	ret = failed ? -1 : 0;
	return ret;
}

/**
 * basic_test() - basic functions test on glink packet device
 *
 * This function used to perform the basic function like
 * open, read, write, close and ioctl on glink packet device.
 *
 * Return: 0 for success and -1 for failure.
 */
static int basic_test(void)
{
	int ret;
	int failed = 0;
	printf("Running Basic Test...\n");
	do {
                printf("Sending 'open' command...\n");
		ret = send_open_cmd(cntl_fd);
		UT_ASSERT_INT(ret, ==, 0);

                printf("Sending 'tx config' command...\n");
                ret = send_tx_config_cmd(cntl_fd);
                UT_ASSERT_INT(ret, ==, 0);

                printf("Sending 'rx done config' command...\n");
                ret = send_rx_done_config_cmd(cntl_fd);
                UT_ASSERT_INT(ret, ==, 0);

                printf("Opening data device...\n");
		data_fd = open(data_device_nm, O_RDWR); UT_ASSERT_INT(data_fd, >, 0);

                test_data_len = strlen(test_data);

                printf("Basic test write operation: sending data:%s"
                                    " length:%d\n", test_data, test_data_len);
		ret = write(data_fd, test_data, test_data_len);
		UT_ASSERT_INT(ret, ==, test_data_len);

		ret = read(data_fd, rx_test_data, test_data_len);
		UT_ASSERT_INT(ret, ==, test_data_len);
                if (ret >= 0) {
                    // Null terminate the received data
                    rx_test_data[ret] = '\0';
                    printf("Basic test read operation returned: ret:%d,"
                                    "data recieved:%s\n", ret, rx_test_data);
                }
		UT_ASSERT_INT(strcmp(test_data, rx_test_data), ==, 0);

		ret = send_close_cmd(cntl_fd);
		UT_ASSERT_INT(ret, ==, 0);

		ret = close(data_fd);
		UT_ASSERT_INT(ret, ==, 0);
	} while (0);

	ret = failed ? -1 : 0;
	return ret;
}

/**
 * start_test() - start test on glink packet device
 *
 * This function is used to setup basic communication with loopback server
 * on control port and to proceed with any testing on glink packet device.
 */
void start_test(void)
{
	int ret = 0;
	int failed = 0;

	do {
                printf("Opening control channel...\n");
		cntl_fd = open(cntl_device_nm, O_RDWR);
		UT_ASSERT_INT(cntl_fd, >, 0);

		usleep(10);

		UT_ASSERT_INT(basic_test(), ==, 0);

		ret = close(cntl_fd);
		UT_ASSERT_INT(ret, ==, 0);

		printf("Test PASSED\n");
	} while (0);

	if (failed) {
		printf("Test FAILED\n");
		close(data_fd);
		close(cntl_fd);
	}
}

int main(int argc, char**argv)
{
	start_test();
	return 0;
}
