/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   This file contains the data structure definitions for DGNSS Correction
   Data framework API

   Copyright (c) 2019 - 2021, 2023 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef QDGNSSDATATYPE_H
#define QDGNSSDATATYPE_H

#include <stdint.h>

#define QDGNSS_DATA_TYPE_MAJOR_VERSION  1
#define QDGNSS_DATA_TYPE_MINOR_VERSION  2

#define CODE_BIAS_COUNT_MAX       32
#define DGNSS_MEASUREMENT_MAX     64
#define DGNSS_SATELLITE_MAX       64
#define DGNSS_AVAILABILITY_MESSAGE_MAX  255
#define DGNSS_PHASE_BIAS_MAX        16
#define DGNSS_SSR_GRID_POINTS_MAX   64

typedef enum {
    /** GPS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GPS     = 0,
    /** SBAS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_SBAS    = 1,
    /** QZSS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_QZSS    = 2,
    /** GALILEO identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GALILEO = 3,
    /** GLONASS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GLONASS = 4,
    /** BDS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_BDS     = 5
} DGnssConstellationIdentifier;

typedef uint16_t  DGnssConstellationBitMask;
typedef enum {
    /** GPS constellation */
    DGNSS_CONSTELLATION_GPS_BIT     = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GPS),
    /** SBAS constellation */
    DGNSS_CONSTELLATION_SBAS_BIT    = (1<<DGNSS_CONSTELLATION_IDENTIFIER_SBAS),
    /** QZSS constellation */
    DGNSS_CONSTELLATION_QZSS_BIT    = (1<<DGNSS_CONSTELLATION_IDENTIFIER_QZSS),
    /** GALILEO constellation */
    DGNSS_CONSTELLATION_GALILEO_BIT = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GALILEO),
    /** GLONASS constellation */
    DGNSS_CONSTELLATION_GLONASS_BIT = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GLONASS),
    /** BDS constellation */
    DGNSS_CONSTELLATION_BDS_BIT     = (1<<DGNSS_CONSTELLATION_IDENTIFIER_BDS)
} DGnssConstellationBits;

typedef enum {
    /** GPS L1 C/A signal */
    DGNSS_SIGNAL_GPS_L1CA  = 0,
    /** GPS L1 P signal */
    DGNSS_SIGNAL_GPS_L1P   = 1,
    /** GPS L1 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L1Z   = 2,
    /** GPS L1 codeless signal */
    DGNSS_SIGNAL_GPS_L1N   = 3,
    /** GPS L2 C/A signal */
    DGNSS_SIGNAL_GPS_L2CA  = 4,
    /** GPS L2 P signal */
    DGNSS_SIGNAL_GPS_L2P   = 5,
    /** GPS L2 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L2Z   = 6,
    /** GPS L2C (M) signal */
    DGNSS_SIGNAL_GPS_L2CM  = 7,
    /** GPS L2C (L) signal */
    DGNSS_SIGNAL_GPS_L2CL  = 8,
    /** GPS L2C (M+L) signal */
    DGNSS_SIGNAL_GPS_L2CML = 9,
    /** GPS L2 semi-codeless signal */
    DGNSS_SIGNAL_GPS_L2D   = 10,
    /** GPS L2 codeless signal */
    DGNSS_SIGNAL_GPS_L2N   = 11,
    /** GPS L5 I signal */
    DGNSS_SIGNAL_GPS_L5I   = 12,
    /** GPS L5 Q signal */
    DGNSS_SIGNAL_GPS_L5Q   = 13,
    /** GPS L5 I+Q signal */
    DGNSS_SIGNAL_GPS_L5IQ  = 14,
    /** GPS L1C-D signal */
    DGNSS_SIGNAL_GPS_L1CD  = 15,
    /** GPS L1C-P signal */
    DGNSS_SIGNAL_GPS_L1CP  = 16,
    /** GPS L1C-(D+P) signal */
    DGNSS_SIGNAL_GPS_L1CDP = 17
} DGnssGpsSignalIdentifier;

typedef uint32_t  DGnssGpsSignalBitMask;
typedef enum {
    /** GPS L1 C/A signal */
    DGNSS_SIGNAL_GPS_L1CA_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CA),
    /** GPS L1 P signal */
    DGNSS_SIGNAL_GPS_L1P_BIT   = (1<<DGNSS_SIGNAL_GPS_L1P),
    /** GPS L1 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L1Z_BIT   = (1<<DGNSS_SIGNAL_GPS_L1Z),
    /** GPS L1 codeless signal */
    DGNSS_SIGNAL_GPS_L1N_BIT   = (1<<DGNSS_SIGNAL_GPS_L1N),
    /** GPS L2 C/A signal */
    DGNSS_SIGNAL_GPS_L2CA_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CA),
    /** GPS L2 P signal */
    DGNSS_SIGNAL_GPS_L2P_BIT   = (1<<DGNSS_SIGNAL_GPS_L2P),
    /** GPS L2 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L2Z_BIT   = (1<<DGNSS_SIGNAL_GPS_L2Z),
    /** GPS L2C (M) signal */
    DGNSS_SIGNAL_GPS_L2CM_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CM),
    /** GPS L2C (L) signal */
    DGNSS_SIGNAL_GPS_L2CL_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CL),
    /** GPS L2C (M+L) signal */
    DGNSS_SIGNAL_GPS_L2CML_BIT = (1<<DGNSS_SIGNAL_GPS_L2CML),
    /** GPS L2 semi-codeless signal */
    DGNSS_SIGNAL_GPS_L2D_BIT   = (1<<DGNSS_SIGNAL_GPS_L2D),
    /** GPS L2 codeless signal */
    DGNSS_SIGNAL_GPS_L2N_BIT   = (1<<DGNSS_SIGNAL_GPS_L2N),
    /** GPS L5 I signal */
    DGNSS_SIGNAL_GPS_L5I_BIT   = (1<<DGNSS_SIGNAL_GPS_L5I),
    /** GPS L5 Q signal */
    DGNSS_SIGNAL_GPS_L5Q_BIT   = (1<<DGNSS_SIGNAL_GPS_L5Q),
    /** GPS L5 I+Q signal */
    DGNSS_SIGNAL_GPS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_GPS_L5IQ),
    /** GPS L1C-D signal */
    DGNSS_SIGNAL_GPS_L1CD_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CD),
    /** GPS L1C-P signal */
    DGNSS_SIGNAL_GPS_L1CP_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CP),
    /** GPS L1C-(D+P) signal */
    DGNSS_SIGNAL_GPS_L1CDP_BIT = (1<<DGNSS_SIGNAL_GPS_L1CDP)
} DGnssGpsSignalBits;

typedef enum {
    /** GLONASS G1 C/A signal */
    DGNSS_SIGNAL_GLONASS_G1CA  = 0,
    /** GLONASS G1 P signal */
    DGNSS_SIGNAL_GLONASS_G1P   = 1,
    /** GLONASS G2 C/A signal */
    DGNSS_SIGNAL_GLONASS_G2CA  = 2,
    /** GLONASS G2 P signal */
    DGNSS_SIGNAL_GLONASS_G2P   = 3,
    /** GLONASS G3-I signal */
    DGNSS_SIGNAL_GLONASS_G3I   = 4,
    /** GLONASS G3-Q signal */
    DGNSS_SIGNAL_GLONASS_G3Q   = 5,
    /** GLONASS G3-(I+Q) signal */
    DGNSS_SIGNAL_GLONASS_G3IQ  = 6
} DGnssGlonassSignalIdentifier;

typedef uint32_t DGnssGlonassSignalBitMask;
typedef enum {
    /** GLONASS G1 C/A signal */
    DGNSS_SIGNAL_GLONASS_G1CA_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G1CA),
    /** GLONASS G1 P signal */
    DGNSS_SIGNAL_GLONASS_G1P_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G1P),
    /** GLONASS G2 C/A signal */
    DGNSS_SIGNAL_GLONASS_G2CA_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G2CA),
    /** GLONASS G2 P signal */
    DGNSS_SIGNAL_GLONASS_G2P_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G2P),
    /** GLONASS G3-I signal */
    DGNSS_SIGNAL_GLONASS_G3I_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G3I),
    /** GLONASS G3-Q signal */
    DGNSS_SIGNAL_GLONASS_G3Q_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G3Q),
    /** GLONASS G3-(I+Q) signal */
    DGNSS_SIGNAL_GLONASS_G3IQ_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G3IQ)
} DGnssGlonassSignalBits;

typedef enum {
    /** Galileo E1 C signal */
    DGNSS_SIGNAL_GALILEO_E1C    = 0,
    /** Galileo E1 A signal */
    DGNSS_SIGNAL_GALILEO_E1A    = 1,
    /** Galileo E1 B (I/NAV OS/SC/SOL) signal */
    DGNSS_SIGNAL_GALILEO_E1B    = 2,
    /** Galileo E1 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1BC   = 3,
    /** Galileo E1 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1ABC  = 4,
    /** Galileo E6 C signal */
    DGNSS_SIGNAL_GALILEO_E6C    = 5,
    /** Galileo E6 A signal */
    DGNSS_SIGNAL_GALILEO_E6A    = 6,
    /** Galileo E6 B signal */
    DGNSS_SIGNAL_GALILEO_E6B    = 7,
    /** Galileo E6 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6BC   = 8,
    /** Galileo E6 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6ABC  = 9,
    /** Galileo E5 B-I signal */
    DGNSS_SIGNAL_GALILEO_E5BI   = 10,
    /** Galileo E5 B-Q signal */
    DGNSS_SIGNAL_GALILEO_E5BQ   = 11,
    /** Galileo E5 B-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5BIQ  = 12,
    /** Galileo E5 (A+B)-I signal */
    DGNSS_SIGNAL_GALILEO_E5ABI  = 13,
    /** Galileo E5 (A+B)-Q signal */
    DGNSS_SIGNAL_GALILEO_E5ABQ  = 14,
    /** Galileo E5 (A+B)-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5ABIQ = 15,
    /** Galileo E5 A-I signal */
    DGNSS_SIGNAL_GALILEO_E5AI   = 16,
    /** Galileo E5 A-Q signal */
    DGNSS_SIGNAL_GALILEO_E5AQ   = 17,
    /** Galileo E5 A-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5AIQ  = 18
} DGnssGalileoSignalIdentifier;

typedef uint32_t DGnssGalileoSignalBitMask;
typedef enum {
    /** Galileo E1 C signal */
    DGNSS_SIGNAL_GALILEO_E1C_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1C),
    /** Galileo E1 A signal */
    DGNSS_SIGNAL_GALILEO_E1A_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1A),
    /** Galileo E1 B (I/NAV OS/SC/SOL) signal */
    DGNSS_SIGNAL_GALILEO_E1B_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1B),
    /** Galileo E1 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1BC_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E1BC),
    /** Galileo E1 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1ABC_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E1ABC),
    /** Galileo E6 C signal */
    DGNSS_SIGNAL_GALILEO_E6C_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6C),
    /** Galileo E6 A signal */
    DGNSS_SIGNAL_GALILEO_E6A_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6A),
    /** Galileo E6 B signal */
    DGNSS_SIGNAL_GALILEO_E6B_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6B),
    /** Galileo E6 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6BC_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E6BC),
    /** Galileo E6 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6ABC_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E6ABC),
    /** Galileo E5 B-I signal */
    DGNSS_SIGNAL_GALILEO_E5BI_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5BI),
    /** Galileo E5 B-Q signal */
    DGNSS_SIGNAL_GALILEO_E5BQ_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5BQ),
    /** Galileo E5 B-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5BIQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5BIQ),
    /** Galileo E5 (A+B)-I signal */
    DGNSS_SIGNAL_GALILEO_E5ABI_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5ABI),
    /** Galileo E5 (A+B)-Q signal */
    DGNSS_SIGNAL_GALILEO_E5ABQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5ABQ),
    /** Galileo E5 (A+B)-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5ABIQ_BIT = (1<<DGNSS_SIGNAL_GALILEO_E5ABIQ),
    /** Galileo E5 A-I signal */
    DGNSS_SIGNAL_GALILEO_E5AI_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5AI),
    /** Galileo E5 A-Q signal */
    DGNSS_SIGNAL_GALILEO_E5AQ_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5AQ),
    /** Galileo E5 A-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5AIQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5AIQ)
} DGnssGalileoSignalBits;

typedef enum {
    /** QZSS L1 C/A signal */
    DGNSS_SIGNAL_QZSS_L1CA  = 0,
    /** QZSS LEX-S signal */
    DGNSS_SIGNAL_QZSS_LEXS  = 1,
    /** QZSS LEX-L signal */
    DGNSS_SIGNAL_QZSS_LEXL  = 2,
    /** QZSS LEX-(S+L) signal */
    DGNSS_SIGNAL_QZSS_LEXSL = 3,
    /** QZSS L2C (M) signal */
    DGNSS_SIGNAL_QZSS_L2CM  = 4,
    /** QZSS L2C (L) signal */
    DGNSS_SIGNAL_QZSS_L2CL  = 5,
    /** QZSS L2C (M+L) signal */
    DGNSS_SIGNAL_QZSS_L2CML = 6,
    /** QZSS L5 I signal */
    DGNSS_SIGNAL_QZSS_L5I   = 7,
    /** QZSS L5 Q signal */
    DGNSS_SIGNAL_QZSS_L5Q   = 8,
    /** QZSS L5 I+Q signal */
    DGNSS_SIGNAL_QZSS_L5IQ  = 9,
    /** QZSS L1C-D signal */
    DGNSS_SIGNAL_QZSS_L1CD  = 10,
    /** QZSS L1C-P signal */
    DGNSS_SIGNAL_QZSS_L1CP  = 11,
    /** QZSS L1C-(D+P) signal */
    DGNSS_SIGNAL_QZSS_L1CDP = 12,
    /** QZSS L1S/L1-SAIF signal */
    DGNSS_SIGNAL_QZSS_L1S   = 13
} DGnssQzssSignalIdentifier;

typedef uint32_t DGnssQzssSignalBitMask;
typedef enum {
    /** QZSS L1 C/A signal */
    DGNSS_SIGNAL_QZSS_L1CA_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CA),
    /** QZSS LEX-S signal */
    DGNSS_SIGNAL_QZSS_LEXS_BIT  = (1<<DGNSS_SIGNAL_QZSS_LEXS),
    /** QZSS LEX-L signal */
    DGNSS_SIGNAL_QZSS_LEXL_BIT  = (1<<DGNSS_SIGNAL_QZSS_LEXL),
    /** QZSS LEX-(S+L) signal */
    DGNSS_SIGNAL_QZSS_LEXSL_BIT = (1<<DGNSS_SIGNAL_QZSS_LEXSL),
    /** QZSS L2C (M) signal */
    DGNSS_SIGNAL_QZSS_L2CM_BIT  = (1<<DGNSS_SIGNAL_QZSS_L2CM),
    /** QZSS L2C (L) signal */
    DGNSS_SIGNAL_QZSS_L2CL_BIT  = (1<<DGNSS_SIGNAL_QZSS_L2CL),
    /** QZSS L2C (M+L) signal */
    DGNSS_SIGNAL_QZSS_L2CML_BIT = (1<<DGNSS_SIGNAL_QZSS_L2CML),
    /** QZSS L5 I signal */
    DGNSS_SIGNAL_QZSS_L5I_BIT   = (1<<DGNSS_SIGNAL_QZSS_L5I),
    /** QZSS L5 Q signal */
    DGNSS_SIGNAL_QZSS_L5Q_BIT   = (1<<DGNSS_SIGNAL_QZSS_L5Q),
    /** QZSS L5 I+Q signal */
    DGNSS_SIGNAL_QZSS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_QZSS_L5IQ),
    /** QZSS L1C-D signal */
    DGNSS_SIGNAL_QZSS_L1CD_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CD),
    /** QZSS L1C-P signal */
    DGNSS_SIGNAL_QZSS_L1CP_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CP),
    /** QZSS L1C-(D+P) signal */
    DGNSS_SIGNAL_QZSS_L1CDP_BIT = (1<<DGNSS_SIGNAL_QZSS_L1CDP),
    /** QZSS L1S/L1-SAIF signal */
    DGNSS_SIGNAL_QZSS_L1S_BIT   = (1<<DGNSS_SIGNAL_QZSS_L1S)
} DGnssQzssSignalBits;

typedef enum {
    /** BDS B1-2 I signal */
    DGNSS_SIGNAL_BDS_B12I   = 0,
    /** BDS B1-2 Q signal */
    DGNSS_SIGNAL_BDS_B12Q   = 1,
    /** BDS B1-2 (I+Q) signal */
    DGNSS_SIGNAL_BDS_B12IQ  = 2,
    /** BDS B3-I signal */
    DGNSS_SIGNAL_BDS_B3I    = 3,
    /** BDS B3-Q signal */
    DGNSS_SIGNAL_BDS_B3Q    = 4,
    /** BDS B3-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B3IQ   = 5,
    /** BDS B3A signal */
    DGNSS_SIGNAL_BDS_B3A    = 6,
    /** BDS B2-I signal */
    DGNSS_SIGNAL_BDS_B2I    = 7,
    /** BDS B2-Q signal */
    DGNSS_SIGNAL_BDS_B2Q    = 8,
    /** BDS B2-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B2IQ   = 9,
    /** BDS B2a Data signal */
    DGNSS_SIGNAL_BDS_B2AD   = 10,
    /** BDS B2a Pilot signal */
    DGNSS_SIGNAL_BDS_B2AP   = 11,
    /** BDS B2a (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2ADP  = 12,
    /** BDS B1C Data signal */
    DGNSS_SIGNAL_BDS_B1CD   = 13,
    /** BDS B1C Pilot signal */
    DGNSS_SIGNAL_BDS_B1CP   = 14,
    /** BDS B1C (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B1CDP  = 15,
    /** BDS B1A signal */
    DGNSS_SIGNAL_BDS_B1A    = 16,
    /** BDS B1 Codeless signal */
    DGNSS_SIGNAL_BDS_B1N    = 17,
    /** BDS B2b Data signal */
    DGNSS_SIGNAL_BDS_B2BD   = 18,
    /** BDS B2b Pilot signal */
    DGNSS_SIGNAL_BDS_B2BP   = 19,
    /** BDS B2b (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2BDP  = 20,
    /** BDS B2(B2a+B2b)-D signal */
    DGNSS_SIGNAL_BDS_B2D    = 21,
    /** BDS B2(B2a+B2b)-P signal */
    DGNSS_SIGNAL_BDS_B2P    = 22,
    /** BDS B2(B2a+B2b)-(D+P) signal */
    DGNSS_SIGNAL_BDS_B2DP   = 23
} DGnssBdsSignalIdentifier;

typedef uint32_t DGnssBdsSignalBitMask;
typedef enum {
    /** BDS B1-2 I signal */
    DGNSS_SIGNAL_BDS_B12I_BIT   = (1<<DGNSS_SIGNAL_BDS_B12I),
    /** BDS B1-2 Q signal */
    DGNSS_SIGNAL_BDS_B12Q_BIT   = (1<<DGNSS_SIGNAL_BDS_B12Q),
    /** BDS B1-2 (I+Q) signal */
    DGNSS_SIGNAL_BDS_B12IQ_BIT  = (1<<DGNSS_SIGNAL_BDS_B12IQ),
    /** BDS B3-I signal */
    DGNSS_SIGNAL_BDS_B3I_BIT    = (1<<DGNSS_SIGNAL_BDS_B3I),
    /** BDS B3-Q signal */
    DGNSS_SIGNAL_BDS_B3Q_BIT    = (1<<DGNSS_SIGNAL_BDS_B3Q),
    /** BDS B3-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B3IQ_BIT   = (1<<DGNSS_SIGNAL_BDS_B3IQ),
    /** BDS B3A signal */
    DGNSS_SIGNAL_BDS_B3A_BIT    = (1<<DGNSS_SIGNAL_BDS_B3A),
    /** BDS B2-I signal */
    DGNSS_SIGNAL_BDS_B2I_BIT    = (1<<DGNSS_SIGNAL_BDS_B2I),
    /** BDS B2-Q signal */
    DGNSS_SIGNAL_BDS_B2Q_BIT    = (1<<DGNSS_SIGNAL_BDS_B2Q),
    /** BDS B2-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B2IQ_BIT   = (1<<DGNSS_SIGNAL_BDS_B2IQ),
    /** BDS B2a Data signal */
    DGNSS_SIGNAL_BDS_B2AD_BIT   = (1<<DGNSS_SIGNAL_BDS_B2AD),
    /** BDS B2a Pilot signal */
    DGNSS_SIGNAL_BDS_B2AP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2AP),
    /** BDS B2a (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2ADP_BIT  = (1<<DGNSS_SIGNAL_BDS_B2ADP),
    /** BDS B1C Data signal */
    DGNSS_SIGNAL_BDS_B1CD_BIT   = (1<<DGNSS_SIGNAL_BDS_B1CD),
    /** BDS B1C Pilot signal */
    DGNSS_SIGNAL_BDS_B1CP_BIT   = (1<<DGNSS_SIGNAL_BDS_B1CP),
    /** BDS B1C (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B1CDP_BIT  = (1<<DGNSS_SIGNAL_BDS_B1CDP),
    /** BDS B1A signal */
    DGNSS_SIGNAL_BDS_B1A_BIT    = (1<<DGNSS_SIGNAL_BDS_B1A),
    /** BDS B1 Codeless signal */
    DGNSS_SIGNAL_BDS_B1N_BIT    = (1<<DGNSS_SIGNAL_BDS_B1N),
    /** BDS B2b Data signal */
    DGNSS_SIGNAL_BDS_B2BD_BIT   = (1<<DGNSS_SIGNAL_BDS_B2BD),
    /** BDS B2b Pilot signal */
    DGNSS_SIGNAL_BDS_B2BP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2BP),
    /** BDS B2b (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2BDP_BIT  = (1<<DGNSS_SIGNAL_BDS_B2BDP),
    /** BDS B2(B2a+B2b)-D signal */
    DGNSS_SIGNAL_BDS_B2D_BIT    = (1<<DGNSS_SIGNAL_BDS_B2D),
    /** BDS B2(B2a+B2b)-P signal */
    DGNSS_SIGNAL_BDS_B2P_BIT    = (1<<DGNSS_SIGNAL_BDS_B2P),
    /** BDS B2(B2a+B2b)-(D+P) signal */
    DGNSS_SIGNAL_BDS_B2DP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2DP)
} DGnssBdsSignalBits;

typedef enum {
    /** SBAS L1 C/A signal */
    DGNSS_SIGNAL_SBAS_L1CA  = 0,
    /** SBAS L5 I signal */
    DGNSS_SIGNAL_SBAS_L5I   = 1,
    /** SBAS L5 Q signal */
    DGNSS_SIGNAL_SBAS_L5Q   = 2,
    /** SBAS L5 I+Q signal */
    DGNSS_SIGNAL_SBAS_L5IQ  = 3
} DGnssSbasSignalIdentifier;

typedef uint32_t  DGnssSbasSignalBitMask;
typedef enum {
    /** SBAS L1 C/A signal */
    DGNSS_SIGNAL_SBAS_L1CA_BIT  = (1<<DGNSS_SIGNAL_SBAS_L1CA),
    /** SBAS L5 I signal */
    DGNSS_SIGNAL_SBAS_L5I_BIT   = (1<<DGNSS_SIGNAL_SBAS_L5I),
    /** SBAS L5 Q signal */
    DGNSS_SIGNAL_SBAS_L5Q_BIT   = (1<<DGNSS_SIGNAL_SBAS_L5Q),
    /** SBAS L5 I+Q signal */
    DGNSS_SIGNAL_SBAS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_SBAS_L5IQ)
} DGnssSbasSignalBits;

/** Reference Station */
typedef struct {
    /** 0 - Real, physical reference station
     *  1 - Non-physical or computed reference station */
    uint8_t  stationIndicator          : 1;
    /** 11 - all raw observations are measured at the same instant
     *  10 - all raw observations may be measured at different instants
     *  00 - unknown / not available */
    uint8_t  oscillatorIndicator       : 2;
    /** 00 - correction status unspecified or not available
     *  01 - phase observations are corrected for quarter cycle bias
     *  10 - phase observation are not corrected for qualter cycle bias
     *  11 - reserved */
    uint8_t  quarterCycleIndicator     : 2;
    /** antenna height available
     *  0 - not availiable
     *  1 - availiable */
    uint8_t  heightAvailability        : 1;
    /** antenna reference point uncertainty avilable
     *  0 - not availiable
     *  1 - availiable */
    uint8_t  referencePointUncertainty : 1;
} StationStatusIndicatorMask;

typedef struct {
    /** reference station ID, valid range: 0-4095 */
    uint16_t stationID;
    /** reference station status indicators */
    StationStatusIndicatorMask  statusIndicator;
    /** linked reference station ID, valid range: 0-4095 */
    uint16_t linkedStationID;
    /** ITRF realization year, valid range: 0-63 */
    uint8_t  ITRFYear;
    /** DGnss constellation bit mask */
    DGnssConstellationBitMask constellationBitMask;
    /** antenna reference point: ECEF-X, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefX;
    /** antenna reference point: ECEF-Y, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefY;
    /** antenna reference point: ECEF-Z, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefZ;
    /** antenna height, valid range: 0-6.5535m */
    float    antennaHeight;
    /** antenna reference point: uncertainty-X */
    float    uncertaintyX;
    /** antenna reference point: uncertainty-Y */
    float    uncertaintyY;
    /** antenna reference point: uncertainty-Z */
    float    uncertaintyZ;
    /** antenna reference point: uncertainty confidence-X, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceX;
    /** antenna reference point: uncertainty confidence-Y, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceY;
    /** antenna reference point: uncertainty confidence-Z, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceZ;
} ReferenceStation;

/** Gnss Time */
union GnssEpochTime {
    /** gps time of week, valid range: 0 - 604799999 ms */
    uint32_t  timeOfWeek;
    struct {
        /** glonass time of day, valid range: 0 - 86400999 ms */
        uint32_t timeOfDay : 27;
        /** glonass day of week, valid range: 0 - 6, set to 7 if unknown or not available */
        uint32_t dayOfWeek : 3;
    } glonassEpochTime;
};

/** GLONASS Bias Information */
/** GLONASS code phase bias indicator and FDMA signal mask */
typedef uint8_t BiasIndicatorFDMASignalMask;
typedef enum {
    /** GLONASS pseudorange and phaserange measurements are aligned to the same epoch */
    GLONASS_PSEUDORANGE_PHASERANGE_SAME_EPOCH = (1<<0),
    /** GLONASS L2-P code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L2P_VALID         = (1<<1),
    /** GLONASS L2-C/A code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L2CA_VALID        = (1<<2),
    /** GLONASS L1-P code phase is valid */
    GLONASS_CODE_PHASE_BIAS_L1P_VALID         = (1<<3),
    /** GLONASS L1-C/A code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L1CA_VALID        = (1<<4)
} BiasIndicatorFdmaSignalBits;

typedef struct {
    /** reference station ID, valid range: 0-4095 */
    uint16_t                      stationID;
    /** GLONASS code phase bias indicator and FDMA signals mask */
    BiasIndicatorFDMASignalMask   biasIndicatorSignalMask;
    /** GLONASS L1 C/A code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL1CA;
    /** GLONASS L1 P code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL1P;
    /** GLONASS L2 C/A code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL2CA;
    /** GLONASS L2 P code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL2P;
} GlonassBias;

/** State Space Representation */
typedef struct {
    /** 0 - last set of corrections available for this type and epoch time
     *  1 - more corrections available for this type and epoch time*/
    uint8_t multipleMessageIndicator : 1;
    /** 0 - SSR provider information invalid/not available
     *  1 - SSR provider information valid */
    uint8_t providerInfoValidity     : 1;
    /** orbit corrections refer to satellite reference datum
     *  0 - ITRF      1 - Regional */
    uint8_t datumType                : 1;
    /** Applicable only for header corresponding to an orbit correction message
        0 – orbit position is referenced to satellite antenna phase center(ex.RTCM, 3GPP)
        1 – orbit position is referenced to satellite center of mass(future compatibility) */
    uint8_t satellitePositionRef     : 1;
    /** Issue of Data SSR */
    uint8_t issueOfData              : 4;
} SsrStatusFlags;

typedef struct {
    /** DGnss constellation identifier */
    DGnssConstellationIdentifier   constellationIdentifier;
    /** GNSS epoch time */
    GnssEpochTime                  epochTime;
    /** SSR status flags */
    SsrStatusFlags                 statusFlags;
    /** SSR update interval, valid range: 0-15 */
    uint8_t                        updateInterval;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t                       providerID;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t                        solutionID;
    /** number of satellites, valid range: 1-255 */
    uint8_t                        numberOfSatellites;
} SsrHeader;

/** Satellite specific part of SSR orbit corrections */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t   svId;
    /** IODE */
    uint16_t  IODE;
    /** Delta radial, valid range: -209.7151m ~ 209.7151m */
    float     deltaRadial;
    /** Delta along-track, valid range: -209.7148 ~ 209.7148m */
    float     deltaAlongTrack;
    /** Delta cross-track, valid range: -209.7148m ~ 209.7148m */
    float     deltaCrossTrack;
    /** Dot delta radial, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaRadial;
    /** Dot delta along-track, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaAlongTrack;
    /** Dot delta cross-track, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaCrossTrack;
} SsrSatelliteOrbitCorrections;     //1.SSR – orbit correction

/** 1 SSR orbit corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteOrbitCorrections satOrbitCorrections[DGNSS_SATELLITE_MAX];
} SsrOrbitCorrections;

/** Satellite specific part of SSR clock corrections */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t   svId;
    /** IODE */
    uint16_t  IODE;
    /** Delta clock C0, valid range: -209.7151 ~ 209.7151 m */
    float     deltaClockC0;
    /** Delta clock C1, valid range: -1.048575 ~ 1.048575 m/s */
    float     deltaClockC1;
    /** Delta clock C2, valid range: -1.34217726 ~ 1.34217726 m/s^2 */
    double    deltaClockC2;
} SsrSatelliteClockCorrections;

/** 2 SSR clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteClockCorrections  satClockCorrections[DGNSS_SATELLITE_MAX];
} SsrClockCorrections;

/** SSR combined orbit and clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteOrbitCorrections  satOrbitCorrections[DGNSS_SATELLITE_MAX];
    SsrSatelliteClockCorrections  satClockCorrections[DGNSS_SATELLITE_MAX];
} SsrOrbitAndClockCorrections;

/** Satellite specific part of SSR high-rate clock corrections */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t  svId;
    /** high rate clock correction, valid range: -209.7151 ~ 209.7151 m */
    float    highRateClockCorrection;
} SsrSatelliteHighRateClockCorrections;

/** 3 SSR high-rate clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteHighRateClockCorrections  satHighRateClockCorrections[DGNSS_SATELLITE_MAX];
} SsrHighRateClockCorrections;

/* satellite specific part of SSR code bias */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t       svId;
    /** number of  code biases processed, valid range: 1-255 */
    uint8_t       numberOfCodeBias;
    union {
        DGnssGpsSignalIdentifier     gpsSignalIdentifier;
        DGnssGlonassSignalIdentifier glonassSignalIdentifier;
        DGnssGalileoSignalIdentifier galileoSignalIdentifier;
        DGnssQzssSignalIdentifier    qzssSignalIdentifier;
        DGnssBdsSignalIdentifier     bdsSignalIdentifier;
        DGnssSbasSignalIdentifier    sbasSignalIdentifier;
    } gnssSignalIdentifier[CODE_BIAS_COUNT_MAX];
    float      codeBias[CODE_BIAS_COUNT_MAX];
} SsrSatelliteCodeBias;

/** 4 SSR code bias */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteCodeBias  satCodeBias[DGNSS_SATELLITE_MAX];
} SsrCodeBias;

/** Satellite specific SSR user range accuracy */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t  svId;
    /** SSR user range accuracy, valid range: 0 – 6013.25 mm
        1-sigma URA for range corrections computed from complete set of SSR corrections.
        0 = unknown accuracy
        Non-zero and ≤ 5466.5 mm – valid range
        Larger than 5466.5 mm = SSR corrections may not reliable */
    float  userRangeAccuracy;
} SsrSatelliteUserRangeAccuracy;

/** 5 SSR user range accuracy */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteUserRangeAccuracy  satUserRangeAccuracy[DGNSS_SATELLITE_MAX];
} SsrUserRangeAccuracy;

/*RTK Observations*/
typedef struct {
    /** 0 - no further observations are expected for this reference station ID and epoch time
     *  1 - more observations are expected for this reference station ID and epoch time */
    uint16_t  synchronousFlag                  : 1;
    /** 0 - Divergence-Free smoothing not used
     *  1 - Divergence-Free smoothing used */
    uint16_t  smoothingIndicator               : 1;
    /** 000 - no smoothing is used
     *  001 - less than 30 seconds
     *  010 - 30-60 seconds
     *  011 - 1 to 2 minutes
     *  100 - 2 to 4 minutes
     *  101 - 4 to 8 minutes
     *  110 - more than 8 minutes
     *  111 - unlimited smoothing interval */
    uint16_t  smoothingInterval                : 3;
    /** 00 - clock steering not applied
     *  01 - clock steering has been applied
     *  10 - unknown clock steering status
     *  11 - reserved */
    uint16_t  clockSteeringIndicator           : 2;
    /** 00 - internal clock is used
     *  01 - external clock is used, status is locked
     *  10 - external clock is used, status is not locked
     *  11 - unknown clock is used */
    uint16_t  externalClockIndicator           : 2;
    /** reserved */
    uint16_t                                   : 7;
} DGnssObservationStatusFlags;

typedef uint8_t  DGnssSignalStatusFlags;
typedef enum {
    /** bit0: 1: fully reconstructed measurements available including integer-milliseconds
     *        0: modulo 1-ms measurements */
    MEASUREMENT_COMPLETE         = (1<<0),
    /** bit1: 1: pseudorange measurements valid
     *        0: pseudorange measurements invalid */
    PSEUDORANGE_VALID            = (1<<1),
    /** bit2: 1: Carrier phase measurements valid
              0: Carrier phase messurements invalid */
    CARRIER_PHASE_VALID          = (1<<2),
    /** bit3: 1: Phase range rate measurements valid
              0: Phase ragne rate measurements invalid */
    PHASE_RANGE_RATE_VALID       = (1<<3),
    /** Half-cycle ambiguity indicator, valid only if carrier phase measurements are set to valid
     *  bit5 bit4: Half cycle ambiguity
     *  00 - No Half-cycle ambiguity
     *  01 - Half-cycle ambiguity
     *  1x - Unknown */
    HALF_CYCLE_AMBIGUITY_VALID   = (1<<4),
    /** Half cycle ambiguity unknown */
    HALF_CYCLE_AMBIGUITY_UNKNOWN = (1<<5)
} DGnssSignalStatusBits;

typedef enum {
    /** nominal value of frequency: L1 1598.0625 MHz, L2 1242.9375 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7  = -7,
    /** nominal value of frequency: L1 1598.6250 MHz, L2 1243.3750 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6  = -6,
    /** nominal value of frequency: L1 1599.1875 MHz, L2 1243.8125 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5  = -5,
    /** nominal value of frequency: L1 1599.7500 MHz, L2 1244.2500 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4  = -4,
    /** nominal value of frequency: L1 1600.3125 MHz, L2 1244.6875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3  = -3,
    /** nominal value of frequency: L1 1600.8750 MHz, L2 1245.1250 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2  = -2,
    /** nominal value of frequency: L1 1601.4375 MHz, L2 1245.5625 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1  = -1,
    /** nominal value of frequency: L1 1602.0 MHz, L2 1246.0 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0        = 0,
    /** nominal value of frequency: L1 1602.5625 MHz, L2 1246.4375 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1        = 1,
    /** nominal value of frequency: L1 1603.125 MHz, L2 1246.875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2        = 2,
    /** nominal value of frequency: L1 1603.6875 MHz, L2 1247.3125 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3        = 3,
    /** nominal value of frequency: L1 1604.25 MHz, L2 1247.75 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4        = 4,
    /** nominal value of frequency: L1 1604.8125 MHz, L2 1248.1875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5        = 5,
    /** nominal value of frequency: L1 1605.375 MHz, L2 1248.625 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6        = 6,
    /** reserved */
    DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED        = 14,
    /** frequency channel number is not known or not available */
    DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN         = 15
} ExtendedSatelliteInformation;

typedef struct {
     /** GPS SV ID, valid range: 1-32
      *  GLONASS SV ID, valid range: 1-24
      *  GALILEO SV ID, valid range: 1-36
      *  QZSS SV ID, valid range: 193-202
      *  BeiDou SV ID, valid range: 1-63
      *  SBAS SV ID, range:120-158
      */
    uint8_t svId;
    /** Extended satellite information */
    ExtendedSatelliteInformation extendedSatelliteInformation;
    /** GNSS signal identifier */
    union {
        DGnssGpsSignalIdentifier     gpsSignalIdentifier;
        DGnssGlonassSignalIdentifier glonassSignalIdentifier;
        DGnssGalileoSignalIdentifier galileoSignalIdentifier;
        DGnssQzssSignalIdentifier    qzssSignalIdentifier;
        DGnssBdsSignalIdentifier     bdsSignalIdentifier;
        DGnssSbasSignalIdentifier    sbasSignalIdentifier;
    } gnssSignalIdentifier;
    /** GNSS signal specific observation status */
    DGnssSignalStatusFlags signalObservationStatus;
    /** valid range: 0-76447076.7894m, full pseudorange, represented in meters with
     *               a precision of 0.0006m
     *               0-299792.4574m, modulo 1-ms pseudorange, represented in meters
     *               with a precision of 0.0006m*/
    double  pseudorange;
    /** valid range: 0-76447976.16723m, full phase range, represented in meters with
     *               a precision of 0.00014m
     *               0-299792.45786m, modulo 1-ms phase range, represented in meters
     *               with a precision of 0.00014m*/
    double  phaseRange;
    /** Phaserange-Rate, valid range: -8192.6383 ~ 8192.6383 m */
    double  phaseRangeRate;
    /** Phase range lock time indicator, valid range: 0-1023 */
    uint16_t phaseRangeLockTimeIndicator;
    /** GNSS signal CNR, valid range: 0.0625-63.9375 dB-Hz */
    float signalCNR;
} DGnssMeasurement;

typedef struct {
    /** Reference station ID, valid range: 0-4095 */
    uint16_t                      stationID;
    /** GNSS epoch time */
    GnssEpochTime                 epochTime;
    /** common observation status flags */
    DGnssObservationStatusFlags   statusFlags;
    /** issue of Data Station, valid range: 0-7 */
    uint8_t                       issueOfDataStation;
    /** DGNSS constellaton identifier */
    DGnssConstellationIdentifier  constellationIdentifier;
    /** number of measurements, valid range: 1-64 */
    uint8_t                       numberOfMeasurements;
    DGnssMeasurement dGnssMeasurement[DGNSS_MEASUREMENT_MAX];
} DGnssObservation;

/*Ephemeris data*/
typedef struct {
    /** GPS SV ID, valid range: 1-32,
     *  QZSS SV ID, valid range: 193-202 */
    uint8_t    svId;
    /** status for L2 frequency (2-bits) */
    uint8_t    codeL2;
    /** User Range Accuracy (4-bits) */
    uint8_t    accuracy;
    /** SV health (6-bits) */
    uint8_t    health;
    /** issue of data clock (10-bits) */
    uint16_t   iodc;
    /** P-Code indication flag (1-bit) */
    uint8_t    pcodeFlag;
    /** True if P-code indication is valid */
    bool       isPcodeFlagValid;
    /** Reserved bits (87-bits, First bit in LSB) */
    uint32_t   sf1ResvBits[3];
    /** True if SF1 Reserved bits are valid */
    bool       areSf1BitsValid;
    /** Time of Group Delay (8-bits) */
    uint8_t    tgd;
    /** Time of Group Delay vadidity flag */
    bool       isTgdValid;
    /** Time of clock (16-bits) */
    uint16_t   toc;
    /** Clock acceleration coefficient(8-bits) */
    uint8_t    af2;
    /** Clock drift coefficient (16-bits) */
    uint16_t   af1;
    /** Clock bias coefficient (22-bits) */
    uint32_t   af0;
    /** Issue of data ephemeris tag (8-bits) */
    uint8_t    iode;
    /** Amplitude of the sine harmonic correction term to Orbit Radius (16-bits) */
    uint16_t   Crs;
    /** Mean motion difference from computed value (16-bits) */
    uint16_t   deltaN;
    /** Mean anomaly at reference time (32-bits) */
    uint32_t   M0;
    /** Amplitude of the cosine harmonic correction term to argument of latitude (16-bits) */
    uint16_t   Cuc;
    /** orbital eccentricity (32-bits) */
    uint32_t   e;
    /** Amplitude of the sine harmonic correction term  to argument of latitude (16-bits) */
    uint16_t   Cus;
    /** square-root of the semi-major axis (32-bits) */
    uint32_t   sqrtA;
    /** Reference time of Ephemeris (16-bits) */
    uint16_t   toe;
    /** Fit interval (1-bit) */
    uint8_t    fitIntervalFlag;
    /** Fit interval validity flag */
    bool       isFitIntervalFlagValid;
    /** Amplitude of the cosine harmonic correction term to angle of inclination (16-bits) */
    uint16_t   Cic;
    /** longitude of the ascending node of orbit plane at weekly epoch (32-bits) */
    uint32_t   Omega0;
    /** Amplitude of the sine harmonic correction term to angel of inclination (16-bits) */
    uint16_t   Cis;
    /** inclination angle at reference time (32-bits) */
    uint32_t   i0;
    /** Amplitude of the cosine harmonic correction term to orbit radius (16-bits) */
    uint16_t   Crc;
    /** argument of Perigee (32-bits) */
    uint32_t   omega;
    /** rate of right ascension (24-bits) */
    uint32_t   omegaDot;
    /** Rate of inclination angle (14-bits) */
    uint16_t   idot;
    /** AODO validity flag */
    bool       isAodoValid;
    /** Age of Data offset (5-bits) */
    uint8_t    aodo;
    /** TRUE if the week number specified is valid*/
    bool       isGpsWeekValid;
    /** Unambiguous gps week */
    uint16_t   gpsWeek;
} gpsEphemerisType;

typedef struct {
    /** flags for optional data */
    union {
        uint8_t  filledMask;
        struct {
            uint8_t flagN4      : 1;
            uint8_t flagKp      : 1;
            uint8_t flagNt      : 1;
            uint8_t flagTaoGps  : 1;
            uint8_t flagFreqNum : 1;
            uint8_t flagTaoC    : 1;
            uint8_t             : 2;
        } filledFlagMask;
    } mask;
    /** number of 4-year cycle since 1996, starting from 1 */
    uint8_t  N4;
    /** leap sec change notice, 1: will be changed */
    uint8_t  Kp;
    /** number of days in a 4-year cycle, starting from 1 */
    uint8_t  Nt;
    /** TaoGps: unscaled */
    uint8_t  taoGps;
    /** should be ignored if the type is NOT OTA */
    uint8_t  freqNumber;
    /** Glonass time scale correction to UTC time, for GLO-M */
    uint8_t  taoC;
    /** GLOUTC_SRC_INVALID, GLOUTC_SRC_OTA, GLOUTC_SRC_XTRA,
     *  GLOUTC_SRC_ASSIST */
    uint8_t  src;
} glonassUtcType;

typedef struct {
    /** The GLONASS satellite ID = slot ID + 64
        valid range: 65-88 */
    uint8_t    svId;
    /** flags for optional data */
    union {
        uint8_t  filledMask;
        struct {
            uint8_t  flagGloType  : 1;
            uint8_t  flagTimeSrcP : 1;
            uint8_t  flagFt       : 1;
            uint8_t  flagNt       : 1;
            uint8_t  flagP4       : 1;
            uint8_t  flagLn       : 1;
            uint8_t               : 2;
        }filledFlagMask;
    } mask;
    /** Glonass eph reference time tb: 7-bit LSBs */
    uint8_t   tb;
    /** Glonass-type: 00-GLONASS, 01-GLONASS-M, 02-GLONASS-K */
    uint8_t   gloType;
    /** Indication the satellite operation mode in repect fo time parameters P, 2-bit LSBs */
    uint8_t   timeSrcP;
    /** Health flag Bn: 3-bit LSBs, only check bit 2 */
    uint8_t   Bn;
    /** Time diff between the L2 and L1 transmitters: 5-bit,
        LSB0-LSB3 is data bit, scale factor 2^-30, LSB4 is
        the sign bit, Delat_Tao_n */
    uint8_t   timeDiffL1L2;
    /** Indication of time interval between adjacent eph P1, 2-bit LSBs */
    uint8_t   P1;
    /** Interval indicator P2: 1 bit, LSB */
    uint8_t   P2;
    /** Additional health flag only available on GLONASS-M satellite, 1 bit LSB */
    uint8_t   ln;
    /** Frequency index: 0 ~ 13 */
    uint8_t   freqNumber;
    /** FT is a parameter shown the URE at time tb: 4-bit LSBs */
    uint8_t   Ft;
    /** Acceleration due to lunar-solar gravitational perturbation,
        5-bit LSBs, X/Y/ZDotDot_n */
    uint8_t   acceleration[3];
    /** Calendar number of day within four-year interval starting from the 1st of
        January in a leap year, 11-bit LSBs */
    uint16_t  Nt;
    /** (f(pre)-f(nom))/f(nom), unscaled, 11-bit LSBs, Gamma_n */
    uint16_t  freqOff;
    /** Satellite clock correction: 22-bit LSBs, Tao_n */
    uint32_t  clockCorrection;
    /** Satellite position-xyz, 27-bit LSBs each, X/Y/Zn; */
    uint32_t  satellitePosition[3];
    /** Satellite velocity, 24-bit LSBs each, X/Y/ZDot_n */
    uint32_t  satelliteVelocity[3];
    /** Age of Eph in days, 5-bit LSBs */
    uint8_t   En;
    /** Flag P4,1-bit LSB */
    uint8_t   P4;
} glonassEphemerisType;

typedef struct {
    /** Galileo SV ID, valid range: 301 – 336 */
    uint16_t    svId;
    /** Signal in space accuracy */
    uint8_t     sisa;
    /** Issue of data ephemeris tag(10 bits) */
    uint16_t    iode;
    /** Reference time of Ephemeris (14 bits) [minutes] */
    uint16_t    toe;
    /** Mean anomaly at reference time */
    uint32_t    M0;
    /** Mean motion difference from computed value 16 bits */
    uint16_t    deltaN;
    /** Orbital Eccentricity (32 bits)  */
    uint32_t    e;
    /** square-root of the semi-major axis */
    uint32_t    sqrtA;
    /** Longitude of the ascending node of orbit plane computed according to ref.time (32 bits) */
    uint32_t    Omega0;
    /** Argument of Perigee (32 bits) */
    uint32_t    omega;
    /** Rate of Right Ascension (24 bits) */
    uint32_t    omegaDot;
    /** Inclination angle at reference time (32 bits) */
    uint32_t    i0;
    /** Rate of inclination angle (14 bits) */
    uint16_t    idot;
    /** Amplitude of the cosine harmonic correction term
        to angle of inclination (16 bits) */
    uint16_t    Cic;
    /** Amplitude of the sine harmonic correction term to angel of inclination (16 bits) */
    uint16_t    Cis;
    /** Amplitude of the cosine harmonic correction term to orbit radius (16 bits) */
    uint16_t    Crc;
    /** Amplitude of the sine harmonic correction term to Orbit Radius (16 bits) */
    uint16_t    Crs;
    /** Amplitude of the cosine harmonic correction term to argument of latitude (16 bits) */
    uint16_t    Cuc;
    /** Amplitude of the sine harmonic correction term  to argument of latitude (16 bits) */
    uint16_t    Cus;
    /** week number */
    uint16_t    galileoWeek;
    /** 6 bits for INAV: E5bHS(2-bit), E1bHS(2-bit), E5bDVS(1-bit), E1bDVS(1-bit,LSB)
     *  3 bits for FNAV: E5aHS(2-bit), E5aDVS(1-bit, LSB)
     */
    uint8_t     health;
    /** Time of clock (14 bits) [minutes] */
    uint16_t    toc;
    /** Clock acceleration coefficient (6 bits) */
    uint8_t     af2;
    /** Clock drift coefficient (21 bits) */
    uint32_t    af1;
    /** Clock bias coefficient (31 bits) */
    uint32_t    af0;
    /** BGD (E1, E5a), 10 bits */
    uint16_t    bgdE1E5a;
    /** BGD (E1, E5b), 10 bits */
    uint16_t    bgdE1E5b;
    /** Tgd flag */
    uint16_t    tgd;
    /** TRUE if it is from F/NAV */
    bool        isFnav;
} galileoEphemerisType;

typedef struct {
    /** BDS SV ID, valid range: 201 - 237 */
    uint8_t    svId;
    /** Unambiguous BDS week number */
    uint16_t   bdsWeek;
    /** Reference time of Ephemeris (17-bits)  */
    uint32_t   toe;
    /** Time of clock (17-bits) */
    uint32_t   toc;
    /** Issue of data ephemeris tag (5-bits) */
    uint8_t    iode;
    /* Issue of Data clock (5-bits) */
    uint8_t    iodc;
    /** SV health (1-bit): SatH1 */
    uint8_t    health;
    /** User range accuracy index (4-bits) */
    uint8_t    URAI;
    /** Square-root of the semi-major axis (32-bits) */
    uint32_t   sqrtA;
    /** Orbital eccentricity (32-bits) */
    uint32_t   e;
    /** Argument of perigee (32-bits) */
    uint32_t   omega;
    /** Mean motion difference from computed value (16-bits) */
    uint16_t   deltaN;
    /** Mean anomaly at reference time (32-bits) */
    uint32_t   M0;
    /** Longitude of the ascending node of orbit plane computed according to ref. time
        (32-bits) */
    uint32_t   omega0;
    /** Rate of right ascension (24-bits) */
    uint32_t   omegaDot;
    /** Inclination angle at reference time (32-bits) */
    uint32_t   i0;
    /** Rate of inclination angle (14-bits) */
    uint16_t   idot;
    /** Amplitude of the sine harmonic correction term to Orbit Radius (18-bits) */
    uint32_t   Crs;
    /** Amplitude of the cosine harmonic correction term to argument of latitude (18-bits) */
    uint32_t   Cuc;
    /** Amplitude of the sine harmonic correction term  to argument of latitude (18-bits) */
    uint32_t   Cus;
    /** Amplitude of the cosine harmonic correction term to angle of inclination (18-bits) */
    uint32_t   Cic;
    /** Amplitude of the sine harmonic correction term to angel of inclination (18-bits) */
    uint32_t   Cis;
    /** Amplitude of the cosine harmonic correction term to orbit radius (18-bits) */
    uint32_t   Crc;
    /** Clock acceleration coefficient (11-bits) */
    uint16_t   a2;
    /** Clock drift coefficient (22-bits) */
    uint32_t   a1;
    /** Clock bias coefficient (24-bits) */
    uint32_t   a0;
    /** Time of group delay (10-bits) */
    uint16_t   tgd;
} bdsEphemerisType;

typedef struct {
    /** single band BDS raw ephemeris */
    bdsEphemerisType bdsEph;
    /** Tgd2 (4-bits) in D1, 10-bits in D2 */
    uint16_t         tgd2;
    /** Tgd2 validity flag */
    bool             isTgd2Valid;
} bdsMbRawEphemerisType;

typedef struct {
    /** PRN of broadcasting GEO satellite */
    uint16_t   geoPrn;
    /** issue of data GEO */
    uint8_t    iodg;
    /** user range accuracy */
    uint8_t    ura;
    /** X of GEO (ECEF) meters */
    float      xg;
    /** Y of GEO (ECEF) meters */
    float      yg;
    /** Z of GEO (ECEF) meters */
    float      zg;
    /** X of GEO rate-of-change meters/sec */
    float      xgDot;
    /** Y of GEO rate-of-change meters/sec */
    float      ygDot;
    /** Z of GEO rate-of-change meters/sec */
    float      zgDot;
    /** X of GEO acceleration meters/sec^2 */
    float      xgAcceleration;
    /** Y of GEO acceleration meters/sec^2 */
    float      ygAcceleration;
    /** Z of GEO acceleration meters/sec^2 */
    float      zgAcceleration;
    /** Clock offset of GEO seconds */
    double     clockOffset;
    /** Clock drift of GEO seconds/sec */
    double     clockDrift;
    /** Time of applicability */
    uint32_t   timeOfApplicability;
    /** Validity of GEO ephemeris */
    uint8_t    valid;
    /** Cumulative GPS time of the update */
    double     updateTimeMsec;
} sbasGeoEphemerisType;

/** 6.2 Observation availability indicator */

/** Table 14: Assistance data to message ID mapping (RTCM message 1013, 3GPP posSIBType)*/
/** Assistance data to message ID mapping
    Value  RTCM message ID                           3GPP Message    3GPP Message
                                                     (posSIBType)    (assistanceDataElement)
    ------------------------------------------------------------------------------------------------
    0      1005, 1006, 1032                          posSibType1-5   GNSS-RTK-ReferenceStationInfo
    1      1001-1004, 1009-1012, MSM 1-7             posSibType1-6
                                                     posSibType2-12  GNSS-RTK-CommonObservationInfo,
                                                                     GNSS-RTK-Observations
    2      1230                                      posSibType2-13  GLO-RTK-BiasInformation
    3      1057,1060, 1063, 1066                     posSibType2-17  GNSS-SSR-OrbitCorrections
    4      1058,1060, 1064, 1066                     posSibType2-18  GNSS-SSR-ClockCorrections
    5      1062, 1068 (High rate clock corrections)  N/A             N/A
    6      1059, 1065                                posSibType2-19  GNSS-SSR-CodeBias
    7      1061, 1067                                posSibType2-20  GNSS-SSR-URA
    8      N/A                                       posSibType2-21  GNSS-SSR-PhaseBias
    9      N/A                                       posSibType2-22  GNSS-SSR-STEC-Correction
    10     N/A                                       posSibType2-23  GNSS-SSR-GriddedCorrection
    11     N/A                                       posSibType1-8   GNSS-SSR-CorrectionPoints
    12-255 Reserved                                  Reserved        Reserved
*/

/** Unique identifier to which the following message
    broadcast is applicable (ex. RSID for RTCM, NCGI for 3GPP)
    valid range: RTCM 0 - 4095, 3GPP N/A
    RTCM: 1013, DF003
    3GPP: NCGI-r15 encoded as
    [b9…b0] = mcc-r15
    [b19…b10] = mnc-r15
    [b55…b20] = nr-cellidentity-r15
    [b63…b56] = [0 … 0] */
typedef uint64_t DGnssObserAvailIndIdentifier;

/**   Table 13: Observation availability indicator */
typedef struct {
    /** assistance data to message ID mapping */
    uint16_t                        messageID;
    /** True = synchronous broadcast
        False = asynchronous broadcast */
    bool                            syncFlag;
    /** Message transmission interval
        (approximate value for asynchronous transmission)
        unit: 10ms,
        valid range: [0-6,553.5) s,  Set to 655.35 = Not Available */
    uint16_t                        transmissionInterval;
    /** Indicates whether the observation availability provided
        in this packet is constellation specific */
    bool                            isConstellationSpecific;
    /** DGnss constellation identifier,
        Applicable only when isConstellationSpecific = true */
    DGnssConstellationIdentifier    constellationIdentifier;
} DGnssAvailMessageType;

typedef struct {
    /** Unique identifier for this source */
    DGnssObserAvailIndIdentifier    uID;

    /** GNSS epoch time */
    GnssEpochTime                   epochTime;
    /** Count of unique correction data message types supported/generated from this source
        valid range: 1-255 */
    uint8_t                         numberOfMessage;
    DGnssAvailMessageType           dGnssAvailMessages[DGNSS_AVAILABILITY_MESSAGE_MAX];
} DGnssObserAvailIndicator;

/** 6.6.7 SSR phase bias */
/** Table 33: Satellite specific SSR carrier phase bias definition (3GPP: GNSS-SSR-PhaseBias) */
typedef struct {
    /** GNSS signal identifier */
    union {
        DGnssGpsSignalIdentifier     gpsSignalIdentifier;
        DGnssGlonassSignalIdentifier glonassSignalIdentifier;
        DGnssGalileoSignalIdentifier galileoSignalIdentifier;
        DGnssQzssSignalIdentifier    qzssSignalIdentifier;
        DGnssBdsSignalIdentifier     bdsSignalIdentifier;
        DGnssSbasSignalIdentifier    sbasSignalIdentifier;
    } gnssSignalIdentifier;
    /** valid range +/- 16.383 mm,
        Phase bias is represented in meters with a precision of 1 mm
        If Phase bias value exceeds the range specified,
        it shall be dropped. */
    float       phaseBias;
    /** valid range 0-3,
        A modulo-4 phase discontinuity counter for the GNSS signal identified,
        which is incremented for every discontinuity in phase. */
    uint8_t     phaseDiscontinuityIndicator;
    /** valid range 0-3,
        0 = Undifferenced integer phase. T
            his is the default value, when this field is not present in the input
        1 = Widelane integer,
        2 = Non-integer,
        3 = reserved
    */
    uint8_t     phaseBiasIntegerIndicator;
} DGnssPhaseBiasPerSatellite;

typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t     svId;
    uint8_t     numberOfPhaseBias;
    DGnssPhaseBiasPerSatellite  phaseBias[DGNSS_PHASE_BIAS_MAX];
} DGnssPhaseBiasPerSv;

typedef struct {
    SsrHeader  ssrHeader;
    DGnssPhaseBiasPerSv  phaseBiasPerSv[DGNSS_SATELLITE_MAX];
} SsrPhaseBias;

/** 6.6.8 SSR Atmospheric corrections
    Note: SSR atmospheric corrections does not have a common header. */
/** 6.6.8.1 SSR grid points for atmospheric corrections
Table 34: Grid points or correction point coordinates */
typedef struct {
    /** unit: degree, valide range: [-90, +90) */
    float       latitude;
    /** unit: degree, valide range: [-180, +180) */
    float       longitude;
    /** default is set to true */
    bool        isCorrectionAvailable;
} DGnssGridPointCoordinates;

typedef struct {
    /** SSR provider & solution ID availability */
    bool        isProviderAvailable;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t    providerId;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t     solutionId;
    /** valid range: 0 – 16383 */
    uint16_t    pointSetIdentifier;
    /** valid range: 0 – 64 */
    uint8_t     numberOfGridPoints;
    DGnssGridPointCoordinates gridPointsCoordinates[DGNSS_SSR_GRID_POINTS_MAX];
} DGnssGridPoints;

/** 6.6.8.2 SSR ionosphere slant delay (STEC) correction
    Table 35: Ionosphere slant delay (STEC) polynomial coefficients */
typedef struct {
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t     svId;
    /** unit 0.0001 TECU
        valid range: [0 – 33.6664]
        0 = Unknown / Undefined
        Value larger than 33.6664 TECU may not be reliable
    */
    uint32_t    qualityIndicator;
    /** 0 = Only C00 is available
        1 = C00, C01, C10 are available
        2 = C00, C01, C10, and C11 are available
        3 = C00, C01, C10, C11, C02, and C20 are available
        STEC correction type 3 is not supported in 3GPP-R16 */
    uint8_t     correctionType;
    /** unit 1 TECU
        valid range: +/-409.55 TECU
        Included for all STEC correction types */
    float       c00;
    /** unit 1 TECU/deg
        valid range: +/-40.94 TECU/deg
        Included for STEC correction types 1, 2 and 3 */
    float       c01;
    /** unit 1 TECU/deg
        valid range: +/-40.94 TECU/deg
        Included for STEC correction types 1, 2 and 3 */
    float       c10;
    /** unit 1 TECU/deg^2
        valid range: +/-10.22 TECU/deg^2
        Included for STEC correction types 2 and 3 */
    float       c11;
    /** unit 1 TECU/deg^2
        valid range: +/-0.635 TECU/deg^2
        Included for STEC correction types 3 */
    float       c02;
    /** unit 1 TECU/deg^2
        valid range: +/-0.635 TECU/deg^2
        Included for STEC correction types 3 */
    float       c20;
} DGnssStecPolyCoefficients;

typedef struct {
    /** DGnss constellation identifier */
    DGnssConstellationIdentifier   constellationIdentifier;
    /** GNSS epoch time */
    GnssEpochTime                  epochTime;
    /** valid Range: 0 - 15 */
    uint8_t                        ssrUpdateInterval;
    /** valid Range: 0 - 15 */
    uint8_t                        issueOfSsrData;
    /** SSR provider & solution ID availability */
    bool                           isProviderAvailable;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t                       providerId;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t                        solutionId;
    /** valid Range: 0 - 16383 */
    uint16_t                       setIdentifier;
    /** valid Range: 1 - 64,
    Number of satellites for which STEC
    polynomial coefficients are provided*/
    uint8_t                        numberOfSatellites;
    DGnssStecPolyCoefficients      polyCofficients[DGNSS_SATELLITE_MAX];
} DGnssStecCorrection;

/** Table 36: Gridded corrections – STEC residual */
typedef struct {
    /** Number of satellites for which STEC residual correction is
        available for this grid point - i */
    uint8_t                        numberOfSatellites;
    /** GPS SV ID, valid range: 1-32
     *  GLONASS SV ID, valid range: 1-24
     *  GALILEO SV ID, valid range: 1-36
     *  QZSS SV ID, valid range: 193-202
     *  BeiDou SV ID, valid range: 1-63
     *  SBAS SV ID, range:120-158
     */
    uint8_t     svId[DGNSS_SATELLITE_MAX];
    /** unit: 1 TECU, valid range: +/- 1310.68 TECU */
    float       stecResidualCorrection[DGNSS_SATELLITE_MAX];
} DGnssSatStecResiduals;

typedef struct {
    /** DGnss constellation identifier */
    DGnssConstellationIdentifier   constellationIdentifier;
    /** GNSS epoch time */
    GnssEpochTime                  epochTime;
    /** valid Range: 0 - 15 */
    uint8_t                        ssrUpdateInterval;
    /** valid Range: 0 - 15 */
    uint8_t                        issueOfSsrData;
    /** SSR provider & solution ID availability */
    bool                           isProviderAvailable;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t                       providerId;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t                        solutionId;
    /** valid Range: 0 - 16383 */
    uint16_t                       setIdentifier;
    /** valid Range: 1 - 64,
        Number of grid points at which STEC residuals are provided */
    uint8_t                        numberOfGridPoints;
    /** Satellite STEC residuals for each grid points */
    DGnssSatStecResiduals          satStecResiduals[DGNSS_SSR_GRID_POINTS_MAX];
} DGnssSTECResidualCorrection;

/** 6.6.8.3 SSR tropospheric delay correction
Table 38: Gridded corrections – Tropospheric correction */
typedef struct {
    /** unit: 1m, Valid Range: +/- 1.02m,
        The target device should add the constant nominal value of 2.3 m to
        calculate the tropospheric hydro-static vertical delay. */
    float hydrostaticVerticalDelay;
    /** unit: 1m, Valid Range: +/- 0.508m,
        The target device should add the constant value of 0.252 m to
        calculate the tropospheric wet (non hydro-static) vertical delay. */
    float wetVerticalDelay;
} DGnssTroposphericDelay;

typedef struct {
    /** DGnss constellation identifier */
    DGnssConstellationIdentifier   constellationIdentifier;
    /** GNSS epoch time */
    GnssEpochTime                  epochTime;
    /** Valid Range: 0 - 15 */
    uint8_t                        ssrUpdateInterval;
    /** Valid Range: 0 - 15 */
    uint8_t                        issueOfSsrData;
    /** unit: 1mm, Valid Range: 0 – 6013.25 mm
        Provides the quality of SSR Tropospheric delay corrections
        0 = unknown accuracy
        Non-zero and ≤ 5466.5 mm – valid range
        Larger than 5466.5 mm = SSR corrections may not reliable */
    float                          delayQualityIndicator;
    /** SSR provider & solution ID availability */
    bool                           isProviderAvailable;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t                       providerId;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t                        solutionId;
    /** Valid Range: 0 - 16383 */
    uint16_t                       setIdentifier;
    /** Valid Range: 1 - 64 */
    uint8_t                        numberOfTropoDelay;
    DGnssTroposphericDelay         tropoDelays[DGNSS_SSR_GRID_POINTS_MAX];
} DGnssTroposphericCorrection;

#endif
