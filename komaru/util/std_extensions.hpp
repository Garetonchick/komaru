#pragma once
#include <vector>
#include <ranges>
#include <algorithm>
#include <map>
#include <unordered_map>

namespace komaru::util {

// https://en.cppreference.com/w/cpp/utility/variant/visit2
template <class... Types>
struct Overloaded : Types... {
    using Types::operator()...;
};

template <typename T>
bool VecEq(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

auto Enumerate(std::ranges::input_range auto&& r) {
    return std::views::zip(std::views::iota(static_cast<size_t>(0)), std::forward<decltype(r)>(r));
};

template <typename T>
void UnstableErase(std::vector<T>& v, const T& val) {
    v.resize(std::remove(v.begin(), v.end(), val) - v.begin());
}

template <typename K, typename V>
V GetOr(const std::unordered_map<K, V>& map, const K& key, const V& val = {}) {
    auto it = map.find(key);
    if (it == map.end()) {
        return val;
    }
    return it->second;
}

template <typename K, typename V>
V GetOr(const std::map<K, V>& map, const K& key, const V& val = {}) {
    auto it = map.find(key);
    if (it == map.end()) {
        return val;
    }
    return it->second;
}

std::optional<int64_t> ReadInteger(const std::string& s);
std::optional<double> ReadReal(const std::string& s);

}  // namespace komaru::util
