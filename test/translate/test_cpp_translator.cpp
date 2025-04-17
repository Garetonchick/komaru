#include <gtest/gtest.h>

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>
#include <test/translate/programs.hpp>
#include <test/translate/common.hpp>

#include <print>

using namespace komaru::test;

/*
 *   9      $0
 *   ┌─>Int──┐              +
 * S─┤       ├>|Int x Int|─────>Int
 *   └─>Int──┘
 *   42     $1
 */
TEST(CppTranslator, APlusB) {
    CheckRunCppProgram(MakeAPlusBProgram(9, 42), "51\n");
    CheckRunCppProgram(MakeAPlusBProgram(-21, 39), "18\n");
}

/*
 *                 $0
 *         ┌───────────────────┐
 *         │                 $ │     *15
 *   5     │ <4 ┌────|False│───┴>Int────>Int
 * S───>Int├───>│Bool|     │ $       +10
 *         │    └────|True │───┬>Int────>Int
 *         │       $0          │
 *         └───────────────────┘
 */
TEST(CppTranslator, If101) {
    CheckRunCppProgram(MakeIf101Program(5), "75\n");
    CheckRunCppProgram(MakeIf101Program(4), "60\n");
    CheckRunCppProgram(MakeIf101Program(-2), "8\n");
}

/*
 *                           !y      *15
 *   x   y   <4 ┌────|False│────>Int────>Int
 * S───>Int────>│Bool|     │ !y      +10
 *              └────|True │────>Int────>Int
 */
TEST(CppTranslator, IfWithLocalVar) {
    CheckRunCppProgram(MakeIfWithLocalVarProgram(5), "75\n");
    CheckRunCppProgram(MakeIfWithLocalVarProgram(4), "60\n");
    CheckRunCppProgram(MakeIfWithLocalVarProgram(-2), "8\n");
}

/*
 *               +10
 *   5  ┌───|<4│────>Int
 * S───>│Int|  │ *15
 *      └───|* │────>Int
 */
TEST(CppTranslator, Guards101) {
    CheckRunCppProgram(MakeGuards101Program(5), "75\n");
    CheckRunCppProgram(MakeGuards101Program(4), "60\n");
    CheckRunCppProgram(MakeGuards101Program(-2), "8\n");
}

TEST(CppTranslator, MegaIf) {
    CheckRunCppProgram(MakeMegaIfProgram(0), std::to_string(CalcMegaIfResult(0)) + "\n");
    CheckRunCppProgram(MakeMegaIfProgram(2), std::to_string(CalcMegaIfResult(2)) + "\n");
    CheckRunCppProgram(MakeMegaIfProgram(3), std::to_string(CalcMegaIfResult(3)) + "\n");
    CheckRunCppProgram(MakeMegaIfProgram(-2), std::to_string(CalcMegaIfResult(-2)) + "\n");
}

/* fib:
 *          id
 * ┌───|<2│────>Int
 * │Int|  │ -1      -1       fib      $0
 * └───|* │────>Int┬────>Int─────>Int────┐             +
 *                 │ fib         $1      ├>|Int x Int|───>Int
 *                 └────>Int─────────────┘
 * main:
 *   x     fib
 * S───>Int───>Int
 */
TEST(CppTranslator, Fibonacci) {
    CheckRunCppProgram(MakeFibProgram(0), "0\n");
    CheckRunCppProgram(MakeFibProgram(1), "1\n");
    CheckRunCppProgram(MakeFibProgram(2), "1\n");
    CheckRunCppProgram(MakeFibProgram(3), "2\n");
    CheckRunCppProgram(MakeFibProgram(4), "3\n");
    CheckRunCppProgram(MakeFibProgram(5), "5\n");
    CheckRunCppProgram(MakeFibProgram(6), "8\n");
}
