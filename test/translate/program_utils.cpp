#include "program_utils.hpp"

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <komaru/translate/exec_program.hpp>

namespace komaru::test {

lang::MorphismPtr MakeIntBinaryOp(lang::MorphismTag tag, lang::Type target) {
    return lang::Morphism::Builtin(tag, lang::Type::Int().Pow(2), target);
}

lang::MorphismPtr MakeIntPlus() {
    return MakeIntBinaryOp(lang::MorphismTag::Plus);
}

lang::MorphismPtr MakeIntMul() {
    return MakeIntBinaryOp(lang::MorphismTag::Multiply);
}

lang::MorphismPtr MakeIntMinus() {
    return MakeIntBinaryOp(lang::MorphismTag::Minus);
}

lang::MorphismPtr MakeIntLess() {
    return MakeIntBinaryOp(lang::MorphismTag::Less, lang::Type::Bool());
}

lang::MorphismPtr MakeIntGreater() {
    return MakeIntBinaryOp(lang::MorphismTag::Greater, lang::Type::Bool());
}

lang::MorphismPtr MakeRBindIntBinaryOp(lang::MorphismTag tag, int32_t x, lang::Type target) {
    return lang::BindMorphism(MakeIntBinaryOp(tag, target),
                              {{1, lang::Morphism::WithValue("", lang::Value::Atom(x))}});
}

lang::MorphismPtr MakeRBindIntPlus(int32_t x) {
    return MakeRBindIntBinaryOp(lang::MorphismTag::Plus, x);
}

lang::MorphismPtr MakeRBindIntMul(int32_t x) {
    return MakeRBindIntBinaryOp(lang::MorphismTag::Multiply, x);
}

lang::MorphismPtr MakeRBindIntMinus(int32_t x) {
    return MakeRBindIntBinaryOp(lang::MorphismTag::Minus, x);
}

lang::MorphismPtr MakeRBindIntLess(int32_t x) {
    return MakeRBindIntBinaryOp(lang::MorphismTag::Less, x, lang::Type::Bool());
}

lang::MorphismPtr MakeRBindIntGreater(int32_t x) {
    return MakeRBindIntBinaryOp(lang::MorphismTag::Greater, x, lang::Type::Bool());
}

}  // namespace komaru::test
