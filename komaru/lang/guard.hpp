#pragma once
#include <komaru/lang/morphism.hpp>

namespace komaru::lang {

// holds a morphism: a -> Bool
class Guard {
public:
    explicit Guard(MorphismPtr morphism);

    const Morphism& GetMorphism() const;
    std::string ToString(MorphismPtr arg_morphism = Morphism::CommonWithType("x", Type::Var("a")),
                         Style style = Style::Komaru) const;

private:
    MorphismPtr morphism_;
};

}  // namespace komaru::lang
