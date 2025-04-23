#include "common.hpp"

#include <gtest/gtest.h>

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>
#include <komaru/util/string.hpp>

namespace komaru::test {

void CheckRunCppProgram(const lang::CatProgram& program, const std::string& expected_output) {
    translate::cpp::CppTranslator translator("../../catlib/cpp");
    auto maybe_program = translator.Translate(program);

    ASSERT_TRUE(maybe_program.has_value());

    auto cpp_program = std::move(maybe_program.value());

    auto exec_res = translate::ExecProgram(*cpp_program);

    ASSERT_TRUE(exec_res.Success());
    ASSERT_EQ(exec_res.Output(), expected_output);
}

}  // namespace komaru::test
