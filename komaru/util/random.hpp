#pragma once
#include <string>
#include <random>
#include <mutex>

#define THREAD_SAFE_RANDOM_FUNC \
    static std::mutex mu; \
    static std::mt19937 gen((std::random_device()())); \
    std::lock_guard guard(mu);

namespace komaru::util {

std::string RandomAlphaNumString(size_t sz);

// [l, r]
template<typename T>
T RandomInt(T l, T r) {
    THREAD_SAFE_RANDOM_FUNC

    return std::uniform_int_distribution<T>(l, r)(gen);
}

}
