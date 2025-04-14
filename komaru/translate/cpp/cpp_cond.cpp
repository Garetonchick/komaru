#include "cpp_cond.hpp"

#include <komaru/util/string.hpp>

#include <set>

namespace komaru::translate::cpp {

CppCond::CppCond() {
}

CppCond::CppCond(int32_t var_idx) {
    formula_ = {1, var_idx >= 0 ? var_idx + 1 : var_idx - 1};
}

CppCond CppCond::operator|(const CppCond& o) const {
    auto copy = *this;
    return copy |= o;
}

CppCond CppCond::operator&(const CppCond& o) const {
    if (formula_.empty()) {
        return o;
    }
    if (o.formula_.empty()) {
        return *this;
    }

    CppCond cond;

    for (size_t i = 0; i < formula_.size(); i += formula_[i] + 1) {
        int32_t n1 = formula_[i];
        for (size_t j = 0; j < o.formula_.size(); j += o.formula_[j] + 1) {
            int32_t n2 = o.formula_[j];

            cond.formula_.push_back(n1 + n2);
            cond.formula_.append_range(
                std::ranges::subrange(formula_.begin() + i + 1, formula_.begin() + i + 1 + n1));
            cond.formula_.append_range(
                std::ranges::subrange(o.formula_.begin() + j + 1, o.formula_.begin() + j + 1 + n2));
        }
    }

    return cond;
}

CppCond& CppCond::operator|=(const CppCond& o) {
    if (formula_.empty() || o.formula_.empty()) {
        formula_ = {};
        return *this;
    }

    formula_.insert_range(formula_.end(), o.formula_);
    return *this;
}

CppCond& CppCond::operator&=(const CppCond& o) {
    return *this = *this & o;
}

std::string CppCond::ToString() const {
    if (formula_.empty()) {
        return "true";
    }

    std::string res;

    auto var_to_string = [](int32_t var) {
        if (var > 0) {
            return "v" + std::to_string(var - 1);
        }
        return "~v" + std::to_string(-(var + 1));
    };

    for (size_t i = 0; i < formula_.size(); i += formula_[i] + 1) {
        int32_t n = formula_[i];
        if (n == 1) {
            res += var_to_string(formula_[i + 1]);
        } else {
            res += "(";
            for (int32_t j = 0; j < n; ++j) {
                res += var_to_string(formula_[i + 1 + j]);
                if (j + 1 != n) {
                    res += " & ";
                }
            }
            res += ")";
        }

        if (i + n + 1 < formula_.size()) {
            res += " | ";
        }
    }

    return res;
}

bool CppCond::DoesImply(const CppCond& o) const {
    if (o.formula_.empty()) {
        return true;
    }

    for (size_t i = 0; i < formula_.size(); i += formula_[i] + 1) {
        int32_t n1 = formula_[i];
        std::set<int32_t> st(formula_.begin() + i + 1, formula_.begin() + i + 1 + n1);

        for (size_t j = 0; j < o.formula_.size(); j += o.formula_[j] + 1) {
            int32_t n2 = o.formula_[j];

            bool ok = true;

            for (int32_t var_idx : std::ranges::subrange(o.formula_.begin() + j + 1,
                                                         o.formula_.begin() + j + 1 + n2)) {
                ok = ok && st.contains(var_idx);
            }

            if (ok) {
                return true;
            }
        }
    }

    return false;
}

}  // namespace komaru::translate::cpp
