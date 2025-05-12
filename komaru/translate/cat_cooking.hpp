#pragma once

#include <komaru/lang/cat_program.hpp>
#include <komaru/parsers/parser_result.hpp>
#include <komaru/translate/raw_cat_program.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>

#include <expected>

namespace komaru::translate {

class CookingError {
public:
    explicit CookingError(std::string msg)
        : msg_(std::move(msg)) {
    }

    static CookingError From(parsers::ParserError error) {
        return CookingError(std::move(error.Error()));
    }

    const std::string& Error() const {
        return msg_;
    }

private:
    std::string msg_;
};

template <typename T>
using CookingResult = std::expected<T, CookingError>;

inline auto MakeCookingError(std::string msg) {
    return std::unexpected<CookingError>(std::move(msg));
}

CookingResult<lang::CatProgram> Cook(const RawCatProgram& program, hs::HaskellSymbolsRegistry& reg);

}  // namespace komaru::translate
