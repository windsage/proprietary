/*
Copyright (c) 2013 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

/* socket.c - socket interface */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


//Linux
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>

#define WORD  unsigned short
#define WSACleanup()  {}
#define INVALID_SOCKET    -1
#define Sleep sleep

#include <stddef.h>
#include "Socket.h"

static struct _Socket *_ClientSocket[MCLIENT];    // these are the client sockets
extern struct _Socket *_ListenSocket;

char terminationChar = '\n';

static int _CommandNext=0;        // index of next command to perform
static int _CommandRead=0;        // index of slot for next command read from socket

static char *_Command[MCOMMAND];
static char _CommandClient[MCOMMAND];
int current_client = -1;

extern int processDiagPacket(int client, unsigned char *inputMsg, unsigned int cmdLen);
extern int keep_tool_alive;
void SocketWriteEnableMode( int writeEnable )
{
  // Linux do nothing
}

#define ioctlsocket ioctl

/**************************************************************************
* osSockRead - read len bytes into *buf from the socket in pSockInfo
*
* This routine calls recv for socket reading
*
* RETURNS: length read
*/

PARSEDLLSPEC int SocketRead(struct _Socket *pSockInfo, unsigned char *buf, int len)
{
    unsigned long noblock;
    int nread;
    int ncopy;
    int error;
    int il;

    ncopy=0;

    noblock=1;
    error=ioctlsocket(pSockInfo->sockfd,FIONBIO,&noblock);
    if(error!=0)
    {
        return -1;
    }
    //
    // try to get some more data
    //
    nread=MSOCKETBUFFER-pSockInfo->nbuffer;
        nread=recv(pSockInfo->sockfd, &(pSockInfo->buffer[pSockInfo->nbuffer]), nread, 0);

    if(nread<=0)
    {
                if (nread == -1)
        {
            nread=0;
        }
        else
        {
            return -1;
        }
    }
    pSockInfo->nbuffer+=nread;
    //
    // check to see if we have enough stored data to return a message
    //
    for(il=0; il<pSockInfo->nbuffer; il++)
    {
        if ( pSockInfo->buffer[il] == terminationChar )
        {
            il++;
            //
            // copy data to user buffer
            //
            if(len<il)
            {
                ncopy=len-1;
            }
            else
            {
                ncopy=il;
            }
            memcpy(buf,pSockInfo->buffer,ncopy);
            buf[ncopy]=0;
            //
            // compress the internal data
            //
            memcpy(pSockInfo->buffer,&pSockInfo->buffer[il],pSockInfo->nbuffer-il);
            pSockInfo->nbuffer -= il;

            return ncopy;
        }
    }
    // nart doesn't send terminationChar
    if (pSockInfo->nbuffer > 0 && il==pSockInfo->nbuffer) {
        ncopy = pSockInfo->nbuffer;
        memcpy(buf,pSockInfo->buffer,ncopy);
        buf[ncopy]=0;
    }
    //
    // no message, but buffer is full. This is a problem.
    // delete allof the existing data.
    //
    if(il>=MSOCKETBUFFER)
    {
        pSockInfo->nbuffer=0;
    }

    return ncopy;
}

/**************************************************************************
* osSockWrite - write len bytes into the socket, pSockInfo, from *buf
*
* This routine calls a OS specific routine for socket writing
*
* RETURNS: length read
*/
PARSEDLLSPEC int SocketWrite(struct _Socket *pSockInfo, unsigned char *buf, int len)
{
    int    dwWritten;
    int bytes,cnt;
    unsigned char* bufpos;
    int tmp_len;
    int error;

    error=0;

    tmp_len = len;
    bufpos = buf;
    dwWritten = 0;

    while (len)
    {
        if (len < SEND_BUF_SIZE) bytes = len;
        else bytes = SEND_BUF_SIZE;

        cnt = send(pSockInfo->sockfd, (char *)bufpos, bytes, 0);
        if(cnt<=0)
        {

                    if (cnt == -1)
            {
                        error++;
                if(error>100000)
                {
                    break;
                }
                cnt=0;
                    Sleep(10);
                    continue;
            }
            else
            {
                return -1;
            }
        }
        error=0;
        dwWritten += cnt;
        len  -= cnt;
        bufpos += cnt;
    }

    len = tmp_len;

    if (dwWritten != len) {
        dwWritten = 0;
    }

    return dwWritten;
}

/**************************************************************************
* osSockClose - close socket
*
* Close the handle to the pipe
*
* RETURNS: 0 if error, non 0 if no error
*/
void SocketClose(struct _Socket* pOSSock)
{
    close(pOSSock->sockfd);
    free(pOSSock);

    return;
}

#define CONNECT_TIMEOUT 20 /* secs cart will wait for a nart connect */

static int socketConnect(char *target_hostname, unsigned int target_port_num, unsigned int *ip_addr)
{
    int       sfd;
    struct protoent *    proto;
    int       res;
    struct sockaddr_in    sin;
    int       i;
    struct hostent *hostent;

    sfd = socket(PF_INET, SOCK_STREAM, 0);

    /* Allow immediate reuse of port */
    i = 1;
    res = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *) &i, sizeof(i));
    if (res != 0) {
        WSACleanup( );
        return -1;
    }

// Linux disabled Nagle by default

    hostent = gethostbyname(target_hostname);
    if (!hostent) {
        WSACleanup( );
        return -1;
    }

    memcpy(ip_addr, hostent->h_addr_list[0], hostent->h_length);
    *ip_addr = ntohl(*ip_addr);

    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr.s_addr, hostent->h_addr_list[0], hostent->h_length);
    sin.sin_port = htons((short)target_port_num);

    res = connect(sfd, (struct sockaddr *) &sin, sizeof(sin));
    if (res!=0)
    {
        return -1;
    }

    return sfd;
}


PARSEDLLSPEC struct _Socket *SocketConnect(char *pname, unsigned int port)
{
    char        pname_lcl[256];
    char *        mach_name;
    char *        cp;
    struct _Socket *pOSSock;
    int            res;

    strlcpy(pname_lcl, pname, sizeof(pname_lcl));
    pname_lcl[sizeof(pname_lcl) - 1] = '\0';
    mach_name = pname_lcl;
    while (*mach_name == '\\') {
        mach_name++;
    }
    for (cp = mach_name; (*cp != '\0') && (*cp != '\\'); cp++) {
    }
    *cp = '\0';

    if (!strcmp(mach_name, ".")) {
        /* A windows convention meaning "local machine" */
        mach_name = "localhost";
    }

    pOSSock = (struct _Socket *) malloc(sizeof(struct _Socket));
    if(!pOSSock) {
        return NULL;
    }

    strlcpy(pOSSock->hostname, mach_name, sizeof(pOSSock->hostname));
    pOSSock->hostname[sizeof(pOSSock->hostname) - 1] = '\0';

    pOSSock->port_num = port;

    res = socketConnect(pOSSock->hostname, pOSSock->port_num,
                          &pOSSock->ip_addr);;
    if (res < 0) {
      free(pOSSock);
      return NULL;
    }

    pOSSock->sockfd = res;
    pOSSock->nbuffer=0;

    return pOSSock;
}


/* osSockAccept - Wait for a connection
*
*/
PARSEDLLSPEC struct _Socket *SocketAccept(struct _Socket *pOSSock, unsigned long noblock)
{
    struct _Socket *pOSNewSock;
    int        i;
    int        sfd;
    struct sockaddr_in    sin;
    int error;

    error=ioctlsocket(pOSSock->sockfd,FIONBIO,&noblock);
    if(error!=0)
    {
    }

    i = sizeof(sin);
    sfd = accept(pOSSock->sockfd, (struct sockaddr *) &sin, (int *)&i);

    if (sfd < 0)
      return 0;

    pOSNewSock = (struct _Socket *) malloc(sizeof(*pOSNewSock));
    if (!pOSNewSock)
    {
        return NULL;
    }

    strlcpy(pOSNewSock->hostname, inet_ntoa(sin.sin_addr), sizeof(pOSNewSock->hostname));
    pOSNewSock->port_num = pOSSock->port_num;

    pOSNewSock->sockClose = 0;
    pOSNewSock->sockDisconnect = 0;
    pOSNewSock->sockfd = sfd;

    pOSNewSock->nbuffer=0;

    return pOSNewSock;
}



static int socketListen(struct _Socket *pOSSock)
{
    int       sockfd;
    struct protoent *    proto;
    int       res;
    struct sockaddr_in sin;
    int       i;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        WSACleanup( );
        return -1;
    }

    i = 1;

// Linux disabled Nagle by default

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr =  INADDR_ANY;
    sin.sin_port = htons((unsigned short)pOSSock->port_num);

    res = bind(sockfd, (struct sockaddr *) &sin, sizeof(sin));
    if (res != 0) {
        WSACleanup( );
        return -1;
    }

    res = listen(sockfd, 4);
    if (res != 0) {
        WSACleanup( );
        return -1;
    }

    return sockfd;
}



PARSEDLLSPEC struct _Socket *SocketListen(unsigned int port)
{
    struct _Socket *pOSSock;

    pOSSock = (struct _Socket *) malloc(sizeof(struct _Socket));
    if(!pOSSock) {
        return NULL;
    }

    pOSSock->port_num = port;

    pOSSock->sockDisconnect = 0;
    pOSSock->sockClose = 0;

    pOSSock->sockfd = socketListen(pOSSock);
    if(pOSSock->sockfd < 0) {
            free(pOSSock);
        return NULL;
    }

    pOSSock->nbuffer=0;

    return pOSSock;
}

PARSEDLLSPEC void SetStrTerminationChar( char tc )
{
    terminationChar = tc;
}

PARSEDLLSPEC char GetStrterminationChar( void )
{
    return terminationChar;
}

static int ClientAccept()
{
    int it;
    int noblock;
    struct _Socket *TryClientSocket;
    static int OldestClient = 0;

    if(_ListenSocket!=0)
    {
        //
        // If we have no clients, we will block waiting for a client.
        // Otherwise, just check and go on.
        //
        noblock=0;
        for(it=0; it<MCLIENT; it++)
        {
            if(_ClientSocket[it]!=0)
            {
                noblock=1;
                break;
            }
        }
        if(noblock==0)
        {
            printf("\nReady for Client(QDart) Connection!\n" );
        }
        //
        // Look for new client
        //
        for(it=0; it<MCLIENT; it++)
        {
            if(_ClientSocket[it]==0)
            {
                _ClientSocket[it]=SocketAccept(_ListenSocket,noblock);// don't block
                if(_ClientSocket[it]!=0)
                {
                    printf("Client connection is established at [%d]!\n",it);
                    return it;
                }
            }
        }
        // In case all clients have been used up, but there is another client want to connect, give away the oldest one
        if (it == MCLIENT)
        {
            TryClientSocket = SocketAccept(_ListenSocket,noblock);
            if (TryClientSocket)
            {
                SocketClose(_ClientSocket[OldestClient]);
                _ClientSocket[OldestClient] = TryClientSocket;
                return it;
            }
        }
    }
    return -1;
}

static void ClientClose(int client)
{
    if(client>=0 && client<MCLIENT && _ClientSocket[client]!=0)
    {
        SocketClose(_ClientSocket[client]);
        _ClientSocket[client]=0;
    }
}


int SendItDiag(int client, unsigned char *buffer, int length)
{
    int nwrite;
    printf("%s : writing rsp", __func__);
    if(_ListenSocket==0|| (client>=0 && client<MCLIENT && _ClientSocket[client]!=0))
    {
        if ( _ListenSocket == 0 )
        {
            //printf("%s",response);
        }
        else
        {
            SocketWriteEnableMode( 1 );
            nwrite=SocketWrite(_ClientSocket[client],buffer,length);
            SocketWriteEnableMode( 0 );
            for (int i = 0; i < length; i++)
            {
                printf(" %x ",  buffer[i]);
            }
            printf("\n");
            if(nwrite<0)
            {
                printf( "Error - Call to SocketWrite() failed.\n" );
                ClientClose(client);
                return -1;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }

}
#ifdef QCMBR_UART_QDART
int SendItDiag_Uart(int client, char *buffer, int length)
{
    int nwrite;
#ifdef UART_DEBUG
    {
        // DEBUG USE
    }
#endif
    nwrite = UartWrite(&uart_port, buffer, length);

    if(nwrite < 0) {
        printf( "Error - UartWrite failed.\n" );
        return -1;
    }
    return 0;
}
#endif

void Start_Server(int port)
{
    int it;

    SetStrTerminationChar( DIAG_TERM_CHAR );
    SocketWriteEnableMode( 0 );

    _ListenSocket=SocketListen(port);
    if(_ListenSocket==0)
    {
        printf( "Can't open control process listen port %d\n", port );
        exit(-1);
     }

     for ( it=0; it < MCLIENT; it++ )
         _ClientSocket[it] = 0;

     ClientAccept();
}

void *StartReadThread(void *temp)
{
    unsigned int nread;
    unsigned char buffer[MBUFFER];
    int client;
    while(1)
    {
        printf("%s: Waiting for diag packet", __func__);
        nread = CommandNext(buffer,MBUFFER-1,&client);
        if(nread < 0) {
           printf("%s: Recevied zero bytes", __func__);
        }
    }
}

void DispHexString(unsigned char *pkt_buffer, int recvsize)
{
  int i, j, k;

  //if (verbose == 1)
//  {
      for (i=0; i<recvsize; i+=MAX_HS_WIDTH) {
        printf("[%4.4d] ",i);
        for (j=i, k=0; (k<MAX_HS_WIDTH) && ((j+k)<recvsize); k++)
          printf("0x%2.2X ",(pkt_buffer[j+k])&0xFF);
        for (; (k<MAX_HS_WIDTH ); k++)
          printf("     ");
        printf(" ");
        for (j=i, k=0; (k<MAX_HS_WIDTH) && ((j+k)<recvsize); k++)
          printf("%c",(pkt_buffer[j+k]>32)?pkt_buffer[j+k]:'.');
        printf("\n");
      }
  //}
}

int CommandRead()
{
    unsigned char buffer[MBUFFER];
    int nread;
    //int ntotal;
    int it;
    int diagPacketReceived = 0;
    unsigned int cmdLen = 0;

    //
    // look for new clients
    //
    ClientAccept();
    //
    // try to read everything on the client socket
    //
    //ntotal=0;
    while(_CommandNext!=_CommandRead || _Command[_CommandRead]==0)
    {
        if(_ListenSocket==0)
        {
        }
        else
        {
            //
            // read commands from each client in turn
            //
            for(it=0; it<MCLIENT; it++)
            {
                if(_ClientSocket[it]!=0)
                {
                    nread = (int)SocketRead(_ClientSocket[it],buffer,MBUFFER-1);
                    if(nread>0)
                    {
                        printf( "SocketRead() return %d bytes\n", nread );
                        buffer[nread]=0;
                        DispHexString(buffer,nread);

                        if(nread>1 && buffer[nread-1]==DIAG_TERM_CHAR)
                        {
                            diagPacketReceived = 1;
                            buffer[nread-1]=0;
                            cmdLen = nread-0;
                         }
                        //Needed for linux path
                        if(nread>2 && buffer[nread-2]==DIAG_TERM_CHAR)
                        {
                            diagPacketReceived = 1;
                            buffer[nread-2]=0;
                            cmdLen = nread - 1;
                        }
                        //
                        // check to see if we received a diag packet
                        //
                        if(diagPacketReceived) {
                            current_client = it;
                            printf("%s : current_client = %d\n", __func__, current_client);
                            if(processDiagPacket(it,(unsigned char *)buffer,cmdLen)) {
                                printf( "\n--processDiagPacket-succeed------ Wait For Next Diag Packet ----------------\n\n" );
                                continue;
                            }
                            else
                            {
                                printf( "\n--processDiagPacket-failed------- Wait For Next Diag Packet ----------------\n\n" );
                            }
                        }
                    }
                    else if(nread<0)
                    {
                        printf("Closing connection <-- Remote connection closed.");
                        ClientClose(it);
                        if (keep_tool_alive) {
                          printf("waiting here press CTRL+C to terminate\n");
                          while(1);
                        }
                        exit(1);
                    }
                }
            }
        }
    }
    return 0; //ntotal;
}

int CommandNext(unsigned char *command, int max, int *client)
{
    int length;

    if(CommandRead() < 0)
        return -1;

    if(_Command[_CommandNext]!=0)
    {
        length = sizeof( _Command[_CommandNext]); //length=Slength(_Command[_CommandNext]);
        if(length>max)
    {
            _Command[_CommandNext][max]=0;
             length=max;
    }

        if ( command != NULL )
    {
           if ( _Command[_CommandNext] == NULL )
       {
               strlcpy( (char*) command,_Command[_CommandNext],
                      ( sizeof( command ) > sizeof( _Command[_CommandNext] ) ) ?
                        sizeof( _Command[_CommandNext] ) : ( sizeof( command ) ) );
       }
    }

    *client=_CommandClient[_CommandNext];
    if ( _Command[_CommandNext] != NULL ) {
        free( _Command[_CommandNext] );
    }
    _Command[_CommandNext]=0;
    _CommandNext=(_CommandNext+1)%MCOMMAND;

    return length;
    }
    return 0;
}
