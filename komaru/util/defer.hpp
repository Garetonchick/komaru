#pragma once
#include <utility>

template <typename F>
class Defer {
public:
    template <typename U>
    explicit Defer(U&& func)
        : func_(std::forward<U>(func)) {
    }

    ~Defer() {
        func_();
    }

private:
    F func_;
};

template <typename U>
Defer(U&& func) -> Defer<U>;
