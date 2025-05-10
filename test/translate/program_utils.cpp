#include "program_utils.hpp"

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>

namespace komaru::test {

lang::MorphismPtr MakeRBindOp(const lang::MorphismPtr& morphism, int32_t x) {
    return lang::Morphism::Binded(morphism,
                                  {{1, lang::Morphism::Literal(lang::Literal::Number(x))}});
}

lang::MorphismPtr MakeRBindPlus(int32_t x) {
    return MakeRBindOp(lang::Morphism::Plus(), x);
}

lang::MorphismPtr MakeRBindMul(int32_t x) {
    return MakeRBindOp(lang::Morphism::Multiply(), x);
}

lang::MorphismPtr MakeRBindMinus(int32_t x) {
    return MakeRBindOp(lang::Morphism::Minus(), x);
}

lang::MorphismPtr MakeRBindLess(int32_t x) {
    return MakeRBindOp(lang::Morphism::Less(), x);
}

lang::MorphismPtr MakeRBindGreater(int32_t x) {
    return MakeRBindOp(lang::Morphism::Greater(), x);
}

}  // namespace komaru::test
