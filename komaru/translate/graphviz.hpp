#pragma once

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/raw_cat_program.hpp>

#include <string>

namespace komaru::translate {

std::string ToGraphviz(const RawCatProgram& program);
std::string ToGraphviz(const lang::CatProgram& program);

}  // namespace komaru::translate
