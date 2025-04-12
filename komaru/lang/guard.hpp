#pragma once
#include <lang/morphism.hpp>

namespace komaru::lang {

// holds a morphism: a -> Bool
class Guard {
public:
    explicit Guard(MorphismPtr morphism);

    const Morphism& GetMorphism() const;

private:
    MorphismPtr morphism_;
};

}
