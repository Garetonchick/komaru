#pragma once
#include <komaru/lang/cat_program.hpp>

namespace komaru::test {

void CheckRunCppProgram(const lang::CatProgram& program, const std::string& expected_output);

/*
 *   a      $0
 *   ┌─>Int──┐              +
 * S─┤       ├>|Int x Int|─────>Int
 *   └─>Int──┘
 *   b      $1
 */
lang::CatProgram MakeAPlusBProgram(int32_t a, int32_t b);

/*
 *                 $0
 *         ┌───────────────────┐
 *         │                 $ │     *15
 *   x     │ <4 ┌────|False│───┴>Int────>Int
 * S───>Int├───>│Bool|     │ $       +10
 *         │    └────|True │───┬>Int────>Int
 *         │       $0          │
 *         └───────────────────┘
 */
lang::CatProgram MakeIf101Program(int32_t x);

/*
 *               +10
 *   x  ┌───|<4|────>Int
 * S───>│Int|  | *15
 *      └───|* |────>Int
 */
lang::CatProgram MakeGuards101Program(int32_t x);

lang::CatProgram MakeMegaIfProgram(int32_t x);
int32_t CalcMegaIfResult(int32_t x);

}  // namespace komaru::test
