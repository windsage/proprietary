/***********************************************************************
 * Copyright (c) 2023-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#pragma once

#include "MinkTypes.h"
#include "object.h"
#include "vmuuid.h"

#if defined (__cplusplus)
extern "C" {
#endif

#define clockSeqAndNodeSize 8

typedef struct uuid_t uuid_t;

/**
 * Description: Generates an ICredentials object for the process
 *
 * In:          permissions: uint64_t bitmask
 *              appName: Application name (optional)
 *              appHash: SHA256 of the application binary (optional)
 *              appDebug: debug value (optional)
 *              appVersion: application version (optional)
 *              legacyCBOR: client credentials as a CBOR blob (optional)
 *              legacyCBORCredentials_len: Length of CBOR blob (optional)
 *              domain: qti,oem,alt,isv,unk (current "domain" in ICredentials)
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: ICredentials Object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newProcessCred(
    uint64_t permissions, const char *appName, SHA256Hash *appHash,
    uint32_t appDebug, uint32_t appVersion, uint8_t *legacyCBOR,
    uint32_t legacyCBORCredentials_len, const char *domain, uint32_t pid,
    uint32_t uid, Object *outCredentials);

/**
 * Description: Generates an ICredentials object for the environment
 *
 * In:          osID: operating system ID (optional)
 *              vmUUID: uuid for the VM
 *              vmHash: SHA256 of the VM binary (optional)
 *              vmDomain: qti,oem,alt,isv,unk (current "domain" in ICredentials)
 *              vmVersion: version number (optional)
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newEnvCred(const char *osID, uint8_t *vmUUID,
                                    uint8_t *vmHash, const char *vmDomain,
                                    uint32_t vmVersion, Object *outCredentials);

/**
 * Description: Generates an ICredentials object that wraps the proc and env
 *              credentials
 *
 * In:          procCred: The process ICredentials object
 *              envCred: The env ICredentials
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_WrapCredentials(Object *procCred, Object *envCred,
                                         Object *outCredentials);

/**
 * Description: Generates an ICredentials object for local vendor clients
 *
 * In:          appName: Application name
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newLAVendor(const char *appName, uint32_t pid,
                                     uint32_t uid, Object *outCredentials);

/**
 * Description: Generates an ICredentials object for APK clients
 *
 * In:          appName: Application name
 *              appHash: SHA256 of the application binary
 *              appDebug: uint32_t
 *              appVersion: uint32_t
 *              legacyCBOR: client credentials as a CBOR blob
 *              legacyCBORCredentials_len: Length of CBOR blob
 *              domain: qti,oem,alt,isv,unk (current "domain" in ICredentials)
 *              pid: process id
 *              uid: user id
 *
 * Out:         outCredentials: an ICredentials object
 *
 * Return:      Object_OK on success.
 *              All else on failure.
 */
int32_t OSIndCredentials_newLASystem(const char *appName, SHA256Hash *appHash,
                                     uint32_t appDebug, uint32_t appVersion,
                                     uint8_t *legacyCBOR,
                                     uint32_t legacyCBORCredentials_len,
                                     const char *domain, uint32_t pid,
                                     uint32_t uid, uint64_t permissions,
                                     Object *outCredentials);

#if defined (__cplusplus)
}
#endif
