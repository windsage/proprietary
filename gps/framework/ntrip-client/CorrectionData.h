/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

CorrectionData.h

GENERAL DESCRIPTION
Interface file for passing
1. The CorrectionDataCb handler
2. RTX data
3. NMEA GGA sentence
4. Header File Version
=============================================================================*/
#ifndef CORRECTION_DATA_H
#define CORRECTION_DATA_H

#include <functional>
#include <stdint.h>

/** Major File Version */
#define CORRECTION_DATA_FILE_MAJOR_VERSION 1
/** Minor File Version */
#define CORRECTION_DATA_FILE_MINOR_VERSION 1
//########## Cross Compiler Code ###################
#ifdef WIN32
#ifndef EP_PUBLIC
#if   defined (EP_EXPORTS)
#define EP_PUBLIC __declspec(dllexport)
#elif defined (EP_IMPORTS)
#define EP_PUBLIC __declspec(dllimport)
#else
#define EP_PUBLIC
#endif
#endif
#elif __GNUC__ >= 4
#define EP_PUBLIC __attribute__( (visibility( "default" )))
#else
#define EP_PUBLIC
#endif
//########## ADD WITH EVERY HEADER UPDATE! ##########
/** @brief Inject the correction data.

    @param buffer
    The correction data message buffer in RTCM 3.x format
    @param bufferSize
    The buffer size
*/
typedef std::function<void(uint8_t* correctionData, uint32_t lengthInBytes)> correctionDataCb;

struct cdVersion {
    /** set to CORRECTION_DATA_FILE_MAJOR_VERSION */
    uint16_t cdHeaderMajorVersion;
    /** set to CORRECTION_DATA_FILE_MINOR_VERSION */
    uint16_t cdHeaderMinorVersion;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Set the correction data call back function to the RTX translator library.
    RTX translator library shall implement this interface.

    @param callback
    The correction data call back function
*/
EP_PUBLIC void cdSetCorrectionDataCbHandler(correctionDataCb callback);

/** @brief Inject the RTX data stream to the RTX translator library.
    RTX translator library shall implement this interface.

    @param rtxData
    The RTX data buffer
    @param lengthInBytes
    The RTX data buffer size in bytes
*/
EP_PUBLIC void cdInjectRtxData(const uint8_t* rtxData, uint32_t rtxDataSize);

/** @brief API to update Nmea.
    RTX translator library shall implement this interface.

    @param nmea
    nmea sentence such as GGA
    https://www.trimble.com/OEM_ReceiverHelp/V4.44/en/NMEA-0183messages_GGA.html
*/
EP_PUBLIC void cdUpdateNmea(const char* nmea);

/** @brief API to let RTX lib report it's header file version
    RTX translator library shall implement this interface

    @param version
    The CORRECTION_DATA_FILE version
*/
EP_PUBLIC void cdGetHeaderVersion(cdVersion* version);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /** CORRECTION_DATA_H */
