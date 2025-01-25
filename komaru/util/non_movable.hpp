#pragma once

namespace komaru::util {

struct NonMovable {
    NonMovable() = default;
    ~NonMovable() = default;

    NonMovable(NonMovable&) = default;
    NonMovable& operator=(const NonMovable&) = default;

    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};

}
