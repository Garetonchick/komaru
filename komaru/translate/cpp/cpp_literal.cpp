#include "cpp_literal.hpp"

#include <komaru/util/std_extensions.hpp>

#include <format>

namespace komaru::translate::cpp {

std::string ToCppLiteral(const lang::Literal& literal) {
    return literal.Visit(util::Overloaded{[](int64_t val) {
                                              return std::to_string(val);
                                          },
                                          [](double val) {
                                              return std::to_string(val);
                                          },
                                          [](char val) {
                                              return std::format("'{}'", val);
                                          },
                                          [](const std::string& val) {
                                              return std::format("\"{}\"", val);
                                          }});
}

}  // namespace komaru::translate::cpp
