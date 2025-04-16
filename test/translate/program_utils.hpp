#pragma once
#include <komaru/lang/morphism.hpp>
#include <komaru/lang/cat_program.hpp>

namespace komaru::test {

template <typename T>
lang::MorphismPtr MakeAtomValueMorphism(T value) {
    return lang::Morphism::WithValue("", lang::Value::Atom(value));
}

lang::MorphismPtr MakeIntBinaryOp(lang::MorphismTag tag, lang::Type target = lang::Type::Int());
lang::MorphismPtr MakeIntPlus();
lang::MorphismPtr MakeIntMul();
lang::MorphismPtr MakeIntMinus();
lang::MorphismPtr MakeIntLess();
lang::MorphismPtr MakeIntGreater();

lang::MorphismPtr MakeRBindIntBinaryOp(lang::MorphismTag tag, int32_t x,
                                       lang::Type target = lang::Type::Int());
lang::MorphismPtr MakeRBindIntPlus(int32_t x);
lang::MorphismPtr MakeRBindIntMul(int32_t x);
lang::MorphismPtr MakeRBindIntMinus(int32_t x);
lang::MorphismPtr MakeRBindIntLess(int32_t x);
lang::MorphismPtr MakeRBindIntGreater(int32_t x);

}  // namespace komaru::test
