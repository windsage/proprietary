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

#include "Base64Codec.h"

#include <algorithm>

const std::string Base64Codec::CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Base64Codec::encode(const std::string &data) {
    std::string encoded;
    int val = 0, valb = -6;

    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encoded.push_back(CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back(PAD_CHAR);
    }

    return encoded;
}

std::string Base64Codec::decode(const std::string &encoded) {
    std::string decoded;
    int val = 0, valb = -8;

    for (char c : encoded) {
        if (c == PAD_CHAR)
            break;

        int index = findCharIndex(c);
        if (index == -1)
            continue;    // 跳过无效字符

        val = (val << 6) + index;
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decoded;
}

int Base64Codec::findCharIndex(char c) {
    size_t pos = CHARS.find(c);
    return (pos != std::string::npos) ? static_cast<int>(pos) : -1;
}

bool Base64Codec::isValidBase64(const std::string &str) {
    if (str.empty() || str.length() % 4 != 0) {
        return false;
    }

    // 检查字符是否都在BASE64字符集中
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c != PAD_CHAR && findCharIndex(c) == -1) {
            return false;
        }

        // 填充字符只能出现在末尾
        if (c == PAD_CHAR && i < str.length() - 2) {
            return false;
        }
    }

    return true;
}
