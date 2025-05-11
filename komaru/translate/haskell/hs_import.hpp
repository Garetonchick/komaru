#pragma once

#include <string>
#include <vector>

namespace komaru::translate::hs {

struct HaskellImport {
    std::string module_name;
    std::string ref_name;
    std::vector<std::string> symbols;

    std::string ToString() const;
};

}  // namespace komaru::translate::hs
