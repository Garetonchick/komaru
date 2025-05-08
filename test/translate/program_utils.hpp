#pragma once
#include <komaru/lang/morphism.hpp>
#include <komaru/lang/cat_program.hpp>

namespace komaru::test {

template <typename T>
lang::MorphismPtr MakeLiteralMorphism(const T& value) {
    return lang::Morphism::Literal(lang::Literal::Make(value));
}

lang::MorphismPtr MakeRBindIntPlus(int32_t x);
lang::MorphismPtr MakeRBindIntMul(int32_t x);
lang::MorphismPtr MakeRBindIntMinus(int32_t x);
lang::MorphismPtr MakeRBindIntLess(int32_t x);
lang::MorphismPtr MakeRBindIntGreater(int32_t x);

}  // namespace komaru::test
