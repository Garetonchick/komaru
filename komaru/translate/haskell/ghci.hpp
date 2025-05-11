#pragma once

#include <vector>
#include <string>
#include <optional>

#include <komaru/lang/type.hpp>
#include <komaru/translate/haskell/hs_import.hpp>
#include <komaru/util/cli_program_manipulator.hpp>

namespace komaru::translate::hs {

class GHCI;

using GHCIPtr = std::unique_ptr<GHCI>;

class GHCI : public util::CLIProgramManipulator {
    struct PrivateDummy {};

public:
    GHCI(PrivateDummy, const std::vector<std::string>& packages);

    static std::optional<GHCIPtr> CreateSession(const std::vector<std::string>& packages,
                                                const std::vector<HaskellImport>& imports);

    std::optional<lang::Type> GetType(const std::string& name);
    std::optional<lang::TypeConstructor> GetTypeConstructor(const std::string& name);
    static bool HasError(const std::string& resp);

private:
    std::vector<std::string> MakeStartCommand(const std::vector<std::string>& packages);
    std::optional<lang::TypeConstructor> ParseTypeConstructor(const std::string& name,
                                                              const std::string& raw);
};

}  // namespace komaru::translate::hs
