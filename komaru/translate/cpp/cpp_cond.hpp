#pragma once
#include <cstdint>
#include <vector>

namespace komaru::translate::cpp {

// Boolean formula stored as DNF
// TODO: Unit test
class CppCond {
public:
    CppCond();
    explicit CppCond(int32_t var_idx);

    CppCond operator|(const CppCond& o) const;
    CppCond operator&(const CppCond& o) const;
    CppCond& operator|=(const CppCond& o);
    CppCond& operator&=(const CppCond& o);

    std::string ToString() const;
    bool DoesImply(const CppCond& o) const;

private:
    // stored in the format {n_1, i_1, i_2, ..., i_{n_1}, n_2, j_1, j_2, ..., j_{n_2}, ...}
    // = (i_1 & i_2 & ... & i_{n_1}) | (j_1 & j_2 & ... & j_{n_2}) | ...
    // boolean negation is represented by negative indices
    std::vector<int32_t> formula_;
};

}
