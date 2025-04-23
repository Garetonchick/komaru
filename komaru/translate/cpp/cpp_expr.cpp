#include "cpp_expr.hpp"

#include <format>

namespace komaru::translate::cpp {

CppExpr::CppExpr(std::vector<std::string> subexprs)
    : subexprs_(subexprs) {
    if (subexprs.size() == 1) {
        whole_ = subexprs[0];
        return;
    }

    whole_ = "std::make_tuple(";

    for (size_t i = 0; i < subexprs_.size(); ++i) {
        whole_ += subexprs_[i];
        if (i + 1 != subexprs_.size()) {
            whole_ += ", ";
        }
    }
    whole_ += ")";
}

CppExpr::CppExpr(std::string expr, size_t n_components)
    : whole_(expr) {
    if (n_components == 1) {
        subexprs_.push_back(expr);
        return;
    }
    for (size_t i = 0; i < n_components; ++i) {
        subexprs_.push_back(std::format("std::get<{}>({})", i, expr));
    }
}

size_t CppExpr::NumSubexprs() const {
    return subexprs_.size();
}

const std::string& CppExpr::AsWholeExpr() const {
    return whole_;
}

const std::vector<std::string>& CppExpr::GetSubexprs() const {
    return subexprs_;
}

std::vector<std::string> CppExpr::Cook(size_t n_components) const {
    if (!n_components) {
        return {};
    }

    if (n_components == 1) {
        return {whole_};
    }

    if (n_components == NumSubexprs()) {
        return subexprs_;
    }

    std::vector<std::string> exprs;
    exprs.reserve(n_components);

    for (size_t i = 0; i < n_components; ++i) {
        exprs.emplace_back(std::format("std::get<{}>({})", i, whole_));
    }

    return exprs;
}

}  // namespace komaru::translate::cpp
