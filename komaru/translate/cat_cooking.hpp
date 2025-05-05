#pragma once

#include <komaru/translate/raw_cat_program.hpp>
#include <komaru/translate/symbols_registry_like.hpp>
#include <komaru/lang/cat_program.hpp>

#include <expected>

namespace komaru::translate {

class CookingError {
public:
    explicit CookingError(std::string msg)
        : msg_(std::move(msg)) {
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

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::CatProgram> Cook(const RawCatProgram& program, const SymbolsRegistry& reg);

}  // namespace komaru::translate
