#pragma once
#include <utility>

namespace komaru::util {

template<typename T>
class Exchangeable {
public:
    T* operator->() {
        return &value;
    }

    T& operator*() {
        return value;
    }

    template<typename... Args>
    T&& Exchange(Args... args) {
        new (&value) T(std::forward<Args>(args)...);
    }

private:
    T value;
};

}
