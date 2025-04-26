#include "string.hpp"

#include <cctype>
#include <string_view>

namespace komaru::util {

std::string Indent(const std::string& s, const std::string& indent) {
    using std::operator""sv;

    std::string res;

    for (const auto line : std::views::split(s, "\n"sv)) {
        auto sline = std::string(std::string_view(line));
        if (!sline.empty()) {
            res += indent + sline + "\n";
        } else {
            res += "\n";
        }
    }

    if (!res.empty()) {
        res.pop_back();
    }

    return res;
}

std::string Escape(const std::string& s, const std::string& esc) {
    std::string res;
    res.reserve(s.size());

    for (char c : s) {
        bool need_esc = std::invoke([&]() {
            for (char ec : esc) {
                if (c == ec) {
                    return true;
                }
            }
            return false;
        });

        if (need_esc) {
            res.push_back('\\');
        }
        res.push_back(c);
    }

    return res;
}

}  // namespace komaru::util
