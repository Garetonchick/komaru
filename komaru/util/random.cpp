#include "random.hpp"

#include <util/string.hpp>

#include <algorithm>

namespace komaru::util {


std::string RandomAlphaNumString(size_t sz) {
    THREAD_SAFE_RANDOM_FUNC

    static constexpr const char table[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static constexpr size_t tlen = CStrlen(table);
    static_assert(tlen == 62); // 26 * 2 + 10 = 62

    std::uniform_int_distribution<int> dist(0, tlen - 1);

    std::string res(sz, '\0');

    std::ranges::generate(res, [&](){
        return table[dist(gen)];
    });

    return res;
}

}
