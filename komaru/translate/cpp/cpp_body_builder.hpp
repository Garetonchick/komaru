#pragma once

#include <komaru/translate/cpp/cpp_cond.hpp>
#include <komaru/translate/translator.hpp>

#include <string>
#include <list>
#include <vector>
#include <optional>
#include <deque>

namespace komaru::translate::cpp {

class CppScope;

class CppBranches {
public:
    struct Branch {
        std::string expr;
        CppScope* scope;
    };

public:
    template <std::ranges::input_range R1, std::ranges::input_range R2>
    CppBranches(const R1& exprs, const R2& scopes) {
        auto it1 = std::ranges::begin(exprs);
        auto it2 = std::ranges::begin(scopes);
        auto end1 = std::ranges::end(exprs);
        auto end2 = std::ranges::end(scopes);

        while (it1 != end1 && it2 != end2) {
            branches_.emplace_back(*it1, *it2);
            ++it1;
            ++it2;
        }

        if (it1 != end1 || it2 != end2) {
            throw std::runtime_error(
                "mismatch between number of exprs and number of scopes in CppBranches");
        }
    }

    const std::vector<Branch>& GetBranches() const;

private:
    std::vector<Branch> branches_;
};

class CppScope {
public:
    explicit CppScope(CppCond cond);

    void GrowBody(const std::string& s);
    void SetBranches(CppBranches branches);

    const CppCond& GetCond() const;
    const std::string& GetBody() const;
    const std::optional<CppBranches>& GetBranches() const;

private:
    CppCond cond_;
    std::string body_;
    std::optional<CppBranches> branches_;
};

class CppBodyBuilder {
public:
    CppBodyBuilder();

    void AddStatement(const CppCond& cond, std::string statement);
    void AddReturn(const CppCond& cond, std::string statement);
    std::vector<CppCond> AddBranches(const CppCond& cond,
                                     const std::vector<std::string>& branch_exprs);

    TranslationResult<std::string> Extract();
    void Reset();

private:
    std::string ExtractImpl(const CppScope* scope);

private:
    std::deque<CppScope> scopes_;
    std::list<CppScope*> active_scopes_;
    int32_t branch_id_{0};
};

}  // namespace komaru::translate::cpp
