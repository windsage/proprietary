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

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <UniquePtr.h>

#include <hardware/keymaster_defs.h>

namespace keymaster {

template <typename BlobType> struct TKeymasterBlob;
// typedef TKeymasterBlob<keymaster_key_blob_t> KeymasterKeyBlob;

class EvpMdCtxCleaner {
  public:
    explicit EvpMdCtxCleaner(EVP_MD_CTX* ctx) : ctx_(ctx) {}
    ~EvpMdCtxCleaner() { EVP_MD_CTX_cleanup(ctx_); }

  private:
    EVP_MD_CTX* ctx_;
};

template <typename T, typename FreeFuncRet, FreeFuncRet (*FreeFunc)(T*)>
struct OpenSslObjectDeleter {
    void operator()(T* p) { FreeFunc(p); }
};

#define DEFINE_OPENSSL_OBJECT_POINTER(name)                                                        \
    typedef OpenSslObjectDeleter<name, decltype(name##_free(nullptr)), name##_free> name##_Delete; \
    typedef UniquePtr<name, name##_Delete> name##_Ptr;

DEFINE_OPENSSL_OBJECT_POINTER(ASN1_BIT_STRING)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_INTEGER)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OBJECT)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OCTET_STRING)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_TIME)
DEFINE_OPENSSL_OBJECT_POINTER(BN_CTX)
DEFINE_OPENSSL_OBJECT_POINTER(EC_GROUP)
DEFINE_OPENSSL_OBJECT_POINTER(EC_KEY)
DEFINE_OPENSSL_OBJECT_POINTER(EC_POINT)
DEFINE_OPENSSL_OBJECT_POINTER(ENGINE)
DEFINE_OPENSSL_OBJECT_POINTER(EVP_PKEY)
DEFINE_OPENSSL_OBJECT_POINTER(EVP_PKEY_CTX)
DEFINE_OPENSSL_OBJECT_POINTER(PKCS8_PRIV_KEY_INFO)
DEFINE_OPENSSL_OBJECT_POINTER(RSA)
DEFINE_OPENSSL_OBJECT_POINTER(X509)
DEFINE_OPENSSL_OBJECT_POINTER(X509_EXTENSION)
DEFINE_OPENSSL_OBJECT_POINTER(X509_NAME)

/**
 * Many OpenSSL APIs take ownership of an argument on success but don't free the argument on
 * failure. This means we need to tell our scoped pointers when we've transferred ownership, without
 * triggering a warning by not using the result of release().
 */
template <typename T, typename Delete_T>
inline void release_because_ownership_transferred(UniquePtr<T, Delete_T>& p) {
    T* val __attribute__((unused)) = p.release();
}

keymaster_error_t GenerateRandom(uint8_t* buf, size_t length);

keymaster_error_t GetEcdsa256KeyFromCert(const keymaster_blob_t* km_cert, uint8_t* x_coord,
                                         size_t x_length, uint8_t* y_coord, size_t y_length);

}  // namespace keymaster