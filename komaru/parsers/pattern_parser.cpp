#include "pattern_parser.hpp"

#include <komaru/util/string.hpp>

namespace komaru::parsers {

PatternParser::PatternParser(const std::string& raw)
    : raw_(raw) {
}

ParserResult<lang::Pattern> PatternParser::Parse() {
    if (util::Strip(raw_) == "*") {
        return lang::Pattern::Any();
    }
    throw std::runtime_error("not implemented");
}

}  // namespace komaru::parsers
