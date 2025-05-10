#include "programs.hpp"

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>
#include <test/translate/program_utils.hpp>

using namespace komaru::lang;

namespace komaru::test {

lang::CatProgram MakeAPlusBProgram(int32_t a, int32_t b) {
    auto val1 = MakeLiteralMorphism(a);
    auto val2 = MakeLiteralMorphism(b);
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto plus = lang::Morphism::Plus();

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [num0_node, num0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [num1_node, num1_pin] = builder.NewNodeWithPin(Type::Int());
    auto [pair_node, pair_pin] = builder.NewNodeWithPin(Type::Int().Pow(2));
    auto& res_node = builder.NewNode(Type::Int());

    builder.Connect(start_pin, num0_node, val1)
        .Connect(start_pin, num1_node, val2)
        .Connect(num0_pin, pair_node, pos0)
        .Connect(num1_pin, pair_node, pos1)
        .Connect(pair_pin, res_node, plus);

    return builder.Extract();
}

lang::CatProgram MakeIf101Program(int32_t x) {
    auto val = Morphism::Literal(Literal::Number(x));
    auto less4 = MakeRBindLess(4);
    auto pos0 = Morphism::Position(0);
    auto none_pos = Morphism::NonePosition();
    auto mul15 = MakeRBindMul(15);
    auto add10 = MakeRBindPlus(10);

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [val_node, val_pin] = builder.NewNodeWithPin(Type::Int());
    auto& cond_node = builder.NewNode(Type::Bool());
    auto [branch0_node, branch0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [branch1_node, branch1_pin] = builder.NewNodeWithPin(Type::Int());
    auto& res0_node = builder.NewNode(Type::Int());
    auto& res1_node = builder.NewNode(Type::Int());

    auto& false_pin = cond_node.AddOutPin(Pattern::False());
    auto& true_pin = cond_node.AddOutPin(Pattern::True());

    builder.Connect(start_pin, val_node, val)
        .Connect(val_pin, cond_node, less4)
        .Connect(val_pin, branch0_node, pos0)
        .Connect(val_pin, branch1_node, pos0)
        .Connect(false_pin, branch0_node, none_pos)
        .Connect(true_pin, branch1_node, none_pos)
        .Connect(branch0_pin, res0_node, mul15)
        .Connect(branch1_pin, res1_node, add10);

    return builder.Extract();
}

lang::CatProgram MakeIfWithLocalVarProgram(int32_t x) {
    auto val = MakeLiteralMorphism(x);
    auto less4 = MakeRBindLess(4);
    auto mul15 = MakeRBindMul(15);
    auto add10 = MakeRBindPlus(10);
    auto y_var = Morphism::Value("y", Type::Int());
    auto just = Morphism::Just();
    auto just_y_var = Morphism::Binded(just, {{1, y_var}});

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [val_node, val_pin] = builder.NewNodeWithPin(Type::Int(), "y");
    auto& cond_node = builder.NewNode(Type::Bool());
    auto [branch0_node, branch0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [branch1_node, branch1_pin] = builder.NewNodeWithPin(Type::Int());
    auto& res0_node = builder.NewNode(Type::Int());
    auto& res1_node = builder.NewNode(Type::Int());

    auto& false_pin = cond_node.AddOutPin(Pattern::False());
    auto& true_pin = cond_node.AddOutPin(Pattern::True());

    builder.Connect(start_pin, val_node, val)
        .Connect(val_pin, cond_node, less4)
        .Connect(false_pin, branch0_node, just_y_var)
        .Connect(true_pin, branch1_node, just_y_var)
        .Connect(branch0_pin, res0_node, mul15)
        .Connect(branch1_pin, res1_node, add10);

    return builder.Extract();
}

lang::CatProgram MakeGuards101Program(int32_t x) {
    auto val = MakeLiteralMorphism(x);
    auto less4_guard = Guard(MakeRBindLess(4));
    auto mul15 = MakeRBindMul(15);
    auto add10 = MakeRBindPlus(10);

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto& val_node = builder.NewNode(Type::Int());
    auto& res0_node = builder.NewNode(Type::Int());
    auto& res1_node = builder.NewNode(Type::Int());

    auto& less4_pin = val_node.AddOutPin(less4_guard);
    auto& any_pin = val_node.AddOutPin(Pattern::Any());

    builder.Connect(start_pin, val_node, val)
        .Connect(less4_pin, res0_node, add10)
        .Connect(any_pin, res1_node, mul15);

    return builder.Extract();
}

lang::CatProgram MakeMegaIfProgram(int32_t x) {
    auto val = MakeLiteralMorphism(x);
    auto plus = Morphism::Plus();
    auto plus1 = MakeRBindPlus(1);
    auto plus2 = MakeRBindPlus(2);
    auto plus3 = MakeRBindPlus(3);
    auto plus7 = MakeRBindPlus(7);
    auto plus20 = MakeRBindPlus(20);
    auto minus2 = MakeRBindMinus(2);
    auto minus5 = MakeRBindMinus(5);
    auto minus8 = MakeRBindMinus(8);
    auto mul = Morphism::Multiply();
    auto mul2 = MakeRBindMul(2);
    auto mul3 = MakeRBindMul(3);
    auto mul4 = MakeRBindMul(4);
    auto mul5 = MakeRBindMul(5);
    auto mul8 = MakeRBindMul(8);
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto less2 = MakeRBindLess(2);
    auto less_neg_5 = MakeRBindLess(-5);
    auto greater3 = MakeRBindGreater(3);

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [x1_node, x1_pin] = builder.NewNodeWithPin(Type::Int(), "x1");
    auto [x3_node, x3_pin] = builder.NewNodeWithPin(Type::Int(), "x3");
    auto [x4_node, x4_pin] = builder.NewNodeWithPin(Type::Int(), "x4");
    auto [x5_node, x5_pin] = builder.NewNodeWithPin(Type::Int(), "x5");
    auto [x6_node, x6_pin] = builder.NewNodeWithPin(Type::Int(), "x6");
    auto [x7_node, x7_pin] = builder.NewNodeWithPin(Type::Int().Pow(2), "x7");
    auto& x9_node = builder.NewNode(Type::Int(), "x9");
    auto [x10_node, x10_pin] = builder.NewNodeWithPin(Type::Int(), "x10");
    auto [x11_node, x11_pin] = builder.NewNodeWithPin(Type::Int().Pow(2), "x11");
    auto& x12_node = builder.NewNode(Type::Int(), "x12");
    auto [x13_node, x13_pin] = builder.NewNodeWithPin(Type::Int(), "x13");
    auto [x14_node, x14_pin] = builder.NewNodeWithPin(Type::Int(), "x14");
    auto [x15_node, x15_pin] = builder.NewNodeWithPin(Type::Int(), "x15");
    auto& branch0_node = builder.NewNode(Type::Int(), "branch1");
    auto& branch1_node = builder.NewNode(Type::Int(), "branch2");

    auto& c1_pin = branch0_node.AddOutPin(Guard(less2));
    auto& c2_pin = branch0_node.AddOutPin(Pattern::Any());
    auto& c3_pin = branch1_node.AddOutPin(Guard(greater3));
    auto& c4_pin = branch1_node.AddOutPin(Guard(less_neg_5));
    auto& c5_pin = branch1_node.AddOutPin(Pattern::Any());

    builder.Connect(start_pin, x1_node, val)
        .Connect(x1_pin, branch0_node, plus3)
        .Connect(x1_pin, x3_node, minus2)
        .Connect(c1_pin, x4_node, mul8)
        .Connect(c2_pin, x5_node, mul4)
        .Connect(x4_pin, x6_node, plus2)
        .Connect(x5_pin, x6_node, plus1)
        .Connect(x6_pin, x7_node, pos0)
        .Connect(x3_pin, x7_node, pos1)
        .Connect(x7_pin, x10_node, plus)
        .Connect(x3_pin, branch1_node, mul5)
        .Connect(c3_pin, x13_node, minus8)
        .Connect(c4_pin, x14_node, mul3)
        .Connect(c5_pin, x9_node, plus7)
        .Connect(x13_pin, x15_node, mul2)
        .Connect(x14_pin, x15_node, minus5)
        .Connect(x15_pin, x11_node, pos0)
        .Connect(x10_pin, x11_node, pos1)
        .Connect(x11_pin, x12_node, mul);

    return builder.Extract();
}

int32_t CalcMegaIfResult(int32_t x) {
    int32_t branch0 = x + 3;
    int32_t x3 = x - 2;
    int32_t x6 = std::invoke([&]() {
        if (branch0 < 2) {
            return branch0 * 8 + 2;
        }
        return branch0 * 4 + 1;
    });
    int32_t x10 = x6 + x3;
    int32_t branch1 = x3 * 5;

    if (branch1 <= 3 & branch1 >= -5) {
        return branch1 + 7;  // x9
    }

    int32_t x15 = std::invoke([&]() {
        if (branch1 > 3) {
            return (branch1 - 8) * 2;
        }
        return branch1 * 3 - 5;
    });

    return x10 * x15;
}

lang::CatProgram MakeFibProgram(int32_t x) {
    auto val = MakeLiteralMorphism(x);
    auto fib = Morphism::Common("fib", Type::Int(), Type::Int());
    auto less2 = MakeRBindLess(2);
    auto id = Morphism::Identity();
    auto minus1 = MakeRBindMinus(1);
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto plus = Morphism::Plus();

    auto builder = CatProgramBuilder();

    auto& fib_node = builder.NewNode(Type::Int(), "fib");
    auto& x1_node = builder.NewNode(Type::Int());
    auto [x2_node, x2_pin] = builder.NewNodeWithPin(Type::Int());
    auto [x3_node, x3_pin] = builder.NewNodeWithPin(Type::Int());
    auto [x4_node, x4_pin] = builder.NewNodeWithPin(Type::Int());
    auto [x5_node, x5_pin] = builder.NewNodeWithPin(Type::Int());
    auto [x6_node, x6_pin] = builder.NewNodeWithPin(Type::Int().Pow(2));
    auto& x7_node = builder.NewNode(Type::Int());

    auto& c1_pin = fib_node.AddOutPin(Guard(less2));
    auto& c2_pin = fib_node.AddOutPin(Pattern::Any());

    // Connect fib function
    builder.Connect(c1_pin, x1_node, id)
        .Connect(c2_pin, x2_node, minus1)
        .Connect(x2_pin, x3_node, minus1)
        .Connect(x2_pin, x4_node, fib)
        .Connect(x3_pin, x5_node, fib)
        .Connect(x4_pin, x6_node, pos1)
        .Connect(x5_pin, x6_node, pos0)
        .Connect(x6_pin, x7_node, plus);

    auto [main_node, main_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [val_node, val_pin] = builder.NewNodeWithPin(Type::Int());
    auto& final_node = builder.NewNode(Type::Int());

    // Connect main function
    builder.Connect(main_pin, val_node, val).Connect(val_pin, final_node, fib);

    return builder.Extract();
}

translate::RawCatProgram MakeRawFibProgram(int32_t x) {
    translate::RawCatProgram prog;

    // fib
    auto snode = prog.NewRootNodeWithName("Int", "fib", {"| < 2", "*"});
    auto branch0_node = prog.NewEndNode("Int");
    auto branch1_node = prog.NewNodeWithName("Int", "mop");
    auto minus2_node = prog.NewNode("Int");
    auto minus1_fib_node = prog.NewNodeWithName("Int", "x");
    auto minus2_fib_node = prog.NewNode("Int");
    auto pair_node = prog.NewNodeWithName("Int x Int", "y");
    auto sum_node = prog.NewEndNode("Int");

    prog.Connect(snode, branch0_node, 0, "id")
        .Connect(snode, branch1_node, 1, "- 1")
        .Connect(branch1_node, minus2_node, 0, "- 1")
        .Connect(branch1_node, minus1_fib_node, 0, "fib")
        .Connect(minus2_node, minus2_fib_node, 0, "fib")
        .Connect(minus2_fib_node, pair_node, 0, "$0")
        .Connect(minus1_fib_node, pair_node, 0, "$1")
        .Connect(pair_node, sum_node, 0, "+");

    // main
    auto main_node = prog.NewRootNodeWithName("S", "main");
    auto val_node = prog.NewNodeWithName("Int", "kek");
    auto final_node = prog.NewEndNode("Int");

    prog.Connect(main_node, val_node, 0, std::to_string(x)).Connect(val_node, final_node, 0, "fib");

    return prog;
}

lang::CatProgram MakeIO101Program() {
    auto at = Type::Var("a");
    auto bt = Type::Var("b");
    auto ct = Type::Var("c");
    auto io_a = Type::Parameterized("IO", {at});
    auto io_s = Type::Parameterized("IO", {Type::Singleton()});
    auto ma = lang::Type::Parameterized("m", {at});
    auto mb = lang::Type::Parameterized("m", {bt});
    auto mc = lang::Type::Parameterized("m", {ct});
    auto io_int = Type::Parameterized("IO", {Type::Int()});
    auto io_int2 = io_int.Pow(2);
    auto read = Morphism::Value("readLn", io_a);
    auto id = Morphism::Identity();
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);

    auto lift_m2 = lang::Morphism::ChainFunction(
        "liftM2", {lang::Type::FunctionChain({at, bt, ct}), ma, mb, mc});
    auto lifted_plus = Morphism::Binded(lift_m2, {{0, Morphism::Plus()}});
    auto bind = Morphism::ChainFunction(">>=", {ma, lang::Type::Function(at, mb), mb});
    auto print = lang::Morphism::Common("print", at, io_s);
    auto binded_print = lang::Morphism::Binded(bind, {{1, print}});

    auto builder = CatProgramBuilder();

    auto [main_node, main_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [read_node, read_pin] = builder.NewNodeWithPin(io_int);
    auto [num0_node, num0_pin] = builder.NewNodeWithPin(io_int);
    auto [num1_node, num1_pin] = builder.NewNodeWithPin(io_int);
    auto [pair_node, pair_pin] = builder.NewNodeWithPin(io_int2);
    auto [sum_node, sum_pin] = builder.NewNodeWithPin(io_int);
    auto& final_node = builder.NewNode(io_s);

    builder.Connect(main_pin, read_node, read)
        .Connect(read_pin, num0_node, id)
        .Connect(read_pin, num1_node, id)
        .Connect(num0_pin, pair_node, pos0)
        .Connect(num1_pin, pair_node, pos1)
        .Connect(pair_pin, sum_node, lifted_plus)
        .Connect(sum_pin, final_node, binded_print);

    return builder.Extract();
}

translate::RawCatProgram MakeRawIO101Program() {
    translate::RawCatProgram prog;

    auto main_node = prog.NewRootNodeWithName("S", "main");
    auto read_node = prog.NewNode("IO Int");
    auto num0_node = prog.NewNode("IO Int");
    auto num1_node = prog.NewNode("IO Int");
    auto pair_node = prog.NewNode("IO Int x IO Int");
    auto sum_node = prog.NewNode("IO Int");
    auto final_node = prog.NewEndNode("IO S");

    prog.Connect(main_node, read_node, 0, "read")
        .Connect(read_node, num0_node, 0, "id")
        .Connect(read_node, num1_node, 0, "id")
        .Connect(num0_node, pair_node, 0, "$0")
        .Connect(num1_node, pair_node, 0, "$1")
        .Connect(pair_node, sum_node, 0, "liftM2 +")
        .Connect(sum_node, final_node, 0, ">>= print");

    return prog;
}

}  // namespace komaru::test
