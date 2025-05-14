#pragma once

#include <string>
#include <vector>
#include <optional>

namespace komaru::translate::hs {

struct HaskellImport {
    std::string module_name;
    std::string ref_name;
    std::vector<std::string> symbols;

    std::string ToString() const;
};

std::optional<HaskellImport> ParseHaskellImport(std::string s);

}  // namespace komaru::translate::hs
