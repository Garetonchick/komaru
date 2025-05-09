#pragma once
#include <cstdint>
#include <vector>

namespace komaru::translate::common {

// Boolean formula stored as DNF
class Cond {
public:
    Cond();
    explicit Cond(int32_t var_idx);

    Cond operator|(const Cond& o) const;
    Cond operator&(const Cond& o) const;
    Cond& operator|=(const Cond& o);
    Cond& operator&=(const Cond& o);

    std::string ToString() const;
    bool DoesImply(const Cond& o) const;

private:
    // stored in the format {n_1, i_1, i_2, ..., i_{n_1}, n_2, j_1, j_2, ..., j_{n_2}, ...}
    // = (i_1 & i_2 & ... & i_{n_1}) | (j_1 & j_2 & ... & j_{n_2}) | ...
    // boolean negation is represented by negative indices
    std::vector<int32_t> formula_;
};

}  // namespace komaru::translate::common
