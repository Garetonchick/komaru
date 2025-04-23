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

// NOLINTBEGIN(readability-identifier-naming)

inline constexpr auto Pure = [] <typename T> (T val) -> IO<T> {
    return IO<T>([val=std::move(val)]() mutable -> T {
        return val;
    });
};

inline constexpr auto Fmap = [] <typename F, typename A>
(F func, IO<A> a) -> IO<typename std::invoke_result_t<F, A>> {
    using B = typename std::invoke_result_t<F, A>;

    return IO<B>([func=std::move(func), a=std::move(a)]() mutable -> B {
        return func(a.Run());
    });
};

inline constexpr auto Bind = [] <typename F, typename A>
(IO<A> a, F func) -> IO<typename std::invoke_result_t<F, A>::ValueType> {
    using B = typename std::invoke_result_t<F, A>::ValueType;

    return IO<B>([func=std::move(func), a=std::move(a)]() mutable -> B {
        auto b = func(a.Run());
        return b.Run();
    });
};

inline constexpr auto Chain = [] <typename A, typename B> (IO<A> a, IO<B> b) -> IO<B> {
    return IO<B>([a=std::move(a), b=std::move(b)]() mutable -> B{
        a.Run();
        return b.Run();
    });
};

inline constexpr auto LiftM2 = [] <typename F, typename A, typename B>
(F func, IO<A> a, IO<B> b) -> IO<std::invoke_result_t<F, A, B>> {
    using C = std::invoke_result_t<F, A, B>;

    return IO<C>([func = std::move(func), a = std::move(a), b = std::move(b)]() mutable -> C {
        return func(a.Run(), b.Run());
    });
};

inline constexpr auto PutStr = [](std::string s) -> IO<std::monostate> {
    return IO<std::monostate>([s=std::move(s)]() mutable -> std::monostate {
        std::cout << s;
        return std::monostate{};
    });
};

inline constexpr auto PutStrLn = [](std::string s) -> IO<std::monostate> {
    return IO<std::monostate>([s=std::move(s)]() mutable -> std::monostate {
        std::cout << s << "\n";
        return std::monostate{};
    });
};

inline constexpr auto Read = [] <typename T> (std::monostate = {}, DeductionTag<IO<T>> = {}) -> IO<T> {
    return IO<T>([]() mutable -> T {
        return *std::istream_iterator<T>(std::cin);
    });
};

inline constexpr auto Print = [] (auto val) -> IO<std::monostate> {
    return IO<std::monostate>([val=std::move(val)] mutable -> std::monostate {
        std::cout << val;
        return std::monostate{};
    });
};

inline constexpr auto Plus = [] <typename T> (T x, T y) -> T {
    return x + y;
};

inline constexpr auto Minus = [] <typename T> (T x, T y) -> T {
    return x - y;
};

inline constexpr auto Multiply = [] <typename T> (T x, T y) -> T {
    return x * y;
};

inline constexpr auto Less = [] <typename T> (T x, T y) -> bool {
    return x < y;
};

inline constexpr auto LessEq = [] <typename T> (T x, T y) -> bool {
    return x <= y;
};

inline constexpr auto GreaterEq = [] <typename T> (T x, T y) -> bool {
    return x >= y;
};

inline constexpr auto Id = [] <typename T> (T x) -> T {
    return x;
};

// NOLINTEND(readability-identifier-naming)
