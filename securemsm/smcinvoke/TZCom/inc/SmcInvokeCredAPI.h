/*
 * Copyright (c) 2017,2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef __SMCINVOKECREDAPI_H_
#define __SMCINVOKECREDAPI_H_

typedef enum {
    Success = 0,
    UnAuthorisedUser,
    InvalidArguments,
    DeathNotification,
    PkgNameNotFound,
    CertificateErr,
    ConnectionErr,
    DataConversionErr,
    OOBErr,
    OOMErr,
    InsufficientBufferErr,
    BufferOverFlowErr,
    EncodingErr,
} smcInvokeCred_ErrorCode;

typedef enum {
    AttrUid = 1,
    AttrPkgFlags,
    AttrPkgName,
    AttrPkgCert,
    AttrPermissions,
    AttrSystemTime
} smcInvokeCred_Attr;

#endif
