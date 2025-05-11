#include "hs_import.hpp"

#include <sstream>

#include <komaru/util/std_extensions.hpp>

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

}  // namespace komaru::translate::hs
