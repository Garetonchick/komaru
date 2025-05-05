#pragma once

#include <komaru/translate/raw_cat_program.hpp>
#include <komaru/lang/cat_program.hpp>

namespace komaru::translate {

RawCatProgram Uncook(const lang::CatProgram& program);

}
