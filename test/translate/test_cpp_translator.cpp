#include <gtest/gtest.h>

#include <lang/category.hpp>
#include <translate/cpp_translator.hpp>

#include <print>

using namespace komaru::lang;
using namespace komaru::translate;

TEST(CppTranslator, APlusB) {
    Category::DAG dag = {

    };
    Category category(std::move(dag));

    std::unique_ptr<ITranslator> translator = std::make_unique<CppTranslator>();
    auto program = translator->Translate(category);

    std::println("main.cpp\n{}", program->GetProgramSourceCode());
}
