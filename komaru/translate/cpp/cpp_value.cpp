#include "cpp_value.hpp"

#include <util/std_extensions.hpp>

namespace komaru::translate::cpp {

static std::string ToCppValue(const komaru::lang::AtomValue& value) {
    return value.Visit(util::Overloaded{[](int32_t x) {
                                            return std::to_string(x);
                                        },
                                        [](char c) {
                                            return std::string(1, c);
                                        },
                                        [](bool b) -> std::string {
                                            return b ? "true" : "false";
                                        }});
}

static std::string ToCppValue(const komaru::lang::TupleValue& value) {
    std::string res = "std::make_tuple(";

    for (const auto& [i, subvalue] : util::Enumerate(value.GetValues())) {
        res += ToCppValue(subvalue);

        if (i + 1 != value.GetValues().size()) {
            res += ", ";
        }
    }

    res += ")";

    return res;
}

std::string ToCppValue(const lang::Value& value) {
    return value.Visit([](const auto& value) {
        return ToCppValue(value);
    });
}

}  // namespace komaru::translate::cpp
