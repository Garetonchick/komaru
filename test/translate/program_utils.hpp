#pragma once
#include <komaru/lang/morphism.hpp>
#include <komaru/lang/cat_program.hpp>

namespace komaru::test {

template <typename T>
lang::MorphismPtr MakeLiteralMorphism(const T& value) {
    return lang::Morphism::Literal(lang::Literal::Make(value));
}

lang::MorphismPtr MakeRBindPlus(int32_t x);
lang::MorphismPtr MakeRBindMul(int32_t x);
lang::MorphismPtr MakeRBindMinus(int32_t x);
lang::MorphismPtr MakeRBindLess(int32_t x);
lang::MorphismPtr MakeRBindGreater(int32_t x);

}  // namespace komaru::test
