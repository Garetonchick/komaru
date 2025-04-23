#pragma once
#include <vector>
#include <string>

namespace komaru::translate::cpp {

class CppExpr {
public:
    explicit CppExpr(std::vector<std::string> subexprs);
    CppExpr(std::string expr, size_t n_components);

    size_t NumSubexprs() const;
    const std::string& AsWholeExpr() const;
    const std::vector<std::string>& GetSubexprs() const;
    std::vector<std::string> Cook(size_t n_components) const;

private:
    std::string whole_;
    std::vector<std::string> subexprs_;
};

}  // namespace komaru::translate::cpp
