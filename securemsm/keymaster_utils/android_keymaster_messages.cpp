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

#include <android_keymaster_messages.h>
#include <android_keymaster_utils.h>

namespace keymaster {

namespace {

/*
 * Helper functions for working with key blobs.
 */

uint8_t* serialize_key_blob(const keymaster_key_blob_t& key_blob, uint8_t* buf,
                            const uint8_t* end) {
    return append_size_and_data_to_buf(buf, end, key_blob.key_material, key_blob.key_material_size);
}

bool deserialize_key_blob(keymaster_key_blob_t* key_blob, const uint8_t** buf_ptr,
                          const uint8_t* end) {
    delete[] key_blob->key_material;
    key_blob->key_material = nullptr;
    UniquePtr<uint8_t[]> deserialized_key_material;
    if (!copy_size_and_data_from_buf(buf_ptr, end, &key_blob->key_material_size,
                                     &deserialized_key_material))
        return false;
    key_blob->key_material = deserialized_key_material.release();
    return true;
}

void set_blob(keymaster_blob_t* blob, const void* data, size_t length) {
    assert(blob);
    assert(data);
    delete[] blob->data;
    blob->data = dup_buffer(data, length);
    blob->data_length = length;
}

size_t blob_size(const keymaster_blob_t& blob) {
    return sizeof(uint32_t) /* data size */ + blob.data_length;
}

uint8_t* serialize_blob(const keymaster_blob_t& blob, uint8_t* buf, const uint8_t* end) {
    return append_size_and_data_to_buf(buf, end, blob.data, blob.data_length);
}

bool deserialize_blob(keymaster_blob_t* blob, const uint8_t** buf_ptr, const uint8_t* end) {
    delete[] blob->data;
    *blob = {};
    UniquePtr<uint8_t[]> deserialized_blob;
    if (!copy_size_and_data_from_buf(buf_ptr, end, &blob->data_length, &deserialized_blob))
        return false;
    blob->data = deserialized_blob.release();
    return true;
}


}  // namespace

size_t KeymasterResponse::SerializedSize() const {
    if (error != KM_ERROR_OK)
        return sizeof(int32_t);
    else
        return sizeof(int32_t) + NonErrorSerializedSize();
}

uint8_t* KeymasterResponse::Serialize(uint8_t* buf, const uint8_t* end) const {
    buf = append_uint32_to_buf(buf, end, static_cast<uint32_t>(error));
    if (error == KM_ERROR_OK) buf = NonErrorSerialize(buf, end);
    return buf;
}

bool KeymasterResponse::Deserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    if (!copy_uint32_from_buf(buf_ptr, end, &error)) return false;
    if (error != KM_ERROR_OK) return true;
    return NonErrorDeserialize(buf_ptr, end);
}

uint8_t* GenerateRkpKeyResponse::NonErrorSerialize(uint8_t* buf, const uint8_t* end) const {
    buf = serialize_key_blob(key_blob, buf, end);
    return serialize_blob(maced_public_key, buf, end);
}

bool GenerateRkpKeyResponse::NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    return deserialize_key_blob(&key_blob, buf_ptr, end) &&
           deserialize_blob(&maced_public_key, buf_ptr, end);
}

size_t GenerateCsrRequest::SerializedSize() const {
    size_t size = sizeof(uint8_t); /* test_mode */
    size += sizeof(uint32_t);      /* num_keys */
    for (size_t i = 0; i < num_keys; i++) {
        size += blob_size(keys_to_sign_array[i]);
    }
    size += blob_size(endpoint_enc_cert_chain);
    size += blob_size(challenge);
    return size;
}

uint8_t* GenerateCsrRequest::Serialize(uint8_t* buf, const uint8_t* end) const {
    buf = append_to_buf(buf, end, &test_mode, sizeof(uint8_t));
    buf = append_uint32_to_buf(buf, end, num_keys);
    for (size_t i = 0; i < num_keys; i++) {
        buf = serialize_blob(keys_to_sign_array[i], buf, end);
    }
    buf = serialize_blob(endpoint_enc_cert_chain, buf, end);
    return serialize_blob(challenge, buf, end);
}

bool GenerateCsrRequest::Deserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    if (!copy_from_buf(buf_ptr, end, &test_mode, sizeof(uint8_t)) ||
        !copy_from_buf(buf_ptr, end, &num_keys, sizeof(uint32_t))) {
        return false;
    }
    keys_to_sign_array = new (std::nothrow) KeymasterBlob[num_keys];
    if (!keys_to_sign_array) return false;
    for (size_t i = 0; i < num_keys; i++) {
        if (!deserialize_blob(&keys_to_sign_array[i], buf_ptr, end)) return false;
    }
    return deserialize_blob(&endpoint_enc_cert_chain, buf_ptr, end) &&
           deserialize_blob(&challenge, buf_ptr, end);
}

void GenerateCsrRequest::SetKeyToSign(uint32_t index, const void* data, size_t length) {
    if (index >= num_keys) {
        return;
    }
    set_blob(&keys_to_sign_array[index], data, length);
}

void GenerateCsrRequest::SetEndpointEncCertChain(const void* data, size_t length) {
    set_blob(&endpoint_enc_cert_chain, data, length);
}

void GenerateCsrRequest::SetChallenge(const void* data, size_t length) {
    set_blob(&challenge, data, length);
}

size_t GenerateCsrResponse::NonErrorSerializedSize() const {
    return blob_size(keys_to_sign_mac) + blob_size(device_info_blob) +
           blob_size(protected_data_blob);
}

uint8_t* GenerateCsrResponse::NonErrorSerialize(uint8_t* buf, const uint8_t* end) const {
    buf = serialize_blob(keys_to_sign_mac, buf, end);
    buf = serialize_blob(device_info_blob, buf, end);
    return serialize_blob(protected_data_blob, buf, end);
}

bool GenerateCsrResponse::NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    return deserialize_blob(&keys_to_sign_mac, buf_ptr, end) &&
           deserialize_blob(&device_info_blob, buf_ptr, end) &&
           deserialize_blob(&protected_data_blob, buf_ptr, end);
}

size_t GenerateCsrV2Request::SerializedSize() const {
    size_t size = sizeof(uint32_t); /* num_keys */
    for (size_t i = 0; i < num_keys; i++) {
        size += blob_size(keys_to_sign_array[i]);
    }
    size += blob_size(challenge);
    return size;
}

uint8_t* GenerateCsrV2Request::Serialize(uint8_t* buf, const uint8_t* end) const {
    buf = append_uint32_to_buf(buf, end, num_keys);
    for (size_t i = 0; i < num_keys; i++) {
        buf = serialize_blob(keys_to_sign_array[i], buf, end);
    }
    return serialize_blob(challenge, buf, end);
}

bool GenerateCsrV2Request::Deserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    if (!copy_from_buf(buf_ptr, end, &num_keys, sizeof(uint32_t))) return false;

    keys_to_sign_array = new (std::nothrow) KeymasterBlob[num_keys];
    if (!keys_to_sign_array) return false;
    for (size_t i = 0; i < num_keys; i++) {
        if (!deserialize_blob(&keys_to_sign_array[i], buf_ptr, end)) return false;
    }
    return deserialize_blob(&challenge, buf_ptr, end);
}

bool GenerateCsrV2Request::InitKeysToSign(uint32_t count) {
    num_keys = count;
    keys_to_sign_array = new (std::nothrow) KeymasterBlob[count];
    if (!keys_to_sign_array) {
        return false;
    }
    return true;
}

void GenerateCsrV2Request::SetKeyToSign(uint32_t index, const void* data, size_t length) {
    if (index >= num_keys) {
        return;
    }
    set_blob(&keys_to_sign_array[index], data, length);
}

void GenerateCsrV2Request::SetChallenge(const void* data, size_t length) {
    set_blob(&challenge, data, length);
}

size_t GenerateCsrV2Response::NonErrorSerializedSize() const {
    return blob_size(csr);
}

uint8_t* GenerateCsrV2Response::NonErrorSerialize(uint8_t* buf, const uint8_t* end) const {
    return serialize_blob(csr, buf, end);
}

bool GenerateCsrV2Response::NonErrorDeserialize(const uint8_t** buf_ptr, const uint8_t* end) {
    return deserialize_blob(&csr, buf_ptr, end);
}

}  // namespace keymaster