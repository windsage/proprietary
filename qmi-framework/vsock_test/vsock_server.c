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
    LOGI("fd:%d errno:%d\n", fd, errno);
    if (fd < 0 && errno == EAFNOSUPPORT) {
        family = AF_VSOCK;
    } else {
        family = AF_QMSGQ;
        close(fd);
    }
}

int main() {
    int serv_fd, client_fd;
    char buffer[UDP_MAX_DATA_SIZE];
    char *hello = "Hello from server";
    socklen_t len;
    int n, i, j;
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
    union {
        struct sockaddr sa;
        struct sockaddr_vm svm;
    } clientaddr;

    set_socket_family();

    // Creating socket file descriptor
    if ((serv_fd = socket(family, SOCK_DGRAM, 0)) < 0) {
        LOGE("socket creation failed\n");
        return -1;
    } else {
        LOGI("Socket creation passed: fd: %d\n", serv_fd);
    }

    // Bind the socket with the server address
    if (bind(serv_fd, &(addr.sa), sizeof(addr.svm)) < 0)
    {
        LOGE("bind failed %d port:%d\n", errno, addr.svm.svm_port);
        return -1;
    }

    i = 0;
    while(1) {
        len = sizeof(clientaddr.svm);
        n = recvfrom(serv_fd, (char *)buffer, UDP_MAX_DATA_SIZE,
                    0, &(clientaddr.sa), &len);

        LOGI("%d: server received %d bytes\n", i, n);
        sendto(serv_fd, buffer, n, 0, &(clientaddr.sa), len);
        LOGI("%d: message sent.\n", i);
        i++;
    }

    close(serv_fd);
    return 0;
}
