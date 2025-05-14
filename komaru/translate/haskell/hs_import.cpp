#include "hs_import.hpp"

#include <sstream>

#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>

namespace komaru::translate::hs {

std::string HaskellImport::ToString() const {
    std::stringstream ss;
    ss << "import ";
    if (!ref_name.empty()) {
        ss << "qualified ";
    }

    ss << module_name << " ";

    if (!ref_name.empty()) {
        ss << "as " << ref_name << " ";
    }

    if (!symbols.empty()) {
        ss << "(";
        for (auto [i, symbol] : util::Enumerate(symbols)) {
            if (i > 0) {
                ss << ", ";
            }
            ss << symbol;
        }
        ss << ")";
    }

    return ss.str();
}

std::optional<HaskellImport> ParseHaskellImport(std::string s) {
    s = util::Strip(s);
    bool qualified = false;

    if (s.starts_with("import qualified ")) {
        s = util::Strip(s.substr(strlen("import qualified ")));
        qualified = true;
    } else if (s.starts_with("import ")) {
        s = util::Strip(s.substr(strlen("import ")));
    } else {
        return std::nullopt;
    }

    size_t pos = 0;

    while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '.')) {
        ++pos;
    }

    if (!pos) {
        return std::nullopt;
    }

    std::string import_name = s.substr(0, pos);
    std::string ref_name;
    s = util::Strip(s.substr(pos));

    if (qualified) {
        if (!s.starts_with("as ")) {
            return std::nullopt;
        }
        s = util::Strip(s.substr(strlen("as ")));

        pos = 0;

        while (pos < s.size() && std::isalnum(s[pos])) {
            ++pos;
        }

        if (!pos) {
            return std::nullopt;
        }

        ref_name = s.substr(0, pos);
        s = util::Strip(s.substr(pos));
    }

    std::vector<std::string> symbols;

    if (!s.empty() && s[0] == '(') {
        if (s.back() != ')') {
            return std::nullopt;
        }

        s = s.substr(1, s.size() - 2);
        bool expect_comma = false;

        for (size_t i = 0; i < s.size();) {
            if (std::isspace(s[i])) {
                ++i;
                continue;
            }

            if (s[i] == ',') {
                if (!expect_comma) {
                    return std::nullopt;
                }
                ++i;
                expect_comma = false;
                continue;
            }

            size_t end = i;

            while (end < s.size() && !std::isspace(s[end]) && s[end] != ',') {
                ++end;
            }

            symbols.push_back(s.substr(i, end - i));
            i = end;
            expect_comma = true;
        }
    }

    return hs::HaskellImport{.module_name = import_name, .ref_name = ref_name, .symbols = symbols};
}

}  // namespace komaru::translate::hs
