#pragma once

#include <string>
#include <expected>

namespace komaru::parsers {

class ParserError {
public:
    explicit ParserError(std::string msg)
        : msg_(std::move(msg)) {
    }

    const std::string& Error() const {
        return msg_;
    }

private:
    std::string msg_;
};

template <typename T>
using ParserResult = std::expected<T, ParserError>;

inline auto MakeParserError(std::string msg) {
    return std::unexpected<ParserError>(std::move(msg));
}

}  // namespace komaru::parsers
