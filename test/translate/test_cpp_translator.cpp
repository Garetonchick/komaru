#include <gtest/gtest.h>

#include <lang/cat_program.hpp>
#include <translate/cpp/cpp_translator.hpp>
#include <translate/exec_program.hpp>

#include <print>

using namespace komaru::lang;
using namespace komaru::translate;

/*
 *   9      $0
 *   ┌─>Int──┐              +
 * S─┤       ├>|Int x Int|─────>Int
 *   └─>Int──┘
 *   42     $1
 */

TEST(CppTranslator, APlusB) {
    auto nine = Morphism::WithValue("", Value::Atom(9));
    auto forty_two = Morphism::WithValue("", Value::Atom(42));
    auto pos0 = Morphism::Position(0);
    auto pos1 = Morphism::Position(1);
    auto plus = Morphism::Builtin(MorphismTag::Plus, Type::Int().Pow(2), Type::Int());

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

    auto cat_program = builder.Extract();

    std::unique_ptr<ITranslator> translator = std::make_unique<cpp::CppTranslator>();
    auto maybe_program = translator->Translate(cat_program);

    ASSERT_TRUE(maybe_program.has_value());
    auto program = std::move(maybe_program.value());

    std::println("aplusb.cpp\n{}\n", program->GetSourceCode());

    auto exec_res = ExecProgram(*program);

    ASSERT_TRUE(exec_res.Success());
    ASSERT_EQ(exec_res.Output(), "51\n");
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
    auto val = Morphism::WithValue("", Value::Atom(5));
    auto less4 =
        BindMorphism(Morphism::Builtin(MorphismTag::Less, Type::Int().Pow(2), Type::Bool()),
                     {{1, Value::Atom(4)}});
    auto pos0 = Morphism::Position(0);
    auto none_pos = Morphism::NonePosition();
    auto mul15 =
        BindMorphism(Morphism::Builtin(MorphismTag::Multiply, Type::Int().Pow(2), Type::Int()),
                     {{1, Value::Atom(15)}});
    auto add10 = BindMorphism(Morphism::Builtin(MorphismTag::Plus, Type::Int().Pow(2), Type::Int()),
                              {{1, Value::Atom(10)}});

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

    auto cat_program = builder.Extract();

    std::unique_ptr<ITranslator> translator = std::make_unique<cpp::CppTranslator>();
    auto maybe_program = translator->Translate(cat_program);

    ASSERT_TRUE(maybe_program.has_value());

    auto program = std::move(maybe_program.value());

    std::println("if101.cpp\n{}\n", program->GetSourceCode());

    auto exec_res = ExecProgram(*program);

    ASSERT_TRUE(exec_res.Success());
    ASSERT_EQ(exec_res.Output(), "75\n");
}

/*
 *               +10
 *   5  ┌───|<4|────>Int
 * S───>│Int|  | *15
 *      └───|* |────>Int
 */

TEST(CppTranslator, Guards101) {
    auto val = Morphism::WithValue("", Value::Atom(5));
    auto less4_guard =
        Guard(BindMorphism(Morphism::Builtin(MorphismTag::Less, Type::Int().Pow(2), Type::Bool()),
                           {{1, Value::Atom(4)}}));
    auto mul15 =
        BindMorphism(Morphism::Builtin(MorphismTag::Multiply, Type::Int().Pow(2), Type::Int()),
                     {{1, Value::Atom(15)}});
    auto add10 = BindMorphism(Morphism::Builtin(MorphismTag::Plus, Type::Int().Pow(2), Type::Int()),
                              {{1, Value::Atom(10)}});

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

    auto cat_program = builder.Extract();

    std::unique_ptr<ITranslator> translator = std::make_unique<cpp::CppTranslator>();
    auto maybe_program = translator->Translate(cat_program);

    ASSERT_TRUE(maybe_program.has_value());

    auto program = std::move(maybe_program.value());

    std::println("guards101.cpp\n{}\n", program->GetSourceCode());

    auto exec_res = ExecProgram(*program);

    ASSERT_TRUE(exec_res.Success());
    ASSERT_EQ(exec_res.Output(), "75\n");
}
