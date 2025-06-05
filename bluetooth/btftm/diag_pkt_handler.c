
/*
Copyright (c) 2013 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

/* diag_pkt_handler.c - diagnostic packet handler */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "Socket.h"
#include "ftm_common.h"
#include "ftm_dbg.h"

#ifdef LINUX_X86
#define HANDLE  int32_t
#define PVOID   int
#endif

//#include "Qcmbr.h"

#ifdef QCMBR_UART_QDART
extern ConnectionType connectionTypeOptions;
#endif

#define ART2_IOCTL_TYPE_START     50000
#define MBUFFER 2048*2
#define ENDING_FLAG               0x7E
#define ESCAPE_CHARACTER          0x7D

static unsigned short Calc_CRC_16_l(unsigned char* pBufPtr, int bufLen);

extern A_BOOL artSendCmd2( A_UINT8 *pCmdStruct, A_UINT32 cmdSize, unsigned char* responseCharBuf, unsigned int *responseSize );
extern void * bt_ftm_diag_dispatch(void *req_pkt, uint16 pkt_len);

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

// ART2 WMI test command
#define ART2_DEV_IOCTL_REQUEST \
    CTL_CODE( ART2_IOCTL_TYPE_START, 0x901, METHOD_OUT_DIRECT, FILE_ANY_ACCESS )

#define AHDLC_FLAG    0x7e      // Flag byte in Async HDLC
#define AHDLC_ESCAPE  0x7d      // Escape byte in Async HDLC
#define AHDLC_ESC_M   0x20      // Escape mask in Async HDLC

#define DIAG_PKT_LEN    12

unsigned char diagPktHeader[DIAG_PKT_LEN+1];

enum CmdCode
{
    version = 0x0,
    esn = 0x1,
    status = 0xC,
    nvItemRead = 0x26,
    phoneState = 0x3F,
    subSystemDispatch = 0x4B,
    featureQuery = 0x51,
    embeddedFileOperation = 0x59,
    diagProtLoopback = 0x7B,
    extendedBuildID = 0x7C
};

enum SubsystemID
{
    FTMSubsystem = 11
};

enum SubsystemCmdCode
{
    WLANCmdCode = 22,
    WLANNARTCmdCode = 0x8003,
};

// List of responses
unsigned char verResponseMsg[] = {
                        version,                        //CMD_CODE (0)
                        0x4F,0x63,0x74,0x20,0x30,0x38,
                        0x20,0x32,0x30,0x31,0x31,       //COMP_DATE (Compilation date)
                        0x31,0x30,0x3A,0x30,0x37,0x3A,
                        0x30,0x30,                      //COMP_TIME
                        0x4F,0x63,0x74,0x20,0x30,0x38,
                        0x20,0x32,0x30,0x31,0x31,       //REL_DATE (Compilation date)
                        0x31,0x30,0x3A,0x30,0x37,0x3A,
                        0x30,0x30,                      //REL_TIME
                        0x41,0x41,0x41,0x31,0x30,
                        0x30,0x30,0x30,                 //VER_DIR
                        0,                              //SCM
                        1,                              //MOB_CAL_REV
                        255,                            //MOB_MODEL
                        1,0,                            //MOB_FIRM_REV
                        0,                              //SLOT_CYCLE_INDEX
                        1, 0                            //MSM_VER
};

unsigned char esnResponseMsg[] = {
                        esn,                            //CMD_CODE (1)
                        0xEF,0xBE,0xAD,0xDE             //ESN
};
unsigned char statusResponseMsg[] = {
                            status,                 //CMD_CODE (12) 1
                            0, 0, 0,                //RESERVED 3
                            0xce, 0xfa, 0xbe, 0xba, //ESN 4
                            6, 0,                   //RF_MODE 2
                            0, 0, 0, 0,             //MIN1 (Analog) 4
                            0, 0, 0, 0,             //MIN1 (CDMA) 4
                            0, 0,                   //MIN2 (Analog) 2
                            0, 0,                   //MIN2 (CDMA) 2
                            0,                      //RESERVED 1
                            0, 0,                   //CDMA_RX_STATE 2
                            0xff,                   //CDMA_GOOD_FRAMES 1
                            0, 0,                   //ANALOG_CORRECTED_FRAMES 2
                            0, 0,                   //ANALOG_BAD_FRAMES 2
                            0, 0,                   //ANALOG_WORD_SYNCS 2
                            0, 0,                   //ENTRY_REASON 2
                            0, 0,                   //CURRENT_CHAN 2
                            0,                      //CDMA_CODE_CHAN 1
                            0, 0,                   //PILOT_BASE 2
                            0, 0,                   //SID 2
                            0, 0,                   //NID 2
                            0, 0,                   //LOCAID 2
                            0, 0,                   //RSSI 2
                            0                       //POWER  1

};
unsigned char nvItemReadResponseMsg[] = {
                                nvItemRead,             // CMD_CODE (0x26)
                                71, 0,                  //NV_ITEM
                                0x4E,0x41,0x52,0x54,    // ITEM_DATA
                                0x5F,0x41,0x50,0x2F,
                                0x4D,0x6F,0x62,0x0,
                                0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0                     // STATUS
};
unsigned char phoneStateResponseMsg[] = {
                        phoneState,                     //CMD_CODE (0x3F)
                        0x1,                            //PHONE_STATE
                        0x0,0x0                         //EVENT_COUNT
};

unsigned char featureQueryRespMsg[] = {
                              featureQuery,
                              0,0,
                              0
};
unsigned char extendedBuildIDRespMsg[] = {
                                extendedBuildID,        // CMD_CODE (0x7C)
                                2,                      // Version
                                0,0,                    // Reserved
                                0xE8,0x3,0,0,           // MSM Revision (assigned to 1000)
                                1,0x40,0,0,             // Manufacturer model number (assigned to 4001)
                                0x31,0,                 // Softwareversion (ascii value of 1)
                                0x4E,0x41,0x52,0x54,0x5F,
                                0x41,0x50,0x2F,0x4D,0x6F,
                                0x62,0x0                // Model string (NART_AP/Mob)
};

int removeDiagHeader(unsigned char *inputMsg, int cmdLen) {
    int ii, jj;
    int startPktContent = DIAG_PKT_LEN + 4;
    int ESCAPEcnt=0;

    // record the diagPkt header before remove, will be added in TLV response packet to send back to QRCK
    for (ii = 0; ii < DIAG_PKT_LEN; ii++) {
        diagPktHeader[ii] = inputMsg[ii];
    }

    //diag packet contents are the first 12 bytes with 2 byte crc and end character
    //copy bytes from beyond the header into the beginning of the buffer
    //first look to see if there are any esc characters in the header, which will make it longer
    for (ii = 0; ii < startPktContent; ii++) {
        if(inputMsg[ii] == AHDLC_ESCAPE) {
            startPktContent ++;
            ESCAPEcnt++;
        }
    }

    //strip out any esc characters as go
    for (ii = 0, jj=startPktContent; jj < cmdLen; ii++, jj++) {
        //look to see if there are any esc characters we need to remove
        if(inputMsg[jj] == AHDLC_ESCAPE) {
            //skip this character
            jj++;
            //next character needs xor
            inputMsg[ii] = inputMsg[jj] ^ AHDLC_ESC_M;
            ESCAPEcnt++;
        }
        else {
            inputMsg[ii] = inputMsg[jj];
        }
    }
    //Move the null to before the crc and the existing null
    inputMsg[ii-3] = 0;
    return ESCAPEcnt;
}

int addDiagHeader( unsigned char *inputMsg, int cmdLen, unsigned char *outputMsg )
{
    // return the length of the outputMsg
    unsigned char tempBuf[MBUFFER];
    unsigned int crc;
    int i,j;

    //memcpy( tempBuf, diagPktHeader, DIAG_PKT_LEN );
    for ( i = 0; i < DIAG_PKT_LEN; i++ )
    {
        tempBuf[i] = diagPktHeader[i];
    }

    tempBuf[6] = tempBuf[7] = 0;    // field 6, 7 are FTM_REQ_DATA_LEN
    tempBuf[8] = (unsigned char) ( (cmdLen+DIAG_PKT_LEN)  & 0x0FF );
    tempBuf[9] = (unsigned char) ( (cmdLen+DIAG_PKT_LEN)  >> 8 );    // field 8, 9 are FTM_RESPONSE_DATA_LEN

    //memcpy( &tempBuf[DIAG_PKT_LEN], inputMsg, cmdLen );
    for ( i = 0; i < cmdLen; i++ )
    {
        tempBuf[DIAG_PKT_LEN+i] = inputMsg[i];
    }

    crc = Calc_CRC_16_l( tempBuf, (DIAG_PKT_LEN+cmdLen) );
    tempBuf[DIAG_PKT_LEN+cmdLen+0] = (unsigned char) ( crc & 0x0FF );
    tempBuf[DIAG_PKT_LEN+cmdLen+1] = (unsigned char) ( crc >> 8 );

    j = 0;
    for ( i = 0; i < (DIAG_PKT_LEN+cmdLen+2); i++ )
    {
        if( ( tempBuf[i] == AHDLC_ESCAPE ) || ( tempBuf[i] == AHDLC_FLAG ) )
        {
            // add esc character in front of this char and increase pktLen by 1
            outputMsg[j++] = AHDLC_ESCAPE;
            outputMsg[j++] = tempBuf[i] ^ AHDLC_ESC_M;
        }
        else
        {
            outputMsg[j++] = tempBuf[i];
        }
    }
    outputMsg[j++] = DIAG_TERM_CHAR;

    printf( "DIAG Sending [[ \n" );
    for ( i = 0; i < j; i++ )
    {
        printf( "%x ", outputMsg[i] );
    }
    printf( "\n ]]\n" );

    return j;
}

static unsigned short Calc_CRC_16_l(unsigned char* pBufPtr, int bufLen)
{
    /*
        The CRC table size is based on how many bits at a time we are going
        to process through the table.  Given that we are processing the data
        8 bits at a time, this gives us 2^8 (256) entries.
    */
    //const int c_iCRC_TAB_SIZE = 256;    // 2^CRC_TAB_BITS


    /* CRC table for 16 bit CRC, with generator polynomial 0x8408,
    ** calculated 8 bits at a time, LSB first.
    */
    unsigned short _aCRC_16_L_Table[] =
    {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
    };

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Mask for CRC-16 polynomial:
    **
    **      x^16 + x^12 + x^5 + 1
    **
    ** This is more commonly referred to as CCITT-16.
    ** Note:  the x^16 tap is left off, it's implicit.
    */
    const unsigned short c_iCRC_16_L_POLYNOMIAL = 0x8408;

    /* Seed value for CRC register.  The all ones seed is part of CCITT-16, as
    ** well as allows detection of an entire data stream of zeroes.
    */
    const unsigned short c_iCRC_16_L_SEED = 0xFFFF;


    unsigned short data, crc_16;

    /*
        Generate a CRC-16 by looking up the transformation in a table and
        XOR-ing it into the CRC, one byte at a time.
    */
    int i = 0;
    int iLen = bufLen * 8;

    for (crc_16 = c_iCRC_16_L_SEED; iLen >= 8; iLen -= 8, i++)
    {
//        temp1 = (crc_16 ^ pBufPtr[i]);
//        temp1 = temp1 & 0x00ff;
//        temp2 = (crc_16 >> 8) & 0xff;
//        crc_16 = (short)(_aCRC_16_L_Table[temp1] ^ temp2);
        crc_16 = (unsigned short)(_aCRC_16_L_Table[(crc_16 ^ pBufPtr[i]) & 0x00ff] ^ (crc_16 >> 8));
    }

    /*
        Finish calculating the CRC over the trailing data bits
        XOR the MS bit of data with the MS bit of the CRC.
        Shift the CRC and data left 1 bit.
        If the XOR result is 1, XOR the generating polynomial in with the CRC.
    */
    if (iLen != 0)
    {

        data = (unsigned short)(((unsigned short)(pBufPtr[i])) << (16 - 8));        // Align data MSB with CRC MSB

        while (iLen-- != 0)
        {
            if (((crc_16 ^ data) & 0x01) != 0)
            {                                        // Is LSB of XOR a 1

                crc_16 >>= 1;                        // Right shift CRC
                crc_16 ^= c_iCRC_16_L_POLYNOMIAL;    // XOR polynomial into CRC

            }
            else
            {

                crc_16 >>= 1;                        // Right shift CRC

            }    // if ( ((crc_16 ^ data) & 0x01) != 0 )

            data >>= 1;                                // Right shift data

        }    // while (iLen-- != 0)

    }    // if (iLen != 0) {

    return ((unsigned short)~crc_16);            // return the 1's complement of the CRC

}

void DiagPrint(int client, unsigned char *response, int length)
{
    unsigned char temp, fmtMsg[MBUFFER]; //overkill for now
    unsigned short crc;
    int count, i;

    if(length > (MBUFFER - 3)) {
        printf( "DiagPrint input message to large needs to be less than 997 [%d]", length );
        return;
    }

    /* On the QMSL side if the payload data have ESCAPE_CHARACTER it will send by appending 0x7D 0x5D.
     * similarly if the payload data have ENDING_FLAG it will send by appending 0x7D 0x5E
     * after Diag read from the socket below logic will remove those extra bytes and form an actual packet
     * (ie. if 0x7D 0x5D found replac it with 7D. if 0x7D 0x5E found replac it with 7E.)
    */
    for (count=0, i=0; count<length; count++, i++) {
        if (response[count] == ESCAPE_CHARACTER) {
            if(count == (length -1)) {
                count++;
                break;
            } else {
                fmtMsg[i] = response[count+1] ^ 0x20;
                count++;
            }
        } else {
            fmtMsg[i] = response[count];
        }
    }
    length = i;

    crc = Calc_CRC_16_l(response, length);

    /* To send back packet to QMSL, append the CRC to actual packet.
     * if CRC has ESCAPE_CHARACTER below logic will replace it with 0x7D 0x5D. simiarly if
     * if CRC has ENDING_FLAG below logic will replace it with 0x7D 0x5E and send to socket
    */
    for (i=0; i<sizeof(crc); i++) {
        temp = (char)(0xff & (crc >> (i*8)));
        if (temp == ESCAPE_CHARACTER || temp == ENDING_FLAG) {
            fmtMsg[length] = ESCAPE_CHARACTER;
            fmtMsg[length+1] = temp ^ 0x20;
            length += 2;
        } else {
            fmtMsg[length] = temp;
            length ++;
       }
    }

    fmtMsg[length] = DIAG_TERM_CHAR;
    printf("%s: Sending rsp back with len:%d\n", __func__, length);

#ifdef QCMBR_UART_QDART
    if (connectionTypeOptions == SERIAL)
        SendItDiag_Uart(client,fmtMsg,length+1 );
    else if (connectionTypeOptions == ETHERNET) {
        SendItDiag(client,fmtMsg,length +1);
    }
#else
    SendItDiag(client,fmtMsg,length+1);
#endif
}

void DiagRespPrint(int client, unsigned char *response, int length)
{
        unsigned char fmtMsg[MBUFFER]; //overkill for now
        int len=0;
   printf("%s : sending rsp\n", __func__);
    if (length > (MBUFFER - 3)) {
        printf("DiagRespPrint input message to large needs to be less than 997 [%d]", length);
        return;
        }

    // Note: Cannot reduce "length" since there is a checksum calculated by the driver.
    // Note: The only solution is to have the expected length modified in QDART
       len = addDiagHeader(response, length, fmtMsg);
#ifdef QCMBR_UART_QDART
    if (connectionTypeOptions == SERIAL)
        SendItDiag_Uart(client, fmtMsg, len);
    else if (connectionTypeOptions == ETHERNET)
        SendItDiag(client, fmtMsg, len);
#else
     printf("%s : sending rsp\n", __func__);
        SendItDiag(client, fmtMsg, len);
#endif
}

static int cmd_init_done = 0;
static int dev_handle_gotten = 0;
//static HANDLE DevHandle = (PVOID) NULL;

//#define MPWORD 3

//struct _ParameterList myParamList;

int processTLVPacket(unsigned char *tlvString, unsigned int length, unsigned char *respdata, unsigned int *nrespdata )
{
    if ( !artSendCmd2( tlvString, length, respdata, nrespdata ) )
    {
        printf( "DeviceDiagData() FAILED\n" );
        return -1;
    }
    else
    {
        printf( "artSendCmd2() returned OK rspLenRet[%d]\n",*nrespdata );
    }

    return 1;
}


static unsigned char respdata[MAX_RESP_DATA_SIZE];
static unsigned int nrespdata;
//
// returns 1 if the command is consumed
// returns 0 if diag packet contained a nart command that needs to be processed.
//
int processDiagPacket(int client, unsigned char *inputMsg, unsigned int cmdLen)
{
    unsigned int i;
    int myStatus;
    int ESCAPEcnt=0;

    switch (inputMsg[0]) {
    case version:
        printf( "echo for version \n" );
        DiagPrint(client, verResponseMsg, sizeof(verResponseMsg)/sizeof(unsigned char));
                return 1; //verResponseMsg;
            //break;
       case esn:
        printf( "echo for esn\n" );
        DiagPrint(client, esnResponseMsg, sizeof(esnResponseMsg)/sizeof(unsigned char));
                return 1; //esnResponseMsg;
            //break;
       case status:
        printf( "echo for status\n" );
            DiagPrint(client, statusResponseMsg, sizeof(statusResponseMsg)/sizeof(unsigned char));
            return 1; //statusResponseMsg;
            //break;
       case phoneState:
        printf( "echo for phoneState\n" );
            DiagPrint(client, phoneStateResponseMsg, sizeof(phoneStateResponseMsg)/sizeof(unsigned char));
            return 1; //phoneStateResponseMsg;
            //break;
       case nvItemRead:
        printf( "echo for nvItemRead\n" );
            DiagPrint(client, nvItemReadResponseMsg, sizeof(nvItemReadResponseMsg)/sizeof(unsigned char));
            return 1; //nvItemReadResponseMsg;
            //break;
       case subSystemDispatch:
        printf("Diag packet received of type 0x%2x(subsystemDispatch) Len[%d]\n", inputMsg[0], cmdLen);
        DiagPrint(client, inputMsg, cmdLen - 3);
        /* check for BT 4B packet and type Bluetooth :
           1st four bytes : diag header
           next 6 bytes for cmd header
           next bytes for cmd payload except last 3 bytes.
           last bytes represents CRC and diag pkt end byte*/
        if ((inputMsg[10] == 1) && (inputMsg[1] == 11) && (inputMsg[2] == 4) && (inputMsg[3] == 0))//check 4B packet
        {
            /* check for length from 6th byte and copy payload from 10th byte */
            int lengthOfPayload = inputMsg[6];
            unsigned char msgToDUT[lengthOfPayload];

            /* check if 7E was replaced */
            int index = 0;
            int payloadIndex = 10;

            for (payloadIndex = 10; payloadIndex < (10 + lengthOfPayload); payloadIndex++)
            {
                if ((inputMsg[payloadIndex] == 0x7D) || (inputMsg[payloadIndex + 1] == 0x5E))
                {
                    msgToDUT[index] = 0x7e;
                    payloadIndex++;
                }
                else
                {
                    msgToDUT[index] = inputMsg[payloadIndex];
                }

                index++;
            }

#ifdef SUPPORT_USB_INTERFACE
            /* Send the HCI command packet*/
            bt_ftm_diag_dispatch(inputMsg, cmdLen);
#else
            /* Send the HCI command packet*/
            bt_ftm_diag_dispatch(&inputMsg[4], cmdLen - 4);
#endif
            printf("Data Out(to DUT)(hex): ");

            for (i = 0; i < lengthOfPayload; i++)
                DPRINTF(FTM_DBG_TRACE, "%02x ", msgToDUT[i]);

            printf("\n");

        }
            return 1;
            //break;
       case featureQuery:
            printf( "echo for featureQuery\n" );
            DiagPrint(client, featureQueryRespMsg, sizeof(featureQueryRespMsg)/sizeof(unsigned char));
            return 1; //featureQueryRespMsg;
            //break;
       case embeddedFileOperation:
            printf( "echo for embeddedFileOperation\n" );
            DiagPrint(client, inputMsg, cmdLen);
            return 1; //inputMsg;
            //break;
       case diagProtLoopback:
            printf( "echo for diagProtLoopback\n" );
            DiagPrint(client, inputMsg, cmdLen-3);
            return 1;
            //break;
       case extendedBuildID:
            printf( "echo for extendedBuildID\n" );
            DiagPrint(client, extendedBuildIDRespMsg, sizeof(extendedBuildIDRespMsg)/sizeof(unsigned char));
            return 1; //extendedBuildIDRespMsg;
            //break;
       default:
            printf( "echo for UNKNOWN ID\n" );
            printf(">>>> Diag packet received of unknown type %d\n", inputMsg[0]);
            for(i=0; i<cmdLen; i++) {
                printf("%x ", inputMsg[i]);
            }
            printf("\n");
            //For now, pass on anything else as a TLV
            //first echo diag packet back to see if will keep caller happy
            DiagPrint(client, inputMsg, cmdLen);
            //It contains a TLV, remove Diag header and send TLV on
            ESCAPEcnt = removeDiagHeader(inputMsg, cmdLen);
            // processTLVPacket(inputMsg, cmdLen-19-ESCAPEcnt, respdata, &nrespdata);
            return 1;
            //break;
    }
}
