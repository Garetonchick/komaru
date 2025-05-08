#include "program_utils.hpp"

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>

namespace komaru::test {

lang::MorphismPtr MakeRBindIntOp(const lang::MorphismPtr& morphism, int32_t x) {
    return lang::Morphism::Binded(morphism,
                                  {{1, lang::Morphism::Literal(lang::Literal::Number(x))}});
}

lang::MorphismPtr MakeRBindIntPlus(int32_t x) {
    return MakeRBindIntOp(lang::Morphism::Plus(), x);
}

lang::MorphismPtr MakeRBindIntMul(int32_t x) {
    return MakeRBindIntOp(lang::Morphism::Multiply(), x);
}

lang::MorphismPtr MakeRBindIntMinus(int32_t x) {
    return MakeRBindIntOp(lang::Morphism::Minus(), x);
}

lang::MorphismPtr MakeRBindIntLess(int32_t x) {
    return MakeRBindIntOp(lang::Morphism::Less(), x);
}

lang::MorphismPtr MakeRBindIntGreater(int32_t x) {
    return MakeRBindIntOp(lang::Morphism::Greater(), x);
}

}  // namespace komaru::test
