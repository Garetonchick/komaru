#pragma once
#include <komaru/translate/raw_cat_program.hpp>
#include <komaru/lang/cat_program.hpp>

namespace komaru::translate {

lang::CatProgram Cook(const RawCatProgram& program);
RawCatProgram Uncook(const lang::CatProgram& program);

}  // namespace komaru::translate
