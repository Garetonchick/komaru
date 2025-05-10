#pragma once

#include <komaru/lang/cat_program.hpp>

namespace komaru::test {

void CheckRunCppProgram(const lang::CatProgram& program, const std::string& expected_output);
void CheckRunHaskellProgram(const lang::CatProgram& program, const std::string& expected_output);

}  // namespace komaru::test
