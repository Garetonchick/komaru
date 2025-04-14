#pragma once
#include <komaru/lang/value.hpp>

namespace komaru::translate::cpp {

std::string ToCppValue(const lang::Value& value);

}
