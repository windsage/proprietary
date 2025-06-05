/******************************************************************************
  @file    perflock_native_test_client.cpp
  @brief   mpctl test which tests perflocks, hints, profiles

  ---------------------------------------------------------------------------
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-NTS-C"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5555

#include "PerfLog.h"

#define RECV_BUF_LEN 10
void show_usage() {
    printf("usage: perflock_native_test_client <options> <option-args>\n"
           "\tperflock_native_test_client -p <PORT> <options> <option-args>\n"
           "\tperflock_native_test_client --port <PORT> <options> <option-args>\n"
           "\t--acq <handle> <duration> <resource, value, resource, value,...>\n"
           "\t--acqrel <handle> <duration> <resource,value,resource,value,...> <numArgs> <reservedArgs>\n"
           "\t--hint <hint_id> [<duration>] [<pkg-name>] [<hint-type>]\n"
           "\t--profile <handle> <profile_id> [<duration>]\n"
           "\t--rel <handle>\n"
           "\t--hintacqrel <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"
           "\t--hintrenew <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"
           "\t--fbex req [<pkg-name>] <numArgs> [<reserved_list>]\n"
           "\t--evt event [<pkg-name>] <numArgs> [<reserved_list>]\n"

           "\t--rel_offload <handle>\n"
           "\t--hint_offload <hint_id> [<duration>] [<pkg-name>] [<hint-type>]\n"
           "\t--evt_offload event [<pkg-name>] <numArgs> [<reserved_list>]\n"
           "\t--hintacqrel_offload <handle> <hint_id> [<duration>] [<pkg-name>] [<hint-type>] <numArgs> [<reserved_list>]\n"

           "\tperflock acq, rel examles\n"
           "\tperflock_native_test_client --acq 0 5000 0x40C10000,0x1f\n"
           "\tperflock_native_test_client --acqrel 12 5000 \"0x40C10000,0x1f\" 2 0\n"
           "\tperflock_native_test_client --acqrel 12 5000 \"0x40C10000,0x1f,0x40C20000,0x1e\" 4 0\n"
           "\tperflock_native_test_client --rel 5"
           "\tperflock hint examles\n"
           "\tperflock_native_test_client --hint 0x00001203\n"
           "\tperflock_native_test_client --hint 0x00001203 20\n"
           "\tperflock_native_test_client --hint 0x00001203 10 \"helloApp\"\n"

           "\tperflock_native_test_client --rel_offload 5\n"
           "\tperflock_native_test_client --evt_offload 0x00001203 \"helloApp\" 2  0 1\n"
           "\tperflock_native_test_client --hint_offload 0x00001203\n"
           "\tperflock_native_test_client --hint_offload 0x00001203 20\n"
           "\tperflock_native_test_client --hint_offload 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test_client --hintacqrel_offload 23 0x00001203\n"
           "\tperflock_native_test_client --hintacqrel_offload 23 0x00001203 20\n"
           "\tperflock_native_test_client --hintacqrel_offload 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test_client --hintacqrel_offload 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"

           "\tperflock_native_test_client --hintacqrel 23 0x00001203\n"
           "\tperflock_native_test_client --hintacqrel 23 0x00001203 20\n"
           "\tperflock_native_test_client --hintacqrel 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test_client --hintacqrel 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"
           "\tperflock_native_test_client --hintrenew 23 0x00001203\n"
           "\tperflock_native_test_client --hintrenew 23 0x00001203 20\n"
           "\tperflock_native_test_client --hintrenew 23 0x00001203 10 \"helloApp\"\n"
           "\tperflock_native_test_client --hintrenew 23 0x00001203 10 \"helloApp\" 3 1 2 3\n"
           "\tperflock profile acq, rel examples"
           "\tperflock_native_test_client --profile 0 2\n"
           "\tperflock_native_test_client --profile 3 -1\n"
           "\tperflock_native_test_client --profile 0 5 10000\n");
}

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    std::string client_msg = "";
    std::string option = "";
    int start_index = 1;
    char server_ip[] = "127.0.0.1";
    int port = PORT;
    if(argc < 3 || (argc > 1 && strcmp(argv[1],"--help") == 0) || (argc > 1 && strcmp(argv[1],"-h") == 0)) {
        show_usage();
        return 0;
    }
    option = argv[start_index];

    if (option == "-p" || option == "--port") {
        start_index++;
        port = atoi(argv[start_index]);
        start_index++;
    }

    QLOGE(LOG_TAG, "Connecting on %s:%d", server_ip, port);
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported %s:%d\n", server_ip, port);
        return -1;
    }

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        QLOGE(LOG_TAG, "Connecting Failed %s:%d", server_ip, port);
        printf("\nConnection Failed %s:%d\n", server_ip, port);
        return -1;
    }
    for (int i = start_index ;i < argc; i++) {
        client_msg += std::string(argv[i]);
        if (i < argc - 1) {
            client_msg += " ";
        }
    }
    int readbytes = 0;
    char recvBuff[RECV_BUF_LEN];
    memset(recvBuff, '0',sizeof(recvBuff));

    QLOGE(LOG_TAG, "Submitting %s to Pseudo Client", client_msg.c_str());
    send(sock, client_msg.c_str(), client_msg.length(), 0);
    readbytes=read(sock, recvBuff, sizeof(recvBuff));
    if(readbytes < 0 || readbytes >= RECV_BUF_LEN)
    {
        printf("\nRead error \n");
    } else {
        recvBuff[readbytes] = '\0';
        QLOGE(LOG_TAG, "Handle returned: %s", recvBuff);
        printf("\nHandle: %s\n",recvBuff);
    }
    return 0;
}
