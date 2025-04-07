#pragma once
#include <translate/translator.hpp>
#include <translate/cpp/cpp_program_builder.hpp>

#include <unordered_map>

namespace komaru::translate::cpp {

class CppTranslator : public ITranslator {
public:
    std::unique_ptr<IProgram> Translate(const lang::Category& category) override;

private:
    void TranslateMorphism(const lang::Morphism& morphism);
    void TranslateMorphism(const lang::CompoundMorphism& morphism);
    void TranslateMorphism(const lang::BuiltinMorphism& morphism);
    void TranslateMorphism(const lang::ValueMorphism& morphism);

    std::string MorphismName2Cpp(const std::string& mname) const;

private:
    CppProgramBuilder builder_;

    static const std::unordered_map<std::string, std::string> name_conv_;
};

}
