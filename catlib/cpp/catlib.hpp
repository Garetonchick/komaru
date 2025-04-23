#pragma once
#include <functional>
#include <iostream>
#include <iterator>
#include <variant>

template<typename T>
struct DeductionTag {};

template<typename T> requires (!std::is_void<T>())
class [[nodiscard]] IO {
public:
    using ValueType = T;
public:
    template<typename F>
    explicit IO(F producer)
        : producer_(std::move(producer)) {}

    T Run() {
        return producer_();
    }

private:
    std::function<T()> producer_;
};


template<typename T>
IO<T> Pure(T val) {
    return IO<T>([val=std::move(val)]() mutable -> T {
        return val;
    });
}

template<typename F, typename A>
IO<typename std::invoke_result_t<F, A>> Fmap(F func, IO<A> a) {
    using B = typename std::invoke_result_t<F, A>;

    return IO<B>([func=std::move(func), a=std::move(a)]() mutable -> B {
        return func(a.Run());
    });
}

template<typename F, typename A>
IO<typename std::invoke_result_t<F, A>::ValueType> Bind(IO<A> a, F func) {
    using B = typename std::invoke_result_t<F, A>::ValueType;

    return IO<B>([func=std::move(func), a=std::move(a)]() mutable -> B {
        auto b = func(a.Run());
        return b.Run();
    });
}

template<typename A, typename B>
IO<B> Chain(IO<A> a, IO<B> b) {
    return IO<B>([a=std::move(a), b=std::move(b)]() mutable -> B{
        a.Run();
        return b.Run();
    });
}

template<typename F, typename A, typename B>
IO<std::invoke_result_t<F, A, B>> LiftM2(F func, IO<A> a, IO<B> b) {
    using C = std::invoke_result_t<F, A, B>;

    return IO<C>([func = std::move(func), a = std::move(a), b = std::move(b)]() mutable -> C {
        return func(a.Run(), b.Run());
    });
}

inline IO<std::monostate> PutStr(std::string s) {
    return IO<std::monostate>([s=std::move(s)]() mutable -> std::monostate {
        std::cout << s;
        return std::monostate{};
    });
}

inline IO<std::monostate> PutStrLn(std::string s) {
    return IO<std::monostate>([s=std::move(s)]() mutable -> std::monostate {
        std::cout << s << "\n";
        return std::monostate{};
    });
}

template<typename T>
IO<T> Read(std::monostate = {}, DeductionTag<IO<T>> = {}) {
    return IO<T>([]() mutable -> T {
        return *std::istream_iterator<T>(std::cin);
    });
}

// NOLINTBEGIN(readability-identifier-naming)

constexpr auto Print = [] (auto val) -> IO<std::monostate> {
    return IO<std::monostate>([val=std::move(val)] mutable -> std::monostate {
        std::cout << val;
        return std::monostate{};
    });
};

constexpr auto Plus = [] <typename T> (T x, T y) -> T {
    return x + y;
};

constexpr auto Minus = [] <typename T> (T x, T y) -> T {
    return x - y;
};

constexpr auto Multiply = [] <typename T> (T x, T y) -> T {
    return x * y;
};

constexpr auto Less = [] <typename T> (T x, T y) -> bool {
    return x < y;
};

constexpr auto LessEq = [] <typename T> (T x, T y) -> bool {
    return x <= y;
};

constexpr auto GreaterEq = [] <typename T> (T x, T y) -> bool {
    return x >= y;
};

constexpr auto Id = [] <typename T> (T x) -> T {
    return x;
};

// NOLINTEND(readability-identifier-naming)
