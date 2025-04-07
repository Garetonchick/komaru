#include "cpp_translator.hpp"

#include <translate/cpp/cpp_program.hpp>
#include <translate/cpp/cpp_function_builder.hpp>
#include <translate/cpp/cpp_value.hpp>
#include <util/std_extensions.hpp>
#include <util/string.hpp>

#include <cassert>
#include <print>


namespace komaru::translate::cpp {

const std::unordered_map<std::string, std::string> CppTranslator::name_conv_ = {
    {"+", "Plus"},
    {"-", "Minus"},
    {"*", "Mul"},
};

std::unique_ptr<IProgram> CppTranslator::Translate(const lang::Category& cat) {
    // TODO: Move logic from cpp translator to separate class and create it on each translate call
    // Resetting program builder
    new (&builder_) CppProgramBuilder();

    const auto& dag = cat.GetDAG();
    assert(dag.size() == 2 && "Only one morphism dags are supported for now");

    lang::MorphismPtr main_morphism;

    if(!dag[0].links.empty()) {
        main_morphism = dag[0].links[0].morphism;
    } else {
        main_morphism = dag[1].links[0].morphism;
    }

    if(main_morphism->GetSource() != lang::Type::Source() ||
       main_morphism->GetTarget() != lang::Type::Target()) {
        throw std::invalid_argument("main morphism must go from Source to Target");
    }

    TranslateMorphism(*main_morphism);

    builder_.AddHeader("cstdint");
    builder_.AddHeader("tuple");
    builder_.AddHeader("variant");
    builder_.AddHeader("iostream");

    auto main_cpp_func = CppFunctionBuilder()
        .SetName("main")
        .SetReturnType(lang::Type::Int())
        .SetBody(std::format("std::cout << {}({{}}) << std::endl;", main_morphism->GetName()))
        .Extract();

    builder_.AddFunction("main", std::move(main_cpp_func));

    return std::move(builder_).ExtractProgram();
}

void CppTranslator::TranslateMorphism(const lang::Morphism& morphism) {
    // Ignore recursion for now
    // Also it doesn't handle function overloading either
    // TODO: Adress issues above
    if(builder_.HasFunction(morphism.GetName())) {
        return;
    }

    morphism.Visit([this](const auto& morphism) {
        TranslateMorphism(morphism);
    });
}

void CppTranslator::TranslateMorphism(const lang::CompoundMorphism& morphism) {
    std::string body = "return ";

    for(const auto& m : morphism.GetMorphisms() | std::views::reverse) {
        TranslateMorphism(*m);
        auto cpp_name = MorphismName2Cpp(m->GetName());
        body += cpp_name + "(" ;
    }

    body += "arg" + std::string(morphism.GetMorphisms().size(), ')') + ";";

    auto func = CppFunctionBuilder()
        .SetName(morphism.GetName())
        .AddInputParameter(morphism.GetSource(), "arg")
        .SetReturnType(morphism.GetTarget())
        .SetBody(std::move(body))
        .Extract();

    builder_.AddFunction(morphism.GetName(), std::move(func));
}

void CppTranslator::TranslateMorphism(const lang::BuiltinMorphism& morphism) {
    // TODO: better dispatch technique and cleanup this function in general

    std::string id_body = "return arg;";
    std::string plus_body = "return std::get<0>(arg) + std::get<1>(arg);";

    auto builder = CppFunctionBuilder()
        .SetName(MorphismName2Cpp(morphism.GetName()))
        .SetReturnType(lang::Type::Auto());

    switch(morphism.GetTag()) {
        case lang::MorphismTag::Plus: {
            auto in_type = lang::Type::Tuple(
                {lang::Type::Generic("T"), lang::Type::Generic("T")}
            );
            auto func = builder
                .AddInputParameter(in_type, "arg")
                .SetBody(plus_body)
                .Extract();

            builder_.AddFunction(morphism.GetName(), std::move(func));
            break;
        }
        case lang::MorphismTag::Id: {
            auto func = builder
                .AddInputParameter(lang::Type::Generic("T"), "arg")
                .SetBody(id_body)
                .Extract();

            builder_.AddFunction(morphism.GetName(), std::move(func));
            break;
        }
        default:
            throw std::runtime_error("Unsupported builtin morphism for cpp translation");
    }
}

void CppTranslator::TranslateMorphism(const lang::ValueMorphism& morphism) {
    const auto& value = morphism.GetValue();

    std::string body = "return " + ToCppValue(value) + ";";

    auto func = CppFunctionBuilder()
        .SetName(morphism.GetName())
        .AddInputParameter(morphism.GetSource(), "arg")
        .SetReturnType(morphism.GetTarget())
        .SetBody(std::move(body))
        .Extract();

    builder_.AddFunction(morphism.GetName(), std::move(func));
}

std::string CppTranslator::MorphismName2Cpp(const std::string& mname) const {
    auto it = name_conv_.find(mname);
    if(it == name_conv_.end()) {
        return mname;
    }

    return it->second;
}

}
