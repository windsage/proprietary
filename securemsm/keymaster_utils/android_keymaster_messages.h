/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a contribution.
 *
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <android_keymaster_utils.h>
#include <authorization_set.h>
#include <km_version.h>

namespace keymaster {

// Commands
enum AndroidKeymasterCommand : uint32_t {
    GENERATE_KEY = 0,
    BEGIN_OPERATION = 1,
    UPDATE_OPERATION = 2,
    FINISH_OPERATION = 3,
    ABORT_OPERATION = 4,
    IMPORT_KEY = 5,
    EXPORT_KEY = 6,
    GET_VERSION = 7,
    ADD_RNG_ENTROPY = 8,
    GET_SUPPORTED_ALGORITHMS = 9,
    GET_SUPPORTED_BLOCK_MODES = 10,
    GET_SUPPORTED_PADDING_MODES = 11,
    GET_SUPPORTED_DIGESTS = 12,
    GET_SUPPORTED_IMPORT_FORMATS = 13,
    GET_SUPPORTED_EXPORT_FORMATS = 14,
    GET_KEY_CHARACTERISTICS = 15,
    ATTEST_KEY = 16,
    UPGRADE_KEY = 17,
    CONFIGURE = 18,
    GET_HMAC_SHARING_PARAMETERS = 19,
    COMPUTE_SHARED_HMAC = 20,
    VERIFY_AUTHORIZATION = 21,
    DELETE_KEY = 22,
    DELETE_ALL_KEYS = 23,
    DESTROY_ATTESTATION_IDS = 24,
    IMPORT_WRAPPED_KEY = 25,
    EARLY_BOOT_ENDED = 26,
    DEVICE_LOCKED = 27,
    GET_VERSION_2 = 28,
    GENERATE_RKP_KEY = 29,
    GENERATE_CSR = 30,
    GENERATE_TIMESTAMP_TOKEN = 31,
};

/**
 * Keymaster message versions are tied to keymaster versions.  We map the keymaster version to a
 * sequential "message version".  The actual message formatting differences are implemented in the
 * message classes. Note that it is not necessary to increase the message version when new messages
 * are added, only when the serialized format of one or more messages changes.  A message version
 * argument is provided to the message constructor and when the serialization/deserialization
 * methods are called the implementations of those methods should examine the message version and
 * generate/parse the byte stream accordingly.
 *
 * The original design of message versioning uses the GetVersion message, sent from client (e.g. HAL
 * service) to server (e.g. trusted app), and then relies on the client to identify what messages to
 * send.  This architecture assumes that the client is never older than the server.  This assumption
 * turned out not to be true in general.
 *
 * The current approach performs a mutual exchange of message version info between client and
 * server, using the GetVersion2 message.  In addition, it defers the specification of the message
 * ID to the message classes, so a message class can use a different ID when necessary.  ID changes
 * should be rare, in fact the only time they should be required is during the switch from
 * GetVersion to GetVersion2.
 *
 * Assuming both client and server support GetVersion2, the approach is as follows:
 *
 * 1.  Client sends GetVersion2Request, containing its maximum message version, c_max.
 * 2.  Server replies with GetVersion2Response, containing its maximum message version, s_max.
 * 3.  Both sides proceed to create all messages with version min(c_max, s_max).
 *
 * To enable this, the client must always send GetVersion2 as its first message.  If the server
 * doesn't support GetVersion2, it will reply with an error of some sort (the details are likely
 * environment-specific).  If the client gets this error, it must respond by sending GetVersion, and
 * then must configure its message version according to the response.  Note that an acceptable
 * response to a too-old server version is to return an error to the caller of the client, informing
 * it of the problem.
 *
 * On the server side, a server that supports GetVersion2 must also support GetVersion.  If it
 * received GetVersion2 it should proceed as outline above, and expect that the client will not send
 * GetVersion.  If it received GetVersion, it must assume that the client does not support
 * GetVersion2 and reply that it is version 2.0.0 and use the corresponding message version (3).
 */
constexpr int32_t kInvalidMessageVersion = -1;
constexpr int32_t kMaxMessageVersion = 4;
constexpr int32_t kDefaultMessageVersion = 3;

/**
 * MessageVersion returns the message version for a specified KM version and, possibly, KM release
 * date in YYYYMMDD format (it's not recommended to change message formats within a KM version, but
 * it could happen).
 */
inline constexpr int32_t MessageVersion(KmVersion version, uint32_t /* km_date */ = 0) {
    switch (version) {
    case KmVersion::KEYMASTER_1:
        return 1;
    case KmVersion::KEYMASTER_1_1:
        return 2;
    case KmVersion::KEYMASTER_2:
    case KmVersion::KEYMASTER_3:
    case KmVersion::KEYMASTER_4:
    case KmVersion::KEYMASTER_4_1:
        return 3;
    case KmVersion::KEYMINT_1:
        return 4;
    }
    return kInvalidMessageVersion;
}

struct KeymasterMessage : public Serializable {
    explicit KeymasterMessage(int32_t ver) : message_version(ver) { assert(ver >= 0); }

    // The message version that should be used for this message.  This indicates how the data is
    // serialized/deserialized. Commonly, higher message versions serialize/deserialize additional
    // arguments, though there is no specific rule limiting later version to adding parameters.
    const int32_t message_version;
};

/**
 * All responses include an error value, and if the error is not KM_ERROR_OK, return no additional
 * data.
 */
struct KeymasterResponse : public KeymasterMessage {
    explicit KeymasterResponse(int32_t ver)
        : KeymasterMessage(ver), error(KM_ERROR_UNKNOWN_ERROR) {}

    size_t SerializedSize() const override;
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override;
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override;

    virtual size_t NonErrorSerializedSize() const = 0;
    virtual uint8_t* NonErrorSerialize(uint8_t* buf, const uint8_t* end) const = 0;
    virtual bool NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) = 0;

    keymaster_error_t error;
};

// Abstract base for empty requests.
struct EmptyKeymasterRequest : public KeymasterMessage {
    explicit EmptyKeymasterRequest(int32_t ver) : KeymasterMessage(ver) {}

    size_t SerializedSize() const override { return 0; }
    uint8_t* Serialize(uint8_t* buf, const uint8_t*) const override { return buf; }
    bool Deserialize(const uint8_t**, const uint8_t*) override { return true; };
};

// Empty response.
struct EmptyKeymasterResponse : public KeymasterResponse {
    explicit EmptyKeymasterResponse(int32_t ver) : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override { return 0; }
    uint8_t* NonErrorSerialize(uint8_t* buf, const uint8_t*) const override { return buf; }
    bool NonErrorDeserialize(const uint8_t**, const uint8_t*) override { return true; }
};

struct GenerateRkpKeyRequest : KeymasterMessage {
    explicit GenerateRkpKeyRequest(int32_t ver) : KeymasterMessage(ver) {}

    size_t SerializedSize() const override { return sizeof(uint8_t); }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        return append_to_buf(buf, end, &test_mode, sizeof(uint8_t));
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return copy_from_buf(buf_ptr, end, &test_mode, sizeof(uint8_t));
    }

    bool test_mode = false;
};

struct GenerateRkpKeyResponse : public KeymasterResponse {
    explicit GenerateRkpKeyResponse(int32_t ver) : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override;
    uint8_t* NonErrorSerialize(uint8_t* buf, const uint8_t* end) const override;
    bool NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) override;

    KeymasterKeyBlob key_blob;
    KeymasterBlob maced_public_key;
};

struct GenerateCsrRequest : public KeymasterMessage {
    explicit GenerateCsrRequest(int32_t ver) : KeymasterMessage(ver) {}

    ~GenerateCsrRequest() override { delete[] keys_to_sign_array; }

    size_t SerializedSize() const override;
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override;
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override;
    void SetKeyToSign(uint32_t index, const void* data, size_t length);
    void SetEndpointEncCertChain(const void* data, size_t length);
    void SetChallenge(const void* data, size_t length);

    bool test_mode = false;
    size_t num_keys = 0;
    KeymasterBlob* keys_to_sign_array;
    KeymasterBlob endpoint_enc_cert_chain;
    KeymasterBlob challenge;
};

struct GenerateCsrResponse : public KeymasterResponse {
    explicit GenerateCsrResponse(int32_t ver) : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override;
    uint8_t* NonErrorSerialize(uint8_t* buf, const uint8_t* end) const override;
    bool NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) override;

    KeymasterBlob keys_to_sign_mac;
    KeymasterBlob device_info_blob;
    KeymasterBlob protected_data_blob;
};

struct GenerateCsrV2Request : public KeymasterMessage {
    explicit GenerateCsrV2Request(int32_t ver) : KeymasterMessage(ver) {}

    ~GenerateCsrV2Request() override { delete[] keys_to_sign_array; }

    size_t SerializedSize() const override;
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override;
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override;
    bool InitKeysToSign(uint32_t count);
    void SetKeyToSign(uint32_t index, const void* data, size_t length);
    void SetChallenge(const void* data, size_t length);

    uint32_t num_keys = 0;
    KeymasterBlob* keys_to_sign_array = nullptr;
    KeymasterBlob challenge;
};

struct GenerateCsrV2Response : public KeymasterResponse {
    explicit GenerateCsrV2Response(int32_t ver) : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override;
    uint8_t* NonErrorSerialize(uint8_t* buf, const uint8_t* end) const override;
    bool NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) override;

    KeymasterBlob csr;
};

}  // namespace keymaster