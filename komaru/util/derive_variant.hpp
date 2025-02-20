#pragma once
#include <stdexcept>
#include <variant>
#include <concepts>

namespace komaru::util {

// User must define GetVariantPointer
template<typename T>
class DeriveVariant {
public:
    DeriveVariant() = default;

    template<typename F>
    auto Visit(F&& func) const {
        return std::visit(std::forward<F>(func), *static_cast<const T*>(this)->GetVariantPointer());
    }

    template<typename U>
    const U& GetVariant() const {
        const U* subvalue = std::get_if<U>(static_cast<const T*>(this)->GetVariantPointer());
        if(subvalue == nullptr) {
            throw std::runtime_error("Bad GetVariant");
        }

        return *subvalue;
    }

    template<typename U>
    bool Holds() const {
        return std::holds_alternative<U>(*static_cast<const T*>(this)->GetVariantPointer());
    }
};

}
