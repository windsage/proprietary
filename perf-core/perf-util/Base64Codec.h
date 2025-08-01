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

#ifndef __BASE64_CODEC_H__
#define __BASE64_CODEC_H__

#include <string>

class Base64Codec {
private:
    // BASE64字符表
    static const std::string CHARS;
    static const char PAD_CHAR = '=';

    // MTK偏移量常量
    static const int MTK_OFFSET = 33;

    // 查找字符在BASE64表中的索引
    static int findCharIndex(char c);

public:
    /**
     * BASE64编码
     * @param data 原始数据
     * @return 编码后的字符串
     */
    static std::string encode(const std::string &data);

    /**
     * BASE64解码
     * @param encoded 编码后的字符串
     * @return 解码后的原始数据
     */
    static std::string decode(const std::string &encoded);

    /**
     * MTK风格解码：33偏移解密 + base64解码
     * @param offsetEncoded 33偏移加密的数据
     * @return 解码后的原始数据
     */
    static std::string decodeMtk(const std::string &offsetEncoded);

    /**
     * 检查字符串是否为有效的BASE64格式
     * @param str 待检查的字符串
     * @return 是否为有效BASE64
     */
    static bool isValidBase64(const std::string &str);
};

#endif    // __BASE64_CODEC_H__
