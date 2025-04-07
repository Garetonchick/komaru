#include "category.hpp"

namespace komaru::lang {

Category::Category(DAG dag) : dag_(std::move(dag)) {
}

const Category::DAG& Category::GetDAG() const {
    return dag_;
}

}
