#include "programs.hpp"

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>
#include <test/translate/program_utils.hpp>

using namespace komaru::lang;

namespace komaru::test {

lang::CatProgram MakeAPlusBProgram(int32_t a, int32_t b) {
    auto nine = MakeAtomValueMorphism(a);
    auto forty_two = MakeAtomValueMorphism(b);
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto plus = MakeIntPlus();

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [num0_node, num0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [num1_node, num1_pin] = builder.NewNodeWithPin(Type::Int());
    auto [pair_node, pair_pin] = builder.NewNodeWithPin(Type::Int().Pow(2));
    auto& res_node = builder.NewNode(Type::Int());

    builder.Connect(start_pin, num0_node, nine)
        .Connect(start_pin, num1_node, forty_two)
        .Connect(num0_pin, pair_node, pos0)
        .Connect(num1_pin, pair_node, pos1)
        .Connect(pair_pin, res_node, plus);

    return builder.Extract();
}

lang::CatProgram MakeIf101Program(int32_t x) {
    auto val = MakeAtomValueMorphism(x);
    auto less4 = MakeRBindIntLess(4);
    auto pos0 = Morphism::Position(0);
    auto none_pos = Morphism::NonePosition();
    auto mul15 = MakeRBindIntMul(15);
    auto add10 = MakeRBindIntPlus(10);

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [val_node, val_pin] = builder.NewNodeWithPin(Type::Int());
    auto& cond_node = builder.NewNode(Type::Bool());
    auto [branch0_node, branch0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [branch1_node, branch1_pin] = builder.NewNodeWithPin(Type::Int());
    auto& res0_node = builder.NewNode(Type::Int());
    auto& res1_node = builder.NewNode(Type::Int());

    auto& false_pin = cond_node.AddOutPin(Pattern::FromValue(Value::Atom(false)));
    auto& true_pin = cond_node.AddOutPin(Pattern::FromValue(Value::Atom(true)));

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
    auto val = MakeAtomValueMorphism(x);
    auto less4 = MakeRBindIntLess(4);
    auto mul15 = MakeRBindIntMul(15);
    auto add10 = MakeRBindIntPlus(10);
    auto y_var =
        Morphism::WithName("y", Type::Auto(), Type::Int());  // Cheat a bit with the source type

    auto builder = CatProgramBuilder();

    auto [start_node, start_pin] = builder.NewNodeWithPin(Type::Singleton(), "main");
    auto [val_node, val_pin] = builder.NewNodeWithPin(Type::Int(), "y");
    auto& cond_node = builder.NewNode(Type::Bool());
    auto [branch0_node, branch0_pin] = builder.NewNodeWithPin(Type::Int());
    auto [branch1_node, branch1_pin] = builder.NewNodeWithPin(Type::Int());
    auto& res0_node = builder.NewNode(Type::Int());
    auto& res1_node = builder.NewNode(Type::Int());

    auto& false_pin = cond_node.AddOutPin(Pattern::FromValue(Value::Atom(false)));
    auto& true_pin = cond_node.AddOutPin(Pattern::FromValue(Value::Atom(true)));

    builder.Connect(start_pin, val_node, val)
        .Connect(val_pin, cond_node, less4)
        .Connect(false_pin, branch0_node, y_var)
        .Connect(true_pin, branch1_node, y_var)
        .Connect(branch0_pin, res0_node, mul15)
        .Connect(branch1_pin, res1_node, add10);

    return builder.Extract();
}

lang::CatProgram MakeGuards101Program(int32_t x) {
    auto val = MakeAtomValueMorphism(x);
    auto less4_guard = Guard(MakeRBindIntLess(4));
    auto mul15 = MakeRBindIntMul(15);
    auto add10 = MakeRBindIntPlus(10);

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
    auto val = MakeAtomValueMorphism(x);
    auto plus = MakeIntPlus();
    auto plus1 = MakeRBindIntPlus(1);
    auto plus2 = MakeRBindIntPlus(2);
    auto plus3 = MakeRBindIntPlus(3);
    auto plus7 = MakeRBindIntPlus(7);
    auto plus20 = MakeRBindIntPlus(20);
    auto minus2 = MakeRBindIntMinus(2);
    auto minus5 = MakeRBindIntMinus(5);
    auto minus8 = MakeRBindIntMinus(8);
    auto mul = MakeIntMul();
    auto mul2 = MakeRBindIntMul(2);
    auto mul3 = MakeRBindIntMul(3);
    auto mul4 = MakeRBindIntMul(4);
    auto mul5 = MakeRBindIntMul(5);
    auto mul8 = MakeRBindIntMul(8);
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto less2 = MakeRBindIntLess(2);
    auto less_neg_5 = MakeRBindIntLess(-5);
    auto greater3 = MakeRBindIntGreater(3);

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

}  // namespace komaru::test
