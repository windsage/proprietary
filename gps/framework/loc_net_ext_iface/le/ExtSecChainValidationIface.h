/******************************************************************************
Copyright (c) 2023  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/
#ifndef  EXT_SEC_CHAIN_VALIDATION_IFACE_H
#define  EXT_SEC_CHAIN_VALIDATION_IFACE_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509_vfy.h>

class ExtSecChainValidationIface {
public:
    /** @brief
        Delete an instance of ExtSecChainValidationIface object. <br/>
    */
    virtual ~ExtSecChainValidationIface() {};

    /** @brief callback for SSL_CTX_set_verify
        @param preverifyOk - validation status of the certificate
        @param clientCtx: pointer to client context
        @return SUCCESS, on success
        @return FAILURE, on failure
    */
    virtual int ExtSecOverrideVerifyResult(int preverifyOk, X509_STORE_CTX *certContextStore) = 0;

    /** @brief callback for SSL_CTX_set_cert_verify_callback
        @param certContextStore - context which keeps all certificates
        @param connectionData   - extra parameter to pass information.
        @return SUCCESS, on success
        @return FAILURE, on failure
    */
    virtual int ExtSecVerifyCertificateChain(X509_STORE_CTX *certContextStore,
        void *connectionData) = 0;

    /** @brief prepare ssl context for local certificates
        @param sslContext: context to prepare
        @return TRUE, on success
        @return FALSE, on failure
    */
    virtual bool ExtSecPrepareDeviceSslContext(SSL_CTX* const sslContext) = 0;

    /** @brief initialization of certificates chain validation
        @return TRUE, on initialization success
        @return FALSE, on failure
    */
    virtual bool ExtSecChainValidationInit(void) = 0;

    /** @brief handle ssl error returned by ssl handshake
        @param sslHost: host where connection was opened
        @param sslError: ssl error code. if NULL, error is got from ssl context
        @return TRUE,  ssl error handled OK
        @return FALSE, any internal error
    */
    virtual bool ExtSecHandleSslError(const char* sslHost, const unsigned long* sslError) = 0;

    /** @brief check if the given URL is allowed for connect
        @param str_url: host to check as string
        @note  URL can be in formats:
                    proto://host.com/page
                    proto://host.com
                    proto://host.com:port
                    proto://host.com:port/page
                    proto: https, http, coaps, ftp or any other protocol
                    port: 443, 5684 or any integer value
                    page:
    */
    virtual bool ExtSecIsUrlAllowed(const char* strUrl) = 0;

    /** @brief deinitialization of certificates chain validation
    */
    virtual void ExtSecChainValidationDeinit(void) = 0;

    ExtSecChainValidationIface(const ExtSecChainValidationIface &) = delete;
    ExtSecChainValidationIface(ExtSecChainValidationIface &&) = delete;
    ExtSecChainValidationIface& operator=(const ExtSecChainValidationIface &) = delete;
    ExtSecChainValidationIface& operator=(ExtSecChainValidationIface &&) = delete;
};

// Entry point to the loc net ext sec chain validation impl
// Must implement: extern "C" ExtSecChainValidationIface* getLocNetExtSecChainValidationInstance()
typedef ExtSecChainValidationIface* (getLocNetExtSecChainFn)();

#endif /*EXT_SEC_CHAIN_VALIDATION_IFACE_H */
