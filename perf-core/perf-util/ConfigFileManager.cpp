/*
 * Copyright (C) 2025 Transsion Holdings
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

#include "ConfigFileManager.h"

#include <unistd.h>

#include <fstream>

#include "PerfLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CONFIG-FILE-MANAGER"

const char *ConfigFileManager::DEBUG_CONFIG_DIR = "/data/vendor/perf/";
const char *ConfigFileManager::VENDOR_CONFIG_DIR = "/vendor/etc/perf/";
const std::string ConfigFileManager::BASE64_HEADER = "BASE64:";

std::string ConfigFileManager::getConfigFilePath(const char *filename) {
    if (!filename) {
        QLOGE(LOG_TAG, "Invalid filename parameter");
        return "";
    }

    std::string debugPath = std::string(DEBUG_CONFIG_DIR) + filename;
    std::string vendorPath = std::string(VENDOR_CONFIG_DIR) + filename;

    if (access(debugPath.c_str(), F_OK) == 0) {
        QLOGL(LOG_TAG, QLOG_L1, "Using debug config: %s", debugPath.c_str());
        return debugPath;
    }

    if (access(vendorPath.c_str(), F_OK) == 0) {
        QLOGL(LOG_TAG, QLOG_L1, "Using vendor config: %s", vendorPath.c_str());
        return vendorPath;
    }

    QLOGE(LOG_TAG, "Config file not found: %s", filename);
    return "";
}

bool ConfigFileManager::isFileEncrypted(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        QLOGE(LOG_TAG, "Failed to open file for encryption check: %s", filepath.c_str());
        return false;
    }

    std::string header(HEADER_SIZE, '\0');
    file.read(&header[0], HEADER_SIZE);
    file.close();

    if (file.gcount() < static_cast<std::streamsize>(HEADER_SIZE)) {
        return false;
    }

    bool isEncrypted = (header == BASE64_HEADER);

    QLOGL(LOG_TAG, QLOG_L2, "File %s encryption check: %s", filepath.c_str(),
          isEncrypted ? "encrypted" : "plain");

    return isEncrypted;
}

bool ConfigFileManager::decryptFileContent(const std::string &encryptedContent,
                                           std::string &decryptedContent) {
    if (encryptedContent.length() < HEADER_SIZE ||
        encryptedContent.substr(0, HEADER_SIZE) != BASE64_HEADER) {
        QLOGE(LOG_TAG, "Invalid BASE64 encrypted file format");
        return false;
    }

    std::string base64Data = encryptedContent.substr(HEADER_SIZE);

    try {
        decryptedContent = Base64Codec::decodeMtk(base64Data);
        QLOGL(LOG_TAG, QLOG_L1, "Successfully decoded MTK encrypted content, size: %zu -> %zu",
              base64Data.length(), decryptedContent.length());
        return true;
    } catch (const std::exception &e) {
        QLOGE(LOG_TAG, "Exception during MTK decoding: %s", e.what());
        return false;
    } catch (...) {
        QLOGE(LOG_TAG, "Unknown exception during MTK decoding");
        return false;
    }
}

bool ConfigFileManager::readAndDecryptConfig(const std::string &filepath, std::string &content) {
    if (filepath.empty()) {
        QLOGE(LOG_TAG, "Empty filepath provided");
        return false;
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        QLOGE(LOG_TAG, "Failed to open config file: %s", filepath.c_str());
        return false;
    }

    std::string fileContent;
    try {
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        fileContent.resize(fileSize);
        file.read(&fileContent[0], fileSize);
        file.close();

        if (file.gcount() != static_cast<std::streamsize>(fileSize)) {
            QLOGE(LOG_TAG, "Failed to read complete file: %s", filepath.c_str());
            return false;
        }
    } catch (const std::exception &e) {
        QLOGE(LOG_TAG, "Exception reading file %s: %s", filepath.c_str(), e.what());
        file.close();
        return false;
    }

    if (fileContent.length() >= HEADER_SIZE &&
        fileContent.substr(0, HEADER_SIZE) == BASE64_HEADER) {
        QLOGL(LOG_TAG, QLOG_L1, "Decrypting MTK encrypted config file: %s", filepath.c_str());
        return decryptFileContent(fileContent, content);
    } else {
        QLOGL(LOG_TAG, QLOG_L1, "Using plain config file: %s", filepath.c_str());
        content = fileContent;
        return true;
    }
}
