/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2015 NXP Semiconductors
 * The original Work has been changed by NXP Semiconductors.
 *
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*=========================================================================
  FTM NFC NQ Firmware Download Source File
  Description
  This file contains the definitions of the functions
  used to download firmware onto the NQ Chip.
===========================================================================*/

#include "ftm_nfcnq_fwdl.h"
#include "ftm_nfcnq.h"
unsigned int fw_version   = 0x00;
unsigned int chip_version = 0x00;
/* Indicates if firmware packets would be send/received from NFCC */
bool_t in_fw_dwl_mode = 0;

int ftm_nfc_send( uint8_t* buf );

/* lookup table for CRC-16-CCITT calculation */
static uint16_t const crcTable[ 256 ] =
    { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
    0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b,
    0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7,
    0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
    0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a,
    0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e,
    0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae,
    0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32,
    0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca,
    0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235,
    0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3,
    0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d,
    0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f,
    0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d,
    0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
    0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64,
    0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };


/*==========================================================================================================
FUNCTION
    load_firmware_from_library

DESCRIPTION
    gets a pointer to the firmware image and the length of the image

PARAMETERS
    const char      *pathToLib          - path to the firmware image library
    uint8_t        **ppFirmwareImage    - pointer to the pointer to the firmware image
    uint16_t        *pFirmwareImageLen  - pointer to the firmware image length

RETURN VALUE
    void

==========================================================================================================*/
static void load_firmware_from_library( const char *pathToLib, uint8_t **ppFirmwareImage,
                                        uint32_t   *pFirmwareImageLen )
{
    void    *pFirmwareLibHandle     = NULL;
    void    *pTempFirmwareImage     = NULL;
    void    *pTempFirmwareImageLen  = NULL;
    int      status                 = -1;

    do
    {
        if( NULL == pathToLib )
        {
            LOG_MESSAGE("\n fw version 0x%x\n", fw_version);
            if(fw_version == 0x01)
            {
                pathToLib = FIRMWARE_BINARY_PATH"libsn100u_fw.so";              // set the path to sn100u firmware library
                if(chip_version == CHIP_ID_SN220)
                    pathToLib = FIRMWARE_BINARY_PATH"libsn220u_fw.so";          // set the path to sn220 firmware library
            }
            else if(fw_version == 0x12)
                pathToLib = FIRMWARE_BINARY_PATH"libpn557_fw.so";                // set the path to pn557 firmware library
            else if(fw_version == 0x11)
                pathToLib = FIRMWARE_BINARY_PATH"libpn553_fw.so";                // set the path to pn553 firmware library
            else
                pathToLib = FIRMWARE_BINARY_PATH"libpn548ad_fw.so";           // set the default path to pn548ad firmware library
        }

        if( NULL != pFirmwareLibHandle )
        {
            status = dlclose( pFirmwareLibHandle );                                                 // if the firmware library handle is not NULL, release the handle
            pFirmwareLibHandle = NULL;

            dlerror( );                                                                             // clear existing errors
            if( 0 != status )
            {
                LOG_ERROR( "%s: dlclose() failed with status = %d \n", __FUNCTION__, status );
                break;
            }
        }

        pFirmwareLibHandle = dlopen( pathToLib, RTLD_LAZY );                                        // get a handle to firmware library
        LOG_MESSAGE( "Opening library handle from %s\n", pathToLib );

        if( NULL == pFirmwareLibHandle )
        {
            LOG_ERROR( "%s: dlopen() of %s failed  \n", __FUNCTION__, pathToLib);
            break;
        }
        dlerror( );                                                                                 // clear existing errors

        pTempFirmwareImage = ( void * )dlsym( pFirmwareLibHandle, "gphDnldNfc_DlSeq" );             // get a pointer to the firmware library

        if( dlerror( ) || ( NULL == pTempFirmwareImage ) )
        {
            LOG_ERROR( "%s: dlsym() failed, failed to load gphDnldNfc_DlSeq symbol \n", __FUNCTION__ );
            break;
        }
        *ppFirmwareImage = *( uint8_t ** )pTempFirmwareImage;                                       // the returned pointer is a pointer to an uint8_t array

        pTempFirmwareImageLen = ( void * ) dlsym( pFirmwareLibHandle, "gphDnldNfc_DlSeqSz" );       // get a pointer to the firmware library length

        if( dlerror( ) || ( NULL == pTempFirmwareImageLen ) )
        {
            LOG_ERROR( "%s: dlsym() failed, failed to load gphDnldNfc_DlSeqSz symbol \n", __FUNCTION__ );
            break;
        }
        *pFirmwareImageLen = ( uint32_t )( *( ( uint32_t * )pTempFirmwareImageLen ) );              // the returned pointer is a pointer to the length of the image

    } while( FALSE );
}

/*==========================================================================================================
FUNCTION
    send_packet_packet_to_chip

DESCRIPTION
    sends the constructed packets to the NFC chip by calling ProcessCommand() from ftm_nfcnq.c

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void send_packet_packet_to_chip( pfirmware_download_context_t pDownloadContext )
{
    int status = -1;

    status = ProcessCommand( &pDownloadContext->packetToSend );                                     // call ProcessCommand() from ftm_nfcnq.c
    if( 0 != status )
    {
        LOG_ERROR( "%s: ProcessCommand() failed with status = %d \n", __FUNCTION__, status );
    }
}

/*==========================================================================================================
FUNCTION
    calculate_crc16

DESCRIPTION
    calculates CRC-16-CCITT of a given buffer with a given length with seed value of 0xffff(Hex)

PARAMETERS
    uint8_t *pBuff      - buffer for CRC-16-CCITT calculation
    uint16_t buffLen    - length of buffer for CRC-16-CCITT calculation

RETURN VALUE
    uint16_t            - calculated CRC-16-CCITT value of buffer

==========================================================================================================*/
static uint16_t calculate_crc16( uint8_t *pBuff, uint16_t buffLen )
{
    uint16_t temp   = 0;
    uint16_t value  = 0;
    uint16_t crc    = 0xffff;                                                                       // seed value
    uint16_t i      = 0;

    if ( ( NULL == pBuff ) || ( 0 == buffLen ) )
    {
        LOG_ERROR( "%s: Invalid parameters \n", __FUNCTION__ );
    }
    else
    {
        for( i = 0; i < buffLen; i++ )
        {
            value   = 0x00ffU & ( uint16_t )pBuff[ i ];
            temp    = ( crc >> 8U ) ^ value;
            crc     = ( crc << 8U ) ^ crcTable[ temp ];
        }
    }

    return crc;
}

/*==========================================================================================================
FUNCTION
    insert_crc16

DESCRIPTION
    inserts the calculated CRC-16-CCITT value into the end of the buffer

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void insert_crc16( pfirmware_download_context_t pDownloadContext )
{
    uint16_t    crcValueToWrite = 0;
    uint8_t    *crcValueInBytes = NULL;

    /* get CRC-16-CCITT value of packet and convert it into 2 bytes */
    crcValueToWrite = calculate_crc16( &pDownloadContext->packetToSend,
                                        pDownloadContext->headerPlusPayloadLen );
    crcValueInBytes = ( uint8_t * )&crcValueToWrite;

    /* insert crc value into last 2 bytes of the packet */
    if( DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetToSend.payloadLen) < ( FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN + FIRMWARE_DOWNLOAD_PACKET_CRC16_LEN - 1 ))
    {
        pDownloadContext->packetToSend.payloadBuff[ DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetToSend.payloadLen) ]     = crcValueInBytes[ 1 ];
        pDownloadContext->packetToSend.payloadBuff[ DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetToSend.payloadLen) + 1 ] = crcValueInBytes[ 0 ];
    }
    else
    {
        LOG_ERROR( "%s: Packet to send payloadLen more than maximum payloadBuff size \n", __FUNCTION__ );
    }
}

/*==========================================================================================================
FUNCTION
    isNciResponse

DESCRIPTION
    Checks if the received response packet is an NCI packet.

PARAMETERS
    uint8_t responsePacketFirstByte - first byte of the response packet

RETURN VALUE
    uint8_t

==========================================================================================================*/
static uint8_t isNciResponse( uint8_t responsePacketFirstByte )
{
    return ( responsePacketFirstByte == NCIMT_RSP || responsePacketFirstByte == NCIMT_NTF );
}

/*==========================================================================================================
FUNCTION
    read_response_from_chip

DESCRIPTION
    reader thread that constantly checks for responses from NFC chip, checks the integrity of the
    response packets by matching the CRC-16-CCITT values and signals the semaphore held by
    the call to ProcessCommand()

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void *

==========================================================================================================*/
static void *read_response_from_chip( void * ptmpDownloadContext )
{
    pfirmware_download_context_t pDownloadContext = (pfirmware_download_context_t) ptmpDownloadContext;
    uint8_t     lenRead                 = 0;
    uint8_t    *pPacketReceived         = NULL;
    uint16_t    calculatedCrcValue      = 0;
    uint16_t    crcValueFromResponse    = 0;
    uint8_t     totalHeaderLength       = 0;
    uint8_t     payloadLength           = 0;
    uint8_t     nciHeaderWithoutGID[2];
    uint8_t     rsp_byte_0 = 0;
    uint8_t     temp = 0;
    uint8_t     *buf = &pDownloadContext->packetReceived ;

    do
    {
        LOG_MESSAGE("%s: entry\n", __func__);

        if( fdNfc < 0 )
        {
            LOG_ERROR( "%s: Invalid handle \n", __FUNCTION__ );
            break;
        }

        LOG_MESSAGE("%s: reading first byte\n", __func__);
        lenRead = read( fdNfc, &rsp_byte_0,             // read first byte to check if NCI or FW_DNLD packet
                        FRAGMENT_OR_GID_BYTE_LEN );
        if( 0 == lenRead )
        {
            LOG_ERROR( "%s: Error reading response packet fragment or GID byte \n", __FUNCTION__ );
            break;
        }

        LOG_MESSAGE("\n rsp[0] -> 0x%02x\n", rsp_byte_0);

        pDownloadContext->packetReceived.payloadLen = rsp_byte_0;

        pDownloadContext->totalPacketLen = lenRead;

        if( isNciResponse( rsp_byte_0 ) )
        {
            lenRead = read( fdNfc, &nciHeaderWithoutGID,                                // get the rest of header if packet is NCI response
                        NCI_PACKET_HEADER_LEN - FRAGMENT_OR_GID_BYTE_LEN );
            payloadLength = nciHeaderWithoutGID[1];

            LOG_MESSAGE("%s: received NCI packet\n", __func__);
            PrintBytes('R', &nciHeaderWithoutGID, NCI_PACKET_HEADER_LEN - FRAGMENT_OR_GID_BYTE_LEN );
        }
        else
        {
            LOG_MESSAGE("%s: received FW_DNLD packet\n", __func__);

            lenRead = read( fdNfc, &rsp_byte_0,                // get the FW_DNLD response packet payload length byte
                        FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN - FRAGMENT_OR_GID_BYTE_LEN );

            pDownloadContext->packetReceived.payloadLen = (pDownloadContext->packetReceived.payloadLen << 8) | (rsp_byte_0 );

            payloadLength = pDownloadContext->packetReceived.payloadLen  + FIRMWARE_DOWNLOAD_PACKET_CRC16_LEN;

            LOG_MESSAGE("%s: receiving payload len %d bytes, pDownloadContext->packetReceived.payloadLen %d \n", __func__,payloadLength, pDownloadContext->packetReceived.payloadLen);

           LOG_MESSAGE("\n rsp[1] -> 0x%02x\n", rsp_byte_0);
        }


        if( 0 == lenRead )
        {
            LOG_ERROR( "%s: Error reading rest of the response packet header \n", __FUNCTION__ );
            break;
        }

        pDownloadContext->totalPacketLen += lenRead;                                            // update the total length of the received packet
        totalHeaderLength = pDownloadContext->totalPacketLen;

        lenRead = read( fdNfc, &pDownloadContext->packetReceived.payloadBuff,                   // get the rest of the response packet
                   payloadLength );
        if( 0 == lenRead )
        {
            LOG_ERROR( "%s: Error reading response packet payload \n", __FUNCTION__ );
            break;
        }
        LOG_MESSAGE("%s: payload, [crc]:\n", __func__);
        PrintBytes('R', &pDownloadContext->packetReceived.payloadBuff, payloadLength );

        pDownloadContext->totalPacketLen += lenRead;                                            // update the total length of the received packet

        if( totalHeaderLength == NCI_PACKET_HEADER_LEN )
        {
            //printing this as there no reader to process this NTF
            if(NCIMT_NTF == rsp_byte_0)  {
                LOG_MESSAGE("%s: Received below NTF during FWDL:\n", __func__);
                PrintBytes('R', &rsp_byte_0, FRAGMENT_OR_GID_BYTE_LEN);
                PrintBytes('R', &pDownloadContext->packetReceived.payloadLen, FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN - FRAGMENT_OR_GID_BYTE_LEN);
                PrintBytes('R', &pDownloadContext->packetReceived.payloadBuff, payloadLength );
            }
            LOG_MESSAGE( "%s: posting about reception of NCI packet\n", __func__);
            sem_post( &sRspReady );

            continue;
        }

        /*
            Converting the payload length from big endian to little endian.
            first two bytes of firmware packet, which hold the payload length will be in big endian and whereas in SW we need it in little endian, so doing swapping for it.
            Ex: For below response
            NxpNciR : len = 8 > 0004000000008716
            Response length is 0x0004 bytes.
            If we access without swapping(little endian conversion), it will be read as 0x0400 */

        temp = buf[0];
        buf[0] = buf[1];
        buf[1] = temp;

        calculatedCrcValue = calculate_crc16( &pDownloadContext->packetReceived,                // calculate the CRC-16-CCITT value of the received packet
                                             ( DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetReceived.payloadLen) +
                                               FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN ) );

        /* convert crc value from the response packet to an uint16_t */
        if( DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetReceived.payloadLen) < ( FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN + FIRMWARE_DOWNLOAD_PACKET_CRC16_LEN - 1 ))
        {
            crcValueFromResponse   = pDownloadContext->packetReceived.payloadBuff[ DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetReceived.payloadLen) ];
            crcValueFromResponse <<= 8;
            crcValueFromResponse  |= pDownloadContext->packetReceived.payloadBuff[ DL_GET_PAYLOAD_LEN_LITTLE_ENDIAN(pDownloadContext->packetReceived.payloadLen) + 1];
        }
        else
        {
            LOG_ERROR( "%s: Packet received payloadLen more than maximum payloadBuff size \n", __FUNCTION__ );
        }

        if( calculatedCrcValue != crcValueFromResponse )                                        // compare the CRC-16-CCITT values
        {
            LOG_ERROR( "%s: CRC-16-CCITT values do not match, discarding packet, calculate value 0x%04x, crcValueFromResponse 0x%04x \n", __FUNCTION__ , calculatedCrcValue, crcValueFromResponse);
            break;
        }
        else
        {
            PrintBytes('R', pDownloadContext->packetReceived.payloadBuff, pDownloadContext->totalPacketLen - FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN );
            LOG_MESSAGE("%s:%d posting about resp\n", __func__, __LINE__);
            sem_post( &sRspReady );                                                             // signal the semaphore for subsequent packets to be sent
        }

    } while( FALSE == pDownloadContext->fExitReadThread );                                      // exit only when the flag is set
    return NULL;
}

/*==========================================================================================================
FUNCTION
    get_device_firmware_version

DESCRIPTION
    sends the get-firmware-version command (0xF1) to the device and outputs the firmware version of
    the device

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void get_device_firmware_version( pfirmware_download_context_t pDownloadContext )
{
    uint8_t getFirmwareVersionCommand[ ]    = { 0x00, 0x04, 0xF1, 0x00, 0x00, 0x00 };           // command to get firmware version on device
    uint8_t firmwareMajorVersion            = 0;
    uint8_t firmwareMinorVersion            = 0;

    LOG_MESSAGE("\n%s\n", __func__);
    pDownloadContext->headerPlusPayloadLen =
              sizeof( getFirmwareVersionCommand ) / sizeof( getFirmwareVersionCommand[ 0 ] );

    memcpy( &pDownloadContext->packetToSend, &getFirmwareVersionCommand,                        // construct the command packet
           ( pDownloadContext->headerPlusPayloadLen ) );

    LOG_MESSAGE("\n%s: headerPlusPayloadLen %d\n", __func__, pDownloadContext->headerPlusPayloadLen);

    insert_crc16( pDownloadContext );                                                           // insert the CRC-16-CCITT value
    send_packet_packet_to_chip( pDownloadContext );                                             // send the command packet to NFC chipp

    /* continues from here once the reader thread reads the response and flags the semaphore,
       the last 2 bytes of the get version response payload contains the firmware version currently on the device */
    if((chip_version == CHIP_ID_SN100_A)||(chip_version == CHIP_ID_SN100_B)||(chip_version == CHIP_ID_SN220))
    {
        firmwareMajorVersion = pDownloadContext->packetReceived.payloadBuff[ GET_VERSION_FW_MAJOR_VER_OFFSET ];
        firmwareMinorVersion = pDownloadContext->packetReceived.payloadBuff[ GET_VERSION_RSP_MINOR_VERSION_OFFSET];
    }
    else
    {
        firmwareMajorVersion = pDownloadContext->packetReceived.payloadBuff[ pDownloadContext->packetReceived.payloadLen - 1 ];
        firmwareMinorVersion = pDownloadContext->packetReceived.payloadBuff[ pDownloadContext->packetReceived.payloadLen - 2 ];
    }
    LOG_INFORMATION( "\n%s: Firmware version on HW: %02X.%02X.%02X\n", __func__, fw_version, firmwareMajorVersion, firmwareMinorVersion );
}

/*==========================================================================================================
FUNCTION
    reset_nfcc_to_nci_mode

DESCRIPTION
    sends the dl_reset command (0xF0) to the device for NFCC soft reset

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void reset_nfcc_to_nci_mode( pfirmware_download_context_t pDownloadContext )
{
    uint8_t resetNfccToNciModeCommand[ ]    = { 0x00, 0x04, 0xF0, 0x00, 0x00, 0x00 };           // command to soft reset NFCC

    LOG_MESSAGE("\n%s\n", __func__);

    pDownloadContext->headerPlusPayloadLen =
              sizeof( resetNfccToNciModeCommand ) / sizeof( resetNfccToNciModeCommand[ 0 ] );

    memcpy( &pDownloadContext->packetToSend, &resetNfccToNciModeCommand,                        // construct the command packet
           ( pDownloadContext->headerPlusPayloadLen ) );

    insert_crc16( pDownloadContext );                                                           // insert the CRC-16-CCITT value
    ftm_nfc_send(&pDownloadContext->packetToSend );			// no response will be there for this command

    /* continues from here once the reader thread reads the response and flags the semaphore,
       the first byte of reset command response payload contains the status
       The reset will prevent the SN1xx NFCC from sending the DL_STATUS_OK( 0 ) answer.
       Thus, only erronous status can be received. So, no need to check the status byte.
       In case of NQxxx chipsets, reset will be done as hard reset later */
}

/*==========================================================================================================
FUNCTION
    build_firmware_packet

DESCRIPTION
    constructs subsequent packets required to be sent to the NFC chip

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void build_firmware_packet( pfirmware_download_context_t pDownloadContext )
{
    uint8_t* buf = &pDownloadContext->packetToSend.payloadLen;
    uint8_t temp = 0;

    /* for chunks from library that are larger than 256 bytes, the packets have to be fragmented */
    if( pDownloadContext->bytesLeftToSend > FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN )
    {
        pDownloadContext->headerPlusPayloadLen = FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN +         // length of header plus the payload for CRC-16-CCITT calculation
                                                 FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN;

        pDownloadContext->totalPacketLen = FIRMWARE_DOWNLOAD_MAX_PACKET_LEN;                        // length of the entire packet to be sent

       pDownloadContext->packetToSend.payloadLen = FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN ;

       if(chip_version == CHIP_ID_SN220)
       {
            pDownloadContext->packetToSend.payloadLen = SET_FRAG_BIT_SN220(pDownloadContext->packetToSend.payloadLen);
       }
       else
       {
            pDownloadContext->packetToSend.payloadLen = SET_FRAG_BIT_SN1xx(pDownloadContext->packetToSend.payloadLen);
       }
       memcpy( ( &pDownloadContext->packetToSend.payloadBuff ),                                    // copy payload from firmware library
                  &pDownloadContext->pFirmwareImage[ pDownloadContext->readIndexFromLib ],
                   FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN );

        pDownloadContext->readIndexFromLib += FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN;             // update the buffer index used to read from firmware library

        pDownloadContext->bytesLeftToSend -= FIRMWARE_DOWNLOAD_PACKET_MAX_PAYLOAD_LEN;              // update the number of bytes left to send from the chunk
    }

    /* for chunks from library that are smaller than 256 bytes, no fragmentation needed */
    else
    {
        pDownloadContext->headerPlusPayloadLen = pDownloadContext->bytesLeftToSend +                // length of header plus the payload for CRC-16-CCITT calculation
                                                 FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN;

        pDownloadContext->totalPacketLen = pDownloadContext->bytesLeftToSend +                      // length of the entire packet to be sent
                                           FIRMWARE_DOWNLOAD_PACKET_HEADER_LEN +
                                           FIRMWARE_DOWNLOAD_PACKET_CRC16_LEN;

        pDownloadContext->packetToSend.payloadLen = pDownloadContext->bytesLeftToSend ;

        if(chip_version == CHIP_ID_SN220)
        {
            pDownloadContext->packetToSend.payloadLen = CLR_FRAG_BIT_SN220(pDownloadContext->packetToSend.payloadLen);
        }
        else
        {
            pDownloadContext->packetToSend.payloadLen = CLR_FRAG_BIT_SN1xx(pDownloadContext->packetToSend.payloadLen);
        }

        memcpy( ( &pDownloadContext->packetToSend.payloadBuff ),                                    // copy payload from firmware library
                  &pDownloadContext->pFirmwareImage[ pDownloadContext->readIndexFromLib ],
                   pDownloadContext->bytesLeftToSend );

        pDownloadContext->readIndexFromLib += pDownloadContext->bytesLeftToSend;                    // update the buffer index used to read from firmware library

        pDownloadContext->bytesLeftToSend = 0;                                                      // most likely the last fragment from the chunk
    }

    //swap byte 0 and 1, to convert to big endian
    temp = buf [0];
    buf[0] = buf[1];
    buf[1] = temp;

    insert_crc16( pDownloadContext );
}

/*==========================================================================================================
FUNCTION
    process_packets_to_send

DESCRIPTION
    determines if the incoming packet is the first one or any subsequent ones and process them
    accordingly

PARAMETERS
    pfirmware_download_context_t pDownloadContext - pointer to structure containing all the
                                                    information required

RETURN VALUE
    void

==========================================================================================================*/
static void process_packets_to_send( pfirmware_download_context_t pDownloadContext )
{
    uint16_t    nextChunkLenFromLib     = 0;
    uint32_t    buffIndex               = pDownloadContext->readIndexFromLib;

    nextChunkLenFromLib = pDownloadContext->pFirmwareImage[ buffIndex ];            // length of next chunk read from the firmware library

    if(fw_version == 0x01)
        nextChunkLenFromLib = nextChunkLenFromLib & 0x03;                           // masking to read last 2 bits(00000011) from first byte for SN100
    else
        nextChunkLenFromLib = nextChunkLenFromLib & 0x00;                           // masking to avoid reading bits from first byte for NQ2xx/3xx/4xx

    /* length of next chunk is stored in 2 bytes in the firmware library */
    nextChunkLenFromLib <<= 8;
    nextChunkLenFromLib  |=  pDownloadContext->pFirmwareImage[ buffIndex + 1 ];

    LOG_MESSAGE( "%s: nextChunkLenFromLib: %d bytes\n", __FUNCTION__, nextChunkLenFromLib );
    buffIndex += 2;                                                                 // add 2 bytes to the buffer index after length of next chunk is read

    pDownloadContext->readIndexFromLib = buffIndex;                                 // update the buffer index used to read from firmware library

    pDownloadContext->bytesLeftToSend = nextChunkLenFromLib;                        // number of bytes left on the chunk to be sent to the chip

    while( pDownloadContext->bytesLeftToSend > 0 )                                  // constructs and sends packets as long as there are bytes left in the chunk
    {
        build_firmware_packet( pDownloadContext );
        send_packet_packet_to_chip( pDownloadContext );
    }
}


/*==========================================================================================================
FUNCTION
    ftm_nfc_dispatch_nq_fwdl

DESCRIPTION
    called by main() in ftm_main.c to start the firmware download routine

PARAMETERS
    none

RETURN VALUE
    void

==========================================================================================================*/
void ftm_nfc_dispatch_nq_fwdl( void )
{
    int          status                 = 0;
    char        *pathToLib              = NULL;
    uint8_t     *pFirmwareImage         = NULL;
    uint32_t     firmwareImageLen       = 0;

    uint8_t     *pNextChunkFromLib      = NULL;
    uint32_t     nextChunkLenFromLib    = 0;
    uint32_t     totalBytesReadFromLib  = 0;
    uint16_t     readIndexFromLib       = 0;
    union        nqx_uinfo           nqx_info;
    pthread_t                       readerThread;
    bool_t is_teared = 0;

    firmware_download_context_t     downloadContext     = { 0 };
    pfirmware_download_context_t    pDownloadContext    = &downloadContext;
    pDownloadContext->fFirstPacket                      = TRUE;

    uint8_t      coreResetCmd[ ]        = { 0x20, 0x00, 0x01, 0x00 };
    uint8_t      coreResetCmdFW[]       = { 0x20, 0x00, 0x01, 0x80 };

    do
    {
        if( !fdNfc )
        {
            status = ftm_nq_nfc_open( );                                                // get a handle to the kernel driver
            if( status < 0 )
            {
                LOG_ERROR( "\n%s: ftm_nq_nfc_open() failed with status = %d \n", __FUNCTION__, status );
                break;
            }

            nqx_info.i = ioctl( fdNfc, NFCC_GET_INFO, 0 );
            if( nqx_info.i < 0 )
            {
                LOG_ERROR( "%s: nqnfcinfo not enabled, info  = %d \n", __FUNCTION__, nqx_info.i );
            }
            chip_version = nqx_info.info.chip_type;
            LOG_INFORMATION( "\n NQ Chip ID : %x\n", chip_version);
            fw_version = nqx_info.info.rom_version;
            LOG_INFORMATION( "\n Firmware Rom Version : %x", fw_version);
            LOG_INFORMATION( "\n Major num : %x", nqx_info.info.fw_major);
            LOG_INFORMATION( "\n Minor num : %x\n", nqx_info.info.fw_minor);

            status = ftm_hard_reset_nfcc( chip_version );                               // reset NFC hardware
            if( status < 0 )
            {
                LOG_ERROR( "%s: ftm_hard_reset_nfcc() failed with status = %d \n", __FUNCTION__, status );
                goto close_device_node;
            }
        }

        LOG_MESSAGE("%s:%d: starting read thread\n", __func__, __LINE__);
        status = pthread_create( &readerThread, NULL,                                   // create a reader thread
                                 &read_response_from_chip, pDownloadContext );
        if( 0 != status )
        {
            LOG_ERROR( "%s: pthread_create() failed with status = %d \n", __FUNCTION__, status );
            goto close_device_node;
        }

        LOG_MESSAGE("%s:%d: sending core reset \n", __func__, __LINE__);
        status = ProcessCommand(coreResetCmd);
        if( status == -1 )
        {
            LOG_ERROR( "%s: ProcessCommand( coreResetCmd ) error , may be NFC is in teared state %d, recovering... \n", __FUNCTION__, status );

            is_teared = 1;

            status = ioctl( fdNfc, NFC_SET_PWR, FIRMWARE_MODE);
            if( 0 != status )
            {
                LOG_ERROR( "%s: Failed to put NFCC in download mode\n", __FUNCTION__ );
                goto close_device_node;
            }

            in_fw_dwl_mode = 1;
        }
        else
        {
                LOG_MESSAGE("%s:%d: sending core reset pass \n reading firmware", __func__, __LINE__);
        }

        load_firmware_from_library( pathToLib, &pFirmwareImage, &firmwareImageLen );    // get a pointer to firmware library image and get its length
        if( ( NULL == pFirmwareImage ) || ( 0 == firmwareImageLen ) )
        {
            LOG_ERROR( "%s: Firmware library image extraction failed\n", __FUNCTION__ );
            goto close_device_node;
        }

        if(fw_version == 0x01)
        {
            LOG_MESSAGE( "Firmware library image length: %d\n",     firmwareImageLen );
        }
        else
        {
            LOG_MESSAGE( "Firmware library image length: %d\n",     (uint16_t)firmwareImageLen );
        }
        LOG_MESSAGE( "Firmware library image pointer: %X\n",  ( uintptr_t )pFirmwareImage );

        pDownloadContext->pFirmwareImage     = pFirmwareImage;
        if(fw_version == 0x01)
            pDownloadContext->firmwareImageLen   = firmwareImageLen;
        else
            pDownloadContext->firmwareImageLen   = (uint16_t) firmwareImageLen;


        if(chip_version == CHIP_ID_SN220) {
            LOG_INFORMATION( "SN220 Firmware version on file: %02X.%02X.%02X\n", fw_version, pFirmwareImage[ SN220_FW_FILE_MAJOR_VERSION_OFFSET ],
                                                                                                                                                         pFirmwareImage[ SN220_FW_FILE_MINOR_VERSION_OFFSET ] );
        }
        else
        LOG_INFORMATION( "Firmware version on file: %02X.%02X.%02X\n", fw_version, pFirmwareImage[ 5 ], pFirmwareImage[ 4 ] );


        LOG_MESSAGE("\n Sending IOCTL to enter FWDL mode \n");
        if(fw_version == 0x01)
            status = ioctl( fdNfc, NFC_SET_PWR, SN100_FIRMWARE_MODE );                  // set NFCC to firmware download mode
        else
            status = ioctl( fdNfc, NFC_SET_PWR, FIRMWARE_MODE );                        // set NFCC to firmware download mode
        if( 0 != status )
        {
            LOG_ERROR( "%s: Failed to set firmware pin high.\n", __FUNCTION__ );
            goto close_device_node;
        }

        in_fw_dwl_mode = 0;

        sleep( 1 );                                                                     // 1 sec delay to process CORE_RESET_NTF and align CMD-RSP pair thereon.
        status = ProcessCommand(coreResetCmdFW);
        if( status == -1 )
        {
            LOG_ERROR( "%s: ProcessCommand( coreResetCmdFW ) error %d \n", __FUNCTION__, status );

            if(!is_teared)
                goto close_device_node;
        }

        in_fw_dwl_mode = 1;

        /*
           * delay to wait till coreResetCmdFW response is read, before sending get version command,
           * sometimes get version is sent and its response is read, before the response of coreResetFW is read,
           * causing invalid get version response to be read.
           */
        sleep(1);

        LOG_INFORMATION( "\nBefore firmware update...\n" );

        if(!is_teared)
            get_device_firmware_version( pDownloadContext );            // get firmware version loaded on the HW

        LOG_INFORMATION( "\nSending firmware packets... Please wait\n" );

        while( pDownloadContext->readIndexFromLib < pDownloadContext->firmwareImageLen )
        {
            process_packets_to_send( pDownloadContext );                                // build and send download packets with payload from the firmware library image
        }
        LOG_INFORMATION( "All packets sent!\n\n" );

        pDownloadContext->fExitReadThread = TRUE;                                       // set flag to indicate that reader thread is safe to exit

        LOG_INFORMATION( "After firmware update...\n" );
        get_device_firmware_version( pDownloadContext );                                // get device version number after loading firmware

        if(fw_version == 0x01)
        {
            status = ioctl( fdNfc, NFC_SET_PWR, SN100_FW_DWL_LOW );                     // set FW_DNLD GPIO to low
            if( 0 != status )
            {
                LOG_ERROR( "%s: Failed to set firmware pin low.\n", __FUNCTION__ );
                goto close_device_node;
            }
        }

        LOG_INFORMATION( "Waiting for reader thread to terminate...\n" );
        pthread_join( readerThread, NULL );                                             // wait for reader thread to terminate
        LOG_INFORMATION( "Reader thread terminated!\n" );

        LOG_INFORMATION( "Resetting NFCC to NCI mode...\n" );
        reset_nfcc_to_nci_mode( pDownloadContext );                                     // reset the NFCC to NCI mode from download mode

        in_fw_dwl_mode = 0;

        LOG_MESSAGE( "Hard Resetting NFCC...\n" );
        status = ftm_hard_reset_nfcc( chip_version );                                   // reset NFC hardware
        if( status < 0 )
        {
            LOG_ERROR( "%s: ftm_hard_reset_nfcc() failed with status = %d \n", __FUNCTION__, status );
        }

    close_device_node:
        status = ftm_nq_nfc_close( );                                                   // release the handle to the kernel driver
        if( 0 != status )
        {
            LOG_ERROR( "%s: ftm_nq_nfc_close() failed with status = %d \n", __FUNCTION__, status );
        }

        LOG_INFORMATION( "All done!\n\n" );

    } while( FALSE );

}
