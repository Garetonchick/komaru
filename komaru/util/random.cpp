#include "random.hpp"

#include <util/string.hpp>

#include <algorithm>

namespace komaru::util {

std::string RandomAlphaNumString(size_t sz) {
    THREAD_SAFE_RANDOM_FUNC

    static constexpr const char kTable[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static constexpr size_t kTlen = CStrlen(kTable);
    static_assert(kTlen == 62);  // 26 * 2 + 10 = 62

    std::uniform_int_distribution<int> dist(0, kTlen - 1);

    std::string res(sz, '\0');

    std::ranges::generate(res, [&]() {
        return kTable[dist(gen)];
    });

    return res;
}

}  // namespace komaru::util
