#pragma once
#include <lang/cat_program.hpp>
#include <translate/program.hpp>

#include <expected>

namespace komaru::translate {

class TranslationError {
public:
    TranslationError(std::string msg)
        : msg_(std::move(msg)) {
    }

    const std::string& Error() const {
        return msg_;
    }

private:
    std::string msg_;
};

template <typename T>
using TranslationResult = std::expected<T, TranslationError>;

inline auto MakeTranslationError(std::string msg) {
    return std::unexpected<TranslationError>(std::move(msg));
}

struct ITranslator {
    virtual ~ITranslator() = default;

    // TODO: returning unique_ptr sucks
    virtual TranslationResult<std::unique_ptr<IProgram>> Translate(
        const lang::CatProgram& cat_program) = 0;
};

}  // namespace komaru::translate
