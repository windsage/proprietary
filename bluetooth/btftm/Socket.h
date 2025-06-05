/*
Copyright (c) 2013 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

/* socket.h - header file for socket.c */

#ifndef INC_SOCKET_H
#define INC_SOCKET_H

#ifdef _USE_SOCKET_FUNCTIONS_IN_EXE_
#define PARSEDLLSPEC
#else
    #define PARSEDLLSPEC
#endif


#define SEND_BUF_SIZE 1024
#define DIAG_TERM_CHAR 0x7E
#define MCLIENT 4
#define MCOMMAND 50
#define MAX_HS_WIDTH    16


typedef char                    A_CHAR;
typedef unsigned char           A_UCHAR;
typedef A_CHAR                  A_INT8;
typedef A_UCHAR                 A_UINT8;
typedef short                   A_INT16;
typedef unsigned short          A_UINT16;
typedef long int                A_INT32;
typedef unsigned long int       A_UINT32;
typedef unsigned long int       A_UINT;

typedef unsigned char           u8;
typedef unsigned short          u16;

typedef A_UCHAR                 A_BOOL;
typedef void *                  OS_DEVICE_ID;

typedef int64_t                 A_INT64;
typedef uint64_t                A_UINT64;

typedef long                    A_INT_PTR;
typedef unsigned long           A_UINT_PTR;

#define MSOCKETBUFFER 4096
#define MBUFFER 2048*2
#define MAX_RESP_DATA_SIZE 2048*2

#ifdef LINUX_X86
#define PARSEDLLSPEC
#define closesocket     close
#endif

struct _Socket
{
    char hostname[128];
    unsigned int port_num;
    unsigned int ip_addr;
    int inHandle;
    int outHandle;
    int  sockfd;
    unsigned int sockDisconnect;
    unsigned int sockClose;
    int nbuffer;
    char buffer[MSOCKETBUFFER];
};

int SocketRead(struct _Socket *pSockInfo, unsigned char *buf, int len);
int SocketWrite(struct _Socket *pSockInfo, unsigned char *buf, int len);
void SocketClose(struct _Socket *pSockInfo);
struct _Socket *SocketConnect(char *pname, unsigned int port);
struct _Socket *SocketAccept(struct _Socket *pSockInfo, unsigned long noblock);
struct _Socket *SocketListen(unsigned int port);
void Start_Server(int port);
void *StartReadThread(void *temp);
extern int CommandNext(unsigned char *command, int max, int *client);
extern int SendItDiag(int client, unsigned char *buffer, int length);
extern int SendItDiag_Uart(int client, char *buffer, int length);

#endif

