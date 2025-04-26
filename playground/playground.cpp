#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/graphviz.hpp>
#include <komaru/util/filesystem.hpp>
#include <test/translate/programs.hpp>
#include <catlib/cpp/catlib.hpp>

#include <print>
#include <ranges>

using komaru::util::WriteFile;

void SaveCode(const komaru::lang::CatProgram& cat_program) {
    komaru::translate::cpp::CppTranslator translator("../../catlib/cpp");

    auto cpp_program = translator.Translate(cat_program);

    WriteFile("gen.cpp", cpp_program.value()->GetSourceCode());
    std::println("Generated gen.cpp");
}

void PlayWithIO() {
    IO<int32_t> read1 = Read({}, DeductionTag<IO<int32_t>>{});
    IO<int32_t> read2 = read1;
    IO<int32_t> io_sum = LiftM2(Plus, read1, read2);
    IO<std::monostate> io_print = Bind(io_sum, Print);

    io_print.Run();
}

void PlayWithGraphvizRaw() {
    using komaru::translate::RawCatProgram;
    using NodeID = komaru::translate::RawCatProgram::NodeID;

    RawCatProgram prog;
    // fib
    NodeID snode = prog.NewRootNodeWithName("Int", "fib", {"| <2", "*"});
    NodeID branch0_node = prog.NewEndNode("Int");
    NodeID branch1_node = prog.NewNodeWithName("Int", "mop");
    NodeID minus2_node = prog.NewNode("Int");
    NodeID minus1_fib_node = prog.NewNodeWithName("Int", "x");
    NodeID minus2_fib_node = prog.NewNode("Int");
    NodeID pair_node = prog.NewNodeWithName("Int x Int", "y");
    NodeID sum_node = prog.NewEndNode("Int");

    prog.Connect(snode, branch0_node, 0, "id")
        .Connect(snode, branch1_node, 1, "-1")
        .Connect(branch1_node, minus2_node, 0, "-1")
        .Connect(branch1_node, minus1_fib_node, 0, "fib")
        .Connect(minus2_node, minus2_fib_node, 0, "fib")
        .Connect(minus2_fib_node, pair_node, 0, "$0")
        .Connect(minus1_fib_node, pair_node, 0, "$1")
        .Connect(pair_node, sum_node, 0, "+");

    // main
    NodeID main_node = prog.NewRootNodeWithName("S", "main");
    NodeID val_node = prog.NewNodeWithName("Int", "kek");
    NodeID final_node = prog.NewEndNode("Int");

    prog.Connect(main_node, val_node, 0, "5")
        .Connect(val_node, final_node, 0, "fib");

    WriteFile("graph.dot", komaru::translate::ToGraphviz(prog));
    std::println("Generated graph.dot");
}

void VisualizeProgram(const komaru::lang::CatProgram& cat_program) {
    WriteFile("graph.dot", komaru::translate::ToGraphviz(cat_program));
    std::println("Generated graph.dot");
}

int main() {
    // Choose your fighter (or code whatever you wish yourself)
    // SaveCode(komaru::test::MakeFibProgram(5));
    // SaveCode(komaru::test::MakeIO101Program());
    // PlayWithIO();
    // PlayWithGraphvizRaw();
    // VisualizeProgram(komaru::test::MakeFibProgram(5));
    VisualizeProgram(komaru::test::MakeIO101Program());
    // VisualizeProgram(komaru::test::MakeIfWithLocalVarProgram(6));
    // VisualizeProgram(komaru::test::MakeMegaIfProgram(6));
}
