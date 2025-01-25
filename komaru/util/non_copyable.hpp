#pragma once

namespace komaru::util {

struct NonCopyable {
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}
