/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>

#define LOG_TAG "PERF-CONFIG-UTILS"

namespace perfconfig {

std::string StringUtils::trim(const std::string &str) {
    if (str.empty()) {
        return str;
    }

    size_t start = 0;
    size_t end = str.length() - 1;

    while (start <= end &&
           (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) {
        start++;
    }

    while (end > start &&
           (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }

    if (start > end) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

std::string StringUtils::removeAllWhitespace(const std::string &str) {
    std::string result;
    result.reserve(str.length());

    for (char c : str) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            result += c;
        }
    }

    return result;
}

std::vector<std::string> StringUtils::split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;

    if (str.empty()) {
        return tokens;
    }

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!isBlank(token)) {
            tokens.push_back(trim(token));
        }
        start = end + 1;
        end = str.find(delimiter, start);
    }

    std::string lastToken = str.substr(start);
    if (!isBlank(lastToken)) {
        tokens.push_back(trim(lastToken));
    }

    return tokens;
}

bool StringUtils::startsWith(const std::string &str, const std::string &prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool StringUtils::isBlank(const std::string &str) {
    return trim(str).empty();
}

bool StringUtils::parseInt(const std::string &str, int &result) {
    if (str.empty()) {
        return false;
    }

    char *endptr = nullptr;
    errno = 0;
    long val = strtol(str.c_str(), &endptr, 10);

    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        return false;
    }

    if (endptr == str.c_str() || *endptr != '\0') {
        return false;
    }

    result = static_cast<int>(val);
    return true;
}

bool StringUtils::parseLong(const std::string &str, long &result, int base) {
    if (str.empty()) {
        return false;
    }

    char *endptr = nullptr;
    errno = 0;
    long val = strtol(str.c_str(), &endptr, base);

    if (errno == ERANGE) {
        return false;
    }

    if (endptr == str.c_str() || *endptr != '\0') {
        return false;
    }

    result = val;
    return true;
}

std::string StringUtils::toLowerCase(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

}    // namespace perfconfig
