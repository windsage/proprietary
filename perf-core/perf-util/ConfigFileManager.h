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

#ifndef __CONFIG_FILE_MANAGER_H__
#define __CONFIG_FILE_MANAGER_H__

#include <string>

#include "Base64Codec.h"

class ConfigFileManager {
private:
    static const char *DEBUG_CONFIG_DIR;     // "/data/vendor/perf/"
    static const char *VENDOR_CONFIG_DIR;    // "/vendor/etc/perf/"

    // BASE64文件标识
    static const std::string BASE64_HEADER;    // "BASE64:"
    static const size_t HEADER_SIZE = 7;       // "BASE64:"的长度

    struct ConfigFileInfo {
        const char *filename;
        bool isEncrypted;
        std::string content;
    };

public:
    /**
     * 获取配置文件路径（支持优先级）
     * @param filename 文件名
     * @return 文件完整路径
     */
    static std::string getConfigFilePath(const char *filename);

    /**
     * 读取并解密配置文件
     * @param filepath 文件路径
     * @param content 输出解密后的内容
     * @return 成功返回true
     */
    static bool readAndDecryptConfig(const std::string &filepath, std::string &content);

    /**
     * 检测文件是否为BASE64加密
     * @param filepath 文件路径
     * @return 是否为BASE64加密文件
     */
    static bool isFileEncrypted(const std::string &filepath);

    /**
     * 解密BASE64文件内容
     * @param encryptedContent 加密的内容
     * @param decryptedContent 输出解密后的内容
     * @return 成功返回true
     */
    static bool decryptFileContent(const std::string &encryptedContent,
                                   std::string &decryptedContent);
};

#endif    // __CONFIG_FILE_MANAGER_H__
