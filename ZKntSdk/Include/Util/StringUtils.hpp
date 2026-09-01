#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace knt::util {
    inline std::vector<std::string> Split(const std::string& p_String, const std::string& p_Delimeter) {
        std::vector<std::string> s_Parts;

        size_t s_PartStart = 0;
        size_t s_PartEnd;

        while ((s_PartEnd = p_String.find(p_Delimeter, s_PartStart)) != std::string::npos) {
            s_Parts.push_back(p_String.substr(s_PartStart, s_PartEnd - s_PartStart));
            s_PartStart = s_PartEnd + p_Delimeter.length();
        }

        s_Parts.push_back(p_String.substr(s_PartStart));

        return s_Parts;
    }

    inline std::string ToLowerCase(const std::string& p_String) {
        std::string s_String = p_String;
        std::transform(s_String.begin(), s_String.end(), s_String.begin(), ::tolower);
        return s_String;
    }

    inline std::string ToUpperCase(const std::string& p_String) {
        std::string s_String = p_String;
        std::transform(s_String.begin(), s_String.end(), s_String.begin(), ::toupper);
        return s_String;
    }

    inline bool FindSubstring(const std::string& p_String, const std::string& p_SubString, const bool p_CaseSensitive = false) {
        if (p_SubString.empty()) {
            return true;
        }

        const auto it = std::ranges::search(p_String, p_SubString, [p_CaseSensitive](const char ch1, const char ch2) {
                            if (p_CaseSensitive) {
                                return ch1 == ch2;
                            }
                            return std::tolower(ch1) == std::tolower(ch2);
                        }).begin();

        return (it != p_String.end());
    }

    inline bool StartsWith(const std::string& p_String, const std::string& p_Prefix) {
        return p_String.size() >= p_Prefix.size() && p_String.compare(0, p_Prefix.size(), p_Prefix) == 0;
    }

    inline bool EndsWith(const std::string& p_String, const std::string& p_Suffix) {
        return p_String.size() >= p_Suffix.size() && p_String.compare(p_String.size() - p_Suffix.size(), p_Suffix.size(), p_Suffix) == 0;
    }
}
