#pragma once
#include <lang/type.hpp>
#include <lang/morphism.hpp>

namespace komaru::lang {

// Program's category is just a DAG of types and morphisms between them
class Category {
public:
    struct Link;

    struct Node {
        Type type;
        std::vector<const Link> links;
    };

    struct Link {
        MorphismPtr morphism;
        const Node* dst; // Watch out for dangling pointers
    };

    using DAG = std::vector<Node>;

public:
    explicit Category(DAG dag);

    // A joke of an interface :(
    const DAG& GetDAG() const;

private:
    bool Validate(); // validate dag

private:
    DAG dag_;
};

}
