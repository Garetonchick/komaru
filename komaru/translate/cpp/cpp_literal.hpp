#pragma once
#include <komaru/lang/literal.hpp>

namespace komaru::translate::cpp {

std::string ToCppLiteral(const lang::Literal& literal);

}
