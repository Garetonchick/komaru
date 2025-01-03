#pragma once
#include <utility>

template<typename F>
class Defer {
public:
    Defer(F func) : func_(std::move(func)) {}

    ~Defer() {
        func_();
    }
private:
    F func_;
};
