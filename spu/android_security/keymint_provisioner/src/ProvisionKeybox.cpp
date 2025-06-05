/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <dlfcn.h>
#include <fcntl.h>
#include <map>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

#include "InstallKeybox.h"

using namespace std;

#define GUARD_EXIT(code) \
    do {                 \
        ret = (code);    \
        if (ret != 0) {  \
            return ret;  \
        };               \
    } while (0)

#ifdef PRINT_FUNCTIONS
static void PrintCertData(keymaster_blob_t* key_blob) {
    uint8_t* keyboxData = (uint8_t*)key_blob->data;
    cerr << "key_blob->data_length" << key_blob->data_length << endl;
    for (int i = 0; i < (int)key_blob->data_length; i++) {
        char buf[4];
        if (i % 16 == 0) cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}

static void PrintKeyData(keymaster_key_blob_t* key_blob) {
    uint8_t* keyboxData = (uint8_t*)key_blob->key_material;
    cerr << "key_blob->key_material_size" << key_blob->key_material_size << endl;
    for (int i = 0; i < (int)key_blob->key_material_size; i++) {
        char buf[4];
        if (i % 16 == 0) cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}
#endif

int InstallKeybox::consumeTag(ifstream& inFile, const string& tag) {
    string input;

    getline(inFile, input, '<');
    getline(inFile, input, '>');
    if (input.compare(tag) != 0) {
        cerr << "Corrupt input, expected tag: '" << tag << "' found '" << input << "'" << endl;
        return GENERIC_FAILURE;
    }
    return ERROR_OK;
}

string InstallKeybox::consumeElement(ifstream& inFile, const string& startTag) {
    string result;

    consumeTag(inFile, startTag);

    getline(inFile, result, '<');
    inFile.putback('<');

    string endTag = "/" + startTag;
    consumeTag(inFile, endTag);
    return result;
}

int InstallKeybox::processDeviceID(ifstream& inFile, keymaster_blob_t* deviceId) {
    string input;
    size_t input_length = 0;

    // Seek and discard the first quotation mark in device id
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Keybox DeviceID=") != 0) {
        cerr << "Failed to find 'Keybox DeviceID= in input" << endl;
        return GENERIC_FAILURE;
    }

    // Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    input_length = input.size();
    deviceId->data = (uint8_t*)malloc(input_length);
    if (!deviceId->data) {
        cerr << "deviceId->data NULL" << endl;
        return GENERIC_FAILURE;
    }

    memcpy((void*)deviceId->data, &input[0], input_length);
    deviceId->data_length = input_length;

    // Discard '>' at end of <Keybox DeviceID="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::processKeyAlgo(ifstream& inFile, keymaster_algorithm_t* algorithm) {
    string input;

    // Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Key algorithm=") != 0) {
        cerr << "Failed to find 'Key algorithm in input" << endl;
        return GENERIC_FAILURE;
    }
    // Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    // Report current key algorithm and save it off

    if (input.compare("ecdsa") == 0) {
        *algorithm = KM_ALGORITHM_EC;
    } else if (input.compare("rsa") == 0) {
        *algorithm = KM_ALGORITHM_RSA;
    } else {
        cerr << "KeyAlgorithm is not 'ecdsa' or 'rsa' in input" << endl;
        return GENERIC_FAILURE;
    }

    // Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::processKeyFormat(ifstream& inFile) {
    string input;

    // Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("PrivateKey format=") != 0) {
        cerr << "Failed to find 'PrivateKey format' in input" << endl;
        return GENERIC_FAILURE;
    }
    // Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    // Report current key format and save it off
    if (input.compare("pem") == 0) {
    } else {
        cerr << "KeyFormat is not 'pem' in input" << endl;
        return GENERIC_FAILURE;
    }
    // Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::processCertFormat(ifstream& inFile) {
    string input;

    // Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("Certificate format=") != 0) {
        cerr << "Failed to find 'Certificate format' in input" << endl;
        return GENERIC_FAILURE;
    }
    // Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    if (input.compare("pem") == 0) {
    } else {
        cerr << "certFormat is not 'pem' in input" << endl;
        return GENERIC_FAILURE;
    }
    // Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
    return ERROR_OK;
}

int InstallKeybox::processPrivKey(ifstream& inFile, keymaster_key_blob_t* priv_key) {
    string input;

    // extract the key content and store them in input
    getline(inFile, input);
    getline(inFile, input, '<');

    // copy hex of key content in private key data
    priv_key->key_material = (uint8_t*)malloc(input.size());
    if (!priv_key->key_material) {
        cerr << "priv_key->key_material NULL" << endl;
        return GENERIC_FAILURE;
    }
    priv_key->key_material_size = 0;
    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size()) break;
        memcpy((uint8_t*)priv_key->key_material + offset, input1.c_str(), input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t*)priv_key->key_material + offset + input1.size(), 0x0A, 1);
        priv_key->key_material_size += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
    return ERROR_OK;
}

int InstallKeybox::processCertificate(ifstream& inFile, keymaster_blob_t* certificate) {
    string input;

    // extract the key content and store them in input
    getline(inFile, input);
    getline(inFile, input, '<');

    certificate->data = (uint8_t*)malloc(input.size());
    if (!certificate->data) {
        cerr << "certificate->data" << endl;
        return GENERIC_FAILURE;
    }
    certificate->data_length = 0;

    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size()) break;
        memcpy((uint8_t*)certificate->data + offset, input1.c_str(), input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t*)certificate->data + offset + input1.size(), 0xA, 1);
        certificate->data_length += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
    return ERROR_OK;
}

int InstallKeybox::processOneXmlKeybox(ifstream& inFile, std::string deviceId) {
    string input;
    int certCount = 0;
    int keyitem = 0, j;
    keymaster_blob_t* cert_entry;
    km_install_keybox_t keyboxCtx;
    uint8_t prov_cmd_buffer[SPU_KM_SHARED_BUF_LEN];
    uint32_t prov_cmd_buffer_size = sizeof(prov_cmd_buffer);

    memset(&keyboxCtx, 0, sizeof(keyboxCtx));
    int ret = GENERIC_FAILURE;
    // proces device ID:  <Keybox DeviceID=mfg_mod123_0000001>
    ret = (processDeviceID(inFile, &(keyboxCtx.deviceId)));
    if(ret != 0) {
        goto end_parse;
    }
    cout << "myDeviceID: " << deviceId << endl;
    cout << "keyboxCtx.deviceId.data: " << keyboxCtx.deviceId.data << endl;

    for (keyitem = 0; keyitem < MAX_KEYS_IN_KEYBOX; keyitem++) {
        // proces key algorithm:  <<Key algorithm=ecdsa"> or <Key algorithm=rsa>
        ret = (processKeyAlgo(inFile, &(keyboxCtx.keybox[keyitem].algo)));
        if(ret != 0) {
            goto end_parse;
        }

        // proces key format:  <PrivateKey format=pem>
        ret = (processKeyFormat(inFile));
        if(ret != 0) {
            goto end_parse;
        }

        // process key content
        ret = (processPrivKey(inFile, &(keyboxCtx.keybox[keyitem].key)));
        if(ret != 0) {
            goto end_parse;
        }

        // consume tag: <CertificateChain>
        ret = (consumeTag(inFile, "CertificateChain"));
        if(ret != 0) {
            goto end_parse;
        }
        // process number of certificates: <NumberOfCertificates>2</NumberOfCertificates>
        certCount = atol(consumeElement(inFile, "NumberOfCertificates").c_str());

        if (!certCount) {
            cerr << "certCount value is not valid" << endl;
            ret  =  GENERIC_FAILURE;
            goto end_parse;
        }

        // allocate certcount number of keymaster_blob_t type of space.
        // certificate and key shares the same data structure keymaster_blob_t
        keyboxCtx.keybox[keyitem].cert_chain.entry_count = certCount;

        if ((uint32_t)certCount > UINT_MAX / (uint32_t)sizeof(keymaster_blob_t)) {
            cerr << "Value too large to allocate memory" << endl;
            ret =  GENERIC_FAILURE;
            goto end_parse;
        }

        keyboxCtx.keybox[keyitem].cert_chain.entries =
                (keymaster_blob_t*)malloc(certCount * sizeof(keymaster_blob_t));
        if (!keyboxCtx.keybox[keyitem].cert_chain.entries) {
            cerr << "keyboxCtx.keybox[keyitem].cert_chain.entries NULL" << keyitem << endl;
            ret =  GENERIC_FAILURE;
            goto end_parse;
        }

        cert_entry = keyboxCtx.keybox[keyitem].cert_chain.entries;
        for (j = 0; j < certCount; j++) {
            // proces certificate format:  <Certificate format=pem>
            ret = (processCertFormat(inFile));
            if(ret != 0) {
                goto end_parse;
            }
            // process certificate content
            ret = (processCertificate(inFile, cert_entry));
            if(ret != 0) {
                goto end_parse;
            }
            // consume tag: /Certificate
            // consumeTag(inFile, "/Certificate");
            cert_entry++;
        }
        // consume tag: /CertificateChain
        ret = (consumeTag(inFile, "/CertificateChain"));
        if(ret != 0) {
            goto end_parse;
        }
        // consume tag: /Key
        ret = (consumeTag(inFile, "/Key"));
        if(ret != 0) {
            goto end_parse;
        }
    }

    // consume tag: /Keybox
    ret = (consumeTag(inFile, "/Keybox"));
    if(ret != 0) {
        goto end_parse;
    }

    // current Max keys in keybox is 2, one for ECDSA, another one for RSA
    if (memcmp(keyboxCtx.deviceId.data, deviceId.c_str(),
            keyboxCtx.deviceId.data_length)) {
        ret =  DEVICE_ID_NOT_FOUND;
        goto end_parse;
    }

    ret  = mProvisioningClient->KeyMasterInstallKeybox(&keyboxCtx, prov_cmd_buffer,
            &prov_cmd_buffer_size);
    if (ret) {
        cerr << "KeyMasterInstallKeybox error: " << ret << endl;
        goto end_parse;
    }

    ret = mKeymasterTzUtils->tzSpuKmProvisionAttestationKeys(
            prov_cmd_buffer, prov_cmd_buffer_size);
    if (ret != KM_ERROR_OK) {
        cerr << "Failed to provision attestation key: " << ret << endl;
        goto end_parse;
    }

    cout << "Successfully provisioned attestation key" << endl;

    ret =  ERROR_OK;
end_parse:
    cleanKeyBox(&keyboxCtx);

    return ret;
}

static volatile void* secureMemset(volatile void* dst, int c, size_t len) {
    volatile char* buf;

    for (buf = (volatile char*)dst; len; buf[--len] = c)
        ;
    return dst;
}

int InstallKeybox::cleanKeyBox(km_install_keybox_t* keyboxCtx) {
    int keysCount = MAX_KEYS_IN_KEYBOX;

    if (keyboxCtx->deviceId.data) {
        free((void*)keyboxCtx->deviceId.data);
        keyboxCtx->deviceId.data = NULL;
    }

    while ((keysCount--) && (keysCount < INT_MAX)) {
        if (keyboxCtx->keybox[keysCount].key.key_material)
            free((void*)keyboxCtx->keybox[keysCount].key.key_material);
        keymaster_free_cert_chain(&keyboxCtx->keybox[keysCount].cert_chain);
    }
    volatile uint8_t* temp = (uint8_t*)&keyboxCtx;
    secureMemset((volatile void*)temp, 0, sizeof(km_install_keybox_t));
    return ERROR_OK;
}

int InstallKeybox::provisionKeybox(std::string filename, std::string deviceId) {
    ifstream inFile(filename.c_str());
    string input;
    int keyCount = 0;
    int ret = GENERIC_FAILURE;

    if (inFile.is_open()) {
        GUARD_EXIT(consumeTag(inFile, "?xml version=\"1.0\"?"));
        GUARD_EXIT(consumeTag(inFile, "AndroidAttestation"));
        keyCount = atol(consumeElement(inFile, "NumberOfKeyboxes").c_str());
        cout << "Number of keyboxes " << keyCount << endl;

        while ((keyCount--) && (keyCount < INT_MAX)) {
            ret = processOneXmlKeybox(inFile, deviceId);
            //cleanKeyBox();
            if (ret != DEVICE_ID_NOT_FOUND) {
                break;
            }
        }

        if (ret == DEVICE_ID_NOT_FOUND) {
            // consume tag: /AndroidAttestation
            GUARD_EXIT(consumeTag(inFile, "/AndroidAttestation"));
            cerr << "Keybox for '" << deviceId << "' not found!" << endl;
        } else if (ret == 0) {
            //status = 0;
        }
    } else {
        cerr << "ERROR: Unable to open xml keyfile!" << endl;
        ret = GENERIC_FAILURE;
    }

    inFile.close();

    return ret;
}

int InstallKeybox::lockKeybox(void) {
    int ret;

    ret = mKeymasterTzUtils->tzServiceLock(KeymasterTzUtils::TZ_KEY_ID_ATTASTATION_KEY);

    if (ret != 0) {
        cerr << "Failed to lock keybox: " << ret << endl;
        return -1;
    }

    return 0;
}
