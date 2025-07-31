/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>

namespace perfconfig {

class StringUtils {
public:
    static std::string trim(const std::string &str);
    static std::string removeAllWhitespace(const std::string &str);
    static std::vector<std::string> split(const std::string &str, char delimiter);
    static bool startsWith(const std::string &str, const std::string &prefix);
    static bool isBlank(const std::string &str);
    static bool parseInt(const std::string &str, int &result);
    static bool parseLong(const std::string &str, long &result, int base = 10);
    static std::string toLowerCase(const std::string &str);
};

}    // namespace perfconfig

#endif    // STRINGUTILS_H
