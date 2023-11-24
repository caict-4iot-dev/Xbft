/*
 * © COPYRIGHT 2022 Corporation CAICT All rights reserved.
 *  http://www.caict.ac.cn
 *  https://bitfactory.cn
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @author: guojian@caict.ac.cn
 * @date: 2022-06-07 19:31:49
 * @file: strings.h
 */
#ifndef UTILS_STRING_UTIL_H_
#define UTILS_STRING_UTIL_H_

#include <stdarg.h>
#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace common {
using StringVector = std::vector<std::string>;
using StringList = std::list<std::string>;
using StringMap = std::map<std::string, std::string>;
class String {
public:
    static std::string BinToHexString(const std::string &value) {
        std::string result;
        result.resize(value.size() * 2);
        for (size_t i = 0; i < value.size(); i++) {
            uint8_t item = value[i];
            uint8_t high = (item >> 4);
            uint8_t low = (item & 0x0F);
            result[2 * i] = (high >= 0 && high <= 9) ? (high + '0') : (high - 10 + 'a');
            result[2 * i + 1] = (low >= 0 && low <= 9) ? (low + '0') : (low - 10 + 'a');
        }
        return result;
    }

    static std::string Bin4ToHexString(const std::string &value) {
        std::string result;
        for (size_t i = 0; i < value.size() && i < 4; i++) {
            result = AppendFormat(result, "%02x", (uint8_t)value[i]);
        }

        return result;
    }

    static std::string HexStringToBin(const std::string &hexString, bool force_little = false) {
        if (hexString.size() % 2 != 0 || hexString.empty()) {
            return "";
        }
        std::string result;
        result.resize(hexString.size() / 2);
        for (size_t i = 0; i < hexString.size() - 1; i = i + 2) {
            uint8_t high = 0;
            if (hexString[i] >= '0' && hexString[i] <= '9')
                high = (hexString[i] - '0');
            else if (hexString[i] >= 'a' && hexString[i] <= 'f')
                high = (hexString[i] - 'a' + 10);
            else if (hexString[i] >= 'A' && hexString[i] <= 'F' && !force_little) {
                high = (hexString[i] - 'A' + 10);
            } else {
                return "";
            }

            uint8_t low = 0;
            if (hexString[i + 1] >= '0' && hexString[i + 1] <= '9')
                low = (hexString[i + 1] - '0');
            else if (hexString[i + 1] >= 'a' && hexString[i + 1] <= 'f')
                low = (hexString[i + 1] - 'a' + 10);
            else if (hexString[i + 1] >= 'A' && hexString[i + 1] <= 'F' && !force_little) {
                low = (hexString[i + 1] - 'A' + 10);
            } else {
                return "";
            }

            int valuex = (high << 4) + low;
            // sscanf(hex_string.substr(i, 2).c_str(), "%x", &valuex);
            result.at(i / 2) = (char)valuex;
        }

        return result;
    }

    static int64_t ToInt64(const std::string &cr_str) {
        if (0 == cr_str.length()) {
            return 0;
        }
        try {
            return std::stol(cr_str, nullptr, 10);
        } catch (const std::exception &) {
            return 0;
        }
    }

    static bool IsSpace(const char c_nValue) { return (c_nValue > 0) && isspace(c_nValue); }

    static unsigned int ToUint(const std::string &cr_str) {
        if (0 == cr_str.length()) {
            return 0;
        }
        size_t usedDigits = 0;
        uint32_t value = 0;
        for (int i = 0; i < (int)cr_str.size(); i++) {
            char itemValue = cr_str[i];
            if (itemValue >= '0' && itemValue <= '9') {
                value *= 10;
                value += (uint32_t)(itemValue - '0');
                usedDigits++;
            } else if (!IsSpace(itemValue) || usedDigits > 0) {
                break;
            }
        }

        return value;
    }

    static int StoI(const std::string &cr_str) {
        if (0 == cr_str.length()) {
            return 0;
        }
        return atoi(cr_str.c_str());
    }

    static StringVector split(const std::string &cr_str, const std::string &cr_delim) {
        size_t last = 0;
        size_t index = cr_str.find_first_of(cr_delim, last);
        StringVector ret;
        while (index != std::string::npos) {
            ret.push_back(cr_str.substr(last, index - last));
            last = index + 1;
            index = cr_str.find_first_of(cr_delim, last);
        }
        if (index - last > 0) {
            ret.push_back(cr_str.substr(last, index - last));
        }
        return ret;
    }

    static StringMap ParseAttribute(const std::string &cr_str, const std::string &cr_delim, const std::string &cr_ope) {
        StringMap ret;
        StringVector splitStr = split(cr_str, cr_delim);
        for (size_t i = 0; i < splitStr.size(); i++) {
            StringVector item = split(splitStr[i], cr_ope);
            if (item.size() > 1) {
                ret.insert(std::make_pair(item[0], item[1]));
            } else {
                ret.insert(std::make_pair(item[0], ""));
            }
        }
        return ret;
    }

    static std::string ComposePrefix(const std::string &cr_prefix, const std::string &cr_value) {
        std::string result = cr_prefix;
        result += "_";
        result += cr_value;
        return result;
    }

    static std::string ComposePrefix(const std::string &cr_prefix, int64_t value) {
        std::string result = cr_prefix;
        result += "_";
        result += std::to_string(value);
        return result;
    }

    static const int kMaxStringLen = 1024 * 1024;
    /// @brief Format String
    static std::string Format(const char *format, ...) {
        va_list ap;
        std::string str;
        int32_t nTimes = 1;
        while (nTimes < 256) {
            int nMalloc = kMaxStringLen * nTimes;
            char *buf = (char *)malloc(nMalloc);
            if (buf) {
                va_start(ap, format);
                int nCopy = vsnprintf(buf, nMalloc, format, ap);
                va_end(ap);
                if ((-1 < nCopy) && (nCopy < nMalloc)) {
                    str = buf;
                    free(buf);
                    break;
                }
                nTimes = nTimes * 2;
                free(buf);
            } else
                abort();
        }
        return str;
    }

    /// @brief Format String
    static std::string &Format(std::string &str, const char *format, ...) {
        va_list ap;
        int nTimes = 1;
        while (nTimes < 256) {
            int nMalloc = kMaxStringLen * nTimes;
            char *buf = (char *)malloc(nMalloc);
            if (buf) {
                va_start(ap, format);
                int nCopy = vsnprintf(buf, nMalloc, format, ap);
                va_end(ap);
                if ((-1 < nCopy) && (nCopy < nMalloc)) {
                    str = buf;

                    free(buf);
                    break;
                }
                free(buf);
                nTimes = nTimes * 2;
            } else
                abort();
        }
        return str;
    }

    static std::string AppendFormat(const std::string &cr_org, const char *format, ...) {
        va_list ap;

        std::string str = cr_org;
        int nTimes = 1;
        while (nTimes < 256) {
            int nMalloc = kMaxStringLen * nTimes;

            char *buf = (char *)malloc(nMalloc);
            if (!buf)
                abort();

            va_start(ap, format);
            int nCopy = vsnprintf(buf, nMalloc, format, ap);
            va_end(ap);
            if (-1 < nCopy && nCopy < nMalloc) {
                str += buf;
                free(buf);
                break;
            }
            free(buf);
            nTimes = nTimes * 2;
        }

        return str;
    }

    static StringVector Strtok(const std::string &cr_str, char separator) {
        size_t pos = 0;
        size_t newPos = 0;
        StringVector arr;

        while (std::string::npos != pos) {
            pos = cr_str.find_first_of(separator, newPos);
            if (std::string::npos == pos) {  // Complete
                if (pos > newPos) {
                    arr.push_back(cr_str.substr(newPos, pos - newPos));
                }
                break;
            } else {
                if (pos > newPos) {
                    arr.push_back(cr_str.substr(newPos, pos - newPos));
                }
                newPos = pos + 1;
            }
        }
        return arr;
    }
};
}  // namespace utils
#endif  // UTILS_STRING_UTIL_H_
