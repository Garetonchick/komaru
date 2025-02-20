#pragma once
#include <vector>

namespace komaru::util {

// https://en.cppreference.com/w/cpp/utility/variant/visit2
template<class... Types>
struct Overloaded : Types... { using Types::operator()...; };

template<typename T>
bool VecEq(const std::vector<T>& a, const std::vector<T>& b) {
    if(a.size() != b.size()) {
        return false;
    }

    for(size_t i = 0; i < a.size(); ++i) {
        if(a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

}
