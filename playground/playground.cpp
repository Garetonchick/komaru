#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/haskell/hs_translator.hpp>
#include <komaru/translate/graphviz.hpp>
#include <komaru/translate/haskell/ghci.hpp>
#include <komaru/util/filesystem.hpp>
#include <komaru/translate/cat_cooking.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>
#include <komaru/util/cli_program_manipulator.hpp>
#include <komaru/util/string.hpp>
#include <test/translate/programs.hpp>
#include <catlib/cpp/catlib.hpp>

#include <print>
#include <ranges>

using komaru::util::WriteFile;

void SaveCppCode(const komaru::lang::CatProgram& cat_program) {
    komaru::translate::cpp::CppTranslator translator("../../catlib/cpp");

    auto cpp_program = translator.Translate(cat_program);

    WriteFile("gen.cpp", cpp_program.value()->GetSourceCode());
    std::println("Generated gen.cpp");
}

void SaveHaskellCode(const komaru::lang::CatProgram& cat_program) {
    komaru::translate::hs::HaskellTranslator translator;

    auto haskell_program = translator.Translate(cat_program);

    if(!haskell_program) {
        std::println("error: {}", haskell_program.error().Error());
        return;
    }

    WriteFile("gen.hs", haskell_program.value()->GetSourceCode());
    std::println("Generated gen.hs");
}

void PlayWithIO() {
    IO<int32_t> read1 = Read({}, DeductionTag<IO<int32_t>>{});
    IO<int32_t> read2 = read1;
    IO<int32_t> io_sum = LiftM2(Plus, read1, read2);
    IO<std::monostate> io_print = Bind(io_sum, Print);

    io_print.Run();
}

void PlayWithGraphvizRaw() {
    auto raw_fib_program = komaru::test::MakeRawFibProgram(5);

    WriteFile("graph.dot", komaru::translate::ToGraphviz(raw_fib_program));
    std::println("Generated graph.dot");
}

void VisualizeProgram(const komaru::lang::CatProgram& cat_program) {
    WriteFile("graph.dot", komaru::translate::ToGraphviz(cat_program));
    std::println("Generated graph.dot");
}

void PlayWithCooking() {
    auto raw_program = komaru::test::MakeRawIO101Program();
    komaru::translate::hs::HaskellSymbolsRegistry symbols_registry({}, {
        komaru::translate::hs::HaskellImport{
            .module_name = "Control.Monad",
            .ref_name = "",
            .symbols = {}
        }
    });
    auto maybe_program = komaru::translate::Cook(raw_program, symbols_registry);

    if(!maybe_program) {
        std::println("cooking error: {}", maybe_program.error().Error());
        return;
    }
    // DebugCatProgram(maybe_program.value());
    // VisualizeProgram(std::move(maybe_program.value()));
    SaveHaskellCode(maybe_program.value());
}

void PlayWithGHCIFixed() {
    komaru::util::CLIProgramManipulator ghci(
        {"ghci", "-package" ,"OpenGL", "-package", "GLUT"}, "ghci> "
    );

    for(const std::string& input : {
        "import           Graphics.Rendering.OpenGL\n",
        "import qualified Graphics.UI.GLUT          as GLUT\n",
        ":t GLUT.getArgsAndInitialize\n"
    }) {
        std::string response = ghci.Interact(input);
        std::println("input: \"{}\"\nresponse: \"{}\"", input, response);
    }
}

void PlayWithGHCI() {
    komaru::util::CLIProgramManipulator ghci(
        {"ghci", "-package" ,"OpenGL", "-package", "GLUT"}, "ghci> "
    );

    std::string input;

    std::print("ghci> ");

    while(std::getline(std::cin, input)) {
        std::string response = ghci.Interact(input + "\n");
        std::print("{}", response);
    }
}

int main() {
    // Choose your fighter (or code whatever you wish yourself)
    // SaveCode(komaru::test::MakeFibProgram(5));
    // SaveCode(komaru::test::MakeIO101Program());
    // PlayWithIO();
    // PlayWithGraphvizRaw();
    // VisualizeProgram(komaru::test::MakeFibProgram(5));
    // VisualizeProgram(komaru::test::MakeIO101Program());
    // VisualizeProgram(komaru::test::MakeIfWithLocalVarProgram(6));
    // VisualizeProgram(komaru::test::MakeMegaIfProgram(6));
    PlayWithCooking();
    // DebugCatProgram(komaru::test::MakeAPlusBProgram(4, 42));
    // SaveHaskellCode(komaru::test::MakeAPlusBProgram(4, 42));
    // DebugCatProgram(komaru::test::MakeIO101Program());
    // SaveHaskellCode(komaru::test::MakeIf101Program(6));
    // SaveHaskellCode(komaru::test::MakeIfWithLocalVarProgram(6));
    // SaveHaskellCode(komaru::test::MakeGuards101Program(6));
    // SaveHaskellCode(komaru::test::MakeMegaIfProgram(6));
    // SaveHaskellCode(komaru::test::MakeFibProgram(6));
    // SaveHaskellCode(komaru::test::MakeIO101Program());
    // PlayWithGHCIFixed();
    // PlayWithGHCI();
}
