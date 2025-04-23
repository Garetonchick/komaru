#pragma once
#include <string>
#include <ranges>

namespace komaru::util {

inline constexpr bool IsSpace(char c) {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

// constexpr std::string Strip(const std::string& s);
inline constexpr std::string Strip(const std::string& s) {
    int64_t len = static_cast<int64_t>(s.size());
    int64_t start = 0;
    int64_t finish = len - 1;

    while (start < len && IsSpace(s[start])) {
        ++start;
    }

    while (finish >= 0 && IsSpace(s[finish])) {
        --finish;
    }

    if (start > finish) {
        return "";
    }

    return std::string(s.begin() + start, s.begin() + finish + 1);
}

// https://stackoverflow.com/questions/25890784/computing-length-of-a-c-string-at-compile-time-is-this-really-a-constexpr
template <size_t N>
constexpr size_t CStrlen(char const (&)[N]) {
    return N - 1;
}

inline decltype(auto) JoinStrings(const std::string& sep) {
    return std::views::transform([first = true, &sep](const auto& s) mutable {
               if (first) {
                   first = false;
                   return std::array<std::string, 2>{"", s} | std::views::join;
               }
               return std::array<std::string, 2>{sep, s} | std::views::join;
           }) |
           std::views::join;
}

}  // namespace komaru::util
