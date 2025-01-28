#pragma once
#include <lang/type.hpp>
#include <lang/morphism.hpp>

namespace komaru::lang {

// Program's category is just a DAG of types and morphisms between them
class Category {
public:
    struct Link;

    struct Node {
        const Type* type;
        std::vector<Link> links;
    };

    struct Link {
        const Morphism* morphism;
        Node* dst;
    };

    using DAG = std::vector<Node>;

public:
    explicit Category(DAG dag);

    // A joke of an interface :(
    DAG& GetDAG();

private:
    bool Validate(); // validate dag

private:
    DAG dag_;
};

}
