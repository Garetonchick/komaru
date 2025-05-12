#include "std_extensions.hpp"

namespace komaru::util {

std::optional<int64_t> ReadInteger(const std::string& s) {
    char* end = nullptr;
    errno = 0;
    int64_t num = strtoll(s.data(), &end, 10);
    if (errno != 0 || end != s.data() + s.size()) {
        return std::nullopt;
    }
    return num;
}

std::optional<double> ReadReal(const std::string& s) {
    char* end = nullptr;
    errno = 0;
    int64_t num = strtod(s.data(), &end);
    if (errno != 0 || end != s.data() + s.size()) {
        return std::nullopt;
    }
    return num;
}

}  // namespace komaru::util
