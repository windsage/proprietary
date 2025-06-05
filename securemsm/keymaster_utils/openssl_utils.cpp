/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a contribution.
 *
 * Copyright 2015 The Android Open Source Project
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

#include <openssl_utils.h>
#include <openssl_err.h>
#include <android_keymaster_utils.h>

#include <openssl/rand.h>

namespace keymaster {

constexpr uint32_t kAffinePointLength = 32;

keymaster_error_t GenerateRandom(uint8_t* buf, size_t length) {
    if (RAND_bytes(buf, length) != 1) return KM_ERROR_UNKNOWN_ERROR;
    return KM_ERROR_OK;
}

// Remote provisioning helper function
keymaster_error_t GetEcdsa256KeyFromCert(const keymaster_blob_t* km_cert, uint8_t* x_coord,
                                         size_t x_length, uint8_t* y_coord, size_t y_length) {
    if (km_cert == nullptr || x_coord == nullptr || y_coord == nullptr) {
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }
    if (x_length != kAffinePointLength || y_length != kAffinePointLength) {
        return KM_ERROR_INVALID_ARGUMENT;
    }
    const uint8_t* temp = km_cert->data;
    X509* cert = d2i_X509(NULL, &temp, km_cert->data_length);
    if (cert == nullptr) return TranslateLastOpenSslError();
    EVP_PKEY* pubKey = X509_get_pubkey(cert);
    if (pubKey == nullptr) return TranslateLastOpenSslError();
    EC_KEY* ecKey = EVP_PKEY_get0_EC_KEY(pubKey);
    if (ecKey == nullptr) return TranslateLastOpenSslError();
    const EC_POINT* jacobian_coords = EC_KEY_get0_public_key(ecKey);
    if (jacobian_coords == nullptr) return TranslateLastOpenSslError();
    BIGNUM x;
    BIGNUM y;
    BN_CTX* ctx = BN_CTX_new();
    if (ctx == nullptr) return TranslateLastOpenSslError();
    if (!EC_POINT_get_affine_coordinates_GFp(EC_KEY_get0_group(ecKey), jacobian_coords, &x, &y,
                                             ctx)) {
        return TranslateLastOpenSslError();
    }
    uint8_t* tmp_x = x_coord;
    if (BN_bn2binpad(&x, tmp_x, kAffinePointLength) != kAffinePointLength) {
        return TranslateLastOpenSslError();
    }
    uint8_t* tmp_y = y_coord;
    if (BN_bn2binpad(&y, tmp_y, kAffinePointLength) != kAffinePointLength) {
        return TranslateLastOpenSslError();
    }

    EVP_PKEY_free(pubKey);
    X509_free(cert);
    BN_CTX_free(ctx);
    return KM_ERROR_OK;
}

}  // namespace keymaster
