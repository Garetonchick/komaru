#include <gtest/gtest.h>

#include <lang/category.hpp>
#include <translate/cpp/cpp_translator.hpp>
#include <translate/exec_program.hpp>

#include <print>

using namespace komaru::lang;
using namespace komaru::translate;

TEST(CppTranslator, APlusB) {
    auto ti = Type::FromTag(TypeTag::Int);
    auto ti2 = Type::TupleFromTags({TypeTag::Int, TypeTag::Int});
    auto start_morphism = Morphism::Builtin(
        MorphismTag::Id, Type::FromTag(TypeTag::Source), Type::FromTag(TypeTag::Singleton)
    );
    auto value_morphism = Morphism::WithValue(
        "some_value", Value::TupleFromAtoms(9, 42)
    );
    auto plus_morphism = Morphism::Builtin(
        MorphismTag::Plus, ti2, ti
    );
    auto end_morphism = Morphism::Builtin(
        MorphismTag::Id, Type::FromTag(TypeTag::Int), Type::FromTag(TypeTag::Target)
    );
    std::vector<MorphismPtr> program_morphisms = {
        start_morphism, value_morphism, plus_morphism, end_morphism
    };
    auto program_morphism = Morphism::Compound("Main", program_morphisms);

    Category::DAG dag = {
        Category::Node{
            program_morphism->GetSource(),
            {}
        },
        Category::Node{
            program_morphism->GetTarget(),
            {}
        },
    };

    dag[1].links.push_back(Category::Link{program_morphism, &dag[0]});

    Category category(std::move(dag));

    std::unique_ptr<ITranslator> translator = std::make_unique<cpp::CppTranslator>();
    auto program = translator->Translate(category);

    auto exec_res = ExecProgram(*program);

    ASSERT_TRUE(exec_res.Success());
    ASSERT_EQ(exec_res.Output(), "51\n");
}
