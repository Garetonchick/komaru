#pragma once
#include <utility>

template<typename F>
class Defer {
public:
    Defer(auto&& func) : func_(std::forward<decltype(func)>(func)) {}

    ~Defer() {
        func_();
    }
private:
    F func_;
};
