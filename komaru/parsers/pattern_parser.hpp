#pragma once

#include <komaru/lang/pattern.hpp>
#include <komaru/parsers/parser_result.hpp>

namespace komaru::parsers {

class PatternParser {
public:
    explicit PatternParser(const std::string& raw);

    ParserResult<lang::Pattern> Parse();

private:
    std::string raw_;
};

}  // namespace komaru::parsers
