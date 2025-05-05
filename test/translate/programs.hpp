#pragma once
#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/raw_cat_program.hpp>

namespace komaru::test {

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
 *                           !y      *15
 *   x   y   <4 ┌────|False│────>Int────>Int
 * S───>Int────>│Bool|     │ !y      +10
 *              └────|True │────>Int────>Int
 */
lang::CatProgram MakeIfWithLocalVarProgram(int32_t x);

/*
 *               +10
 *   x  ┌───|<4│────>Int
 * S───>│Int|  │ *15
 *      └───|* │────>Int
 */
lang::CatProgram MakeGuards101Program(int32_t x);

lang::CatProgram MakeMegaIfProgram(int32_t x);
int32_t CalcMegaIfResult(int32_t x);

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
lang::CatProgram MakeFibProgram(int32_t x);
translate::RawCatProgram MakeRawFibProgram(int32_t x);

/*
 *               id         $0
 *  read          ┌─|IO Int|─┐                   liftM2 +
 * S────>|IO Int|─┤          ├>|IO Int x IO Int|─────────>|IO Int|──>|IO S|
 *                └─|IO Int|─┘
 *               id         $1
 */
lang::CatProgram MakeIO101Program();
translate::RawCatProgram MakeRawIO101Program();

}  // namespace komaru::test
