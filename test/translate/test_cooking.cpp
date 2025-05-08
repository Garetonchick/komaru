#include <gtest/gtest.h>

#include <test/translate/common.hpp>
#include <test/translate/programs.hpp>

#include <komaru/translate/cat_cooking.hpp>
#include <komaru/translate/simple_symbols_registry.hpp>
#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>

using namespace komaru::test;
using namespace komaru::translate;

int Fib(int n) {
    if (n < 2) {
        return n;
    }

    int prev = 0;
    int cur = 1;

    for (int i = 1; i < n; ++i) {
        int next = cur + prev;
        prev = cur;
        cur = next;
    }

    return cur;
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
// TEST(Cooking, Fibonacci) {
//     for (int i = 0; i < 6; ++i) {
//         auto raw_program = MakeRawFibProgram(i);
//         auto maybe_program = Cook(raw_program, SimpleSymbolsRegistry{});

//         if (!maybe_program) {
//             std::println("cooking error: {}", maybe_program.error().Error());
//         }
//         ASSERT_TRUE(maybe_program.has_value());

//         CheckRunCppProgram(maybe_program.value(), std::format("{}\n", Fib(i)));
//     }
// }

/*
 *               id         $0
 *  read          ┌─|IO Int|─┐                   liftM2 +
 * S────>|IO Int|─┤          ├>|IO Int x IO Int|─────────>|IO Int|──>|IO S|
 *                └─|IO Int|─┘
 *               id         $1
 */
// TEST(Cooking, IO101) {
//     auto raw_program = MakeRawIO101Program();
//     auto maybe_cat_program = Cook(raw_program, SimpleSymbolsRegistry{});

//     if (!maybe_cat_program) {
//         std::println("cooking error: {}", maybe_cat_program.error().Error());
//     }

//     ASSERT_TRUE(maybe_cat_program.has_value());

//     auto cat_program = std::move(maybe_cat_program.value());

//     auto translator = cpp::CppTranslator("../../catlib/cpp");
//     auto maybe_program = translator.Translate(cat_program);

//     ASSERT_TRUE(maybe_program.has_value());
//     auto& program = maybe_program.value();

//     for (const auto& [input, expected] : std::vector<std::pair<std::string, std::string>>{
//              {"4 42", "46"},
//              {"52 321", "373"},
//              {"-8 8", "0"},
//          }) {
//         auto res = ExecProgram(*program, input);

//         ASSERT_TRUE(res.Success());
//         ASSERT_EQ(res.Output(), expected);
//     }
// }
