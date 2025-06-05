/******************************************************************************
  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>

#define LOGI(...) printf(__VA_ARGS__)
#define LOGE(...) printf(__VA_ARGS__)

#define PORT     0x6666
#define UDP_MAX_DATA_SIZE 65535


#ifndef AF_QMSGQ
#define AF_QMSGQ        27
#define PF_QMSGQ        AF_QMSGQ
#endif

sa_family_t family = AF_QMSGQ;

void set_socket_family(void)
{
    int fd;

    fd = socket(AF_QMSGQ, SOCK_DGRAM, 0);
    printf("fd:%d errno:%d\n", fd, errno);
    if (fd < 0 && errno == EAFNOSUPPORT) {
        family = AF_VSOCK;
    } else {
        family = AF_QMSGQ;
        close(fd);
    }
}

int get_checksum(void *buf, int len)
{
    uint32_t sum;
    uint8_t data;
    uint8_t *ptr;
    int i;

    ptr = (uint8_t *)buf;
    sum = 0;
    for (i = 0; i < len; i++) {
        data = *(ptr + i);
        sum = sum ^ data;
    }
    return sum;
}

void fill_tx_buf(void *buf, int len)
{
    uint8_t data;
    uint8_t *ptr;
    int i;

    ptr = (uint8_t *)buf;
    for (i = 0; i < len; i++) {
        data = (uint8_t)(rand() & 0xff);
        *(ptr + i) = data;
    }
}

// Driver code
int main(int argc, char *argv[])
{
    char tx_buffer[UDP_MAX_DATA_SIZE];
    char rx_buffer[UDP_MAX_DATA_SIZE];
    int n, i, msg_len, rc;
    int sockfd;
    socklen_t sa_len;

    set_socket_family();

    // Creating socket file descriptor
    if ((sockfd = socket(family, SOCK_DGRAM, 0)) < 0 ) {
        LOGE("socket creation failed");
        return -1;
    } else {
        LOGI(" Socket creation passed: fd: %d\n", sockfd);
    }

    msg_len = 1;
    i = 0;
    while (msg_len < UDP_MAX_DATA_SIZE) {
        union {
            struct sockaddr sa;
            struct sockaddr_vm svm;
        } addr = {
            .svm = {
                .svm_family = AF_VSOCK,
                .svm_port = PORT,
                .svm_cid = VMADDR_CID_ANY,
            },
        };
        int upper;
        int lower;
        int tx_csum, rx_csum;

        upper = msg_len * 2;
        lower = msg_len;
        msg_len = (rand() % (upper - lower + 1)) + lower;
        if (msg_len >= UDP_MAX_DATA_SIZE)
            msg_len = UDP_MAX_DATA_SIZE;

        fill_tx_buf(tx_buffer, msg_len);
        rc = sendto(sockfd, (const char *)tx_buffer, msg_len, 0, &(addr.sa), sizeof(addr));
        LOGI("%d: message sent %d bytes rc:%d errno:%d.\n", i, msg_len, rc, errno);
        if (rc < 0)
            break;

        n = recvfrom(sockfd, (char *)rx_buffer, UDP_MAX_DATA_SIZE, 0, &(addr.sa), &sa_len);
        LOGI("%d: message recv %d bytes rc:%d errno:%d.\n", i, n, rc, errno);

        tx_csum = get_checksum(tx_buffer, msg_len);
        rx_csum = get_checksum(rx_buffer, n);

        if (tx_csum != rx_csum)
            LOGI("Checksum does not match! tx:%#x rx:%#x\n", tx_csum, rx_csum);

        i++;
    }

    close(sockfd);
    return 0;
}
