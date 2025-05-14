#include <gtest/gtest.h>

#include <komaru/lang/cat_program.hpp>
#include <komaru/translate/haskell/hs_translator.hpp>
#include <komaru/translate/exec_program.hpp>
#include <test/translate/common.hpp>
#include <test/translate/programs.hpp>

#include <tuple>

using namespace komaru::test;
using namespace komaru::translate;

TEST(HaskellTranslator, APlusB) {
    for (const auto& [a, b, expected] : {
             std::make_tuple(9, 42, "51\n"),
             std::make_tuple(-21, 39, "18\n"),
         }) {
        CheckRunHaskellProgram(MakeAPlusBProgram(a, b), expected);
    }
}

TEST(HaskellTranslator, If101) {
    for (const auto& [x, expected] : {
             std::make_tuple(5, "75\n"),
             std::make_tuple(4, "60\n"),
             std::make_tuple(-2, "8\n"),
         }) {
        CheckRunHaskellProgram(MakeIf101Program(x), expected);
    }
}

TEST(HaskellTranslator, IfWithLocalVar) {
    for (const auto& [x, expected] : {
             std::make_tuple(5, "75\n"),
             std::make_tuple(4, "60\n"),
             std::make_tuple(-2, "8\n"),
         }) {
        CheckRunHaskellProgram(MakeIfWithLocalVarProgram(x), expected);
    }
}

TEST(HaskellTranslator, Guards101) {
    for (const auto& [x, expected] : {
             std::make_tuple(5, "75\n"),
             std::make_tuple(4, "60\n"),
             std::make_tuple(-2, "8\n"),
         }) {
        CheckRunHaskellProgram(MakeGuards101Program(x), expected);
    }
}

TEST(HaskellTranslator, MegaIf) {
    for (int32_t x : {0, 2, 3, -2}) {
        std::string expected = std::format("{}\n", CalcMegaIfResult(x));
        CheckRunHaskellProgram(MakeMegaIfProgram(x), expected);
    }
}

TEST(HaskellTranslator, Fibonacci) {
    for (const auto& [n, expected] : {
             std::make_tuple(0, "0\n"),
             std::make_tuple(1, "1\n"),
             std::make_tuple(2, "1\n"),
             std::make_tuple(3, "2\n"),
             std::make_tuple(4, "3\n"),
             std::make_tuple(5, "5\n"),
             std::make_tuple(6, "8\n"),
         }) {
        CheckRunHaskellProgram(MakeFibProgram(n), expected);
    }
}

TEST(HaskellTranslator, IO101) {
    auto cat_program = MakeIO101Program();
    auto translator = hs::HaskellTranslator(
        {}, {hs::HaskellImport{.module_name = "Control.Monad", .ref_name = "", .symbols = {}}});
    auto maybe_program = translator.Translate(cat_program);

    ASSERT_TRUE(maybe_program.has_value()) << maybe_program.error().Error();
    auto& program = maybe_program.value();

    for (const auto& [input, expected] : std::vector<std::pair<std::string, std::string>>{
             {"4\n42", "46\n"},
             {"52\n321", "373\n"},
             {"-8\n8", "0\n"},
         }) {
        auto res = ExecProgram(*program, input);

        ASSERT_TRUE(res.Success()) << std::format("Compile error: {}\nRuntime error:{}",
                                                  res.CompileError(), res.RuntimeError());
        ASSERT_EQ(res.Output(), expected);
    }
}
