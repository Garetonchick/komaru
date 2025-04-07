#include "string.hpp"

#include <cctype>

namespace komaru::util {

// constexpr std::string Strip(const std::string& s) {
//     int64_t len = static_cast<int64_t>(s.size());
//     int64_t start = 0;
//     int64_t finish = len - 1;

//     while(start < len && std::isspace(s[start])) {
//         ++start;
//     }

//     while(finish >= 0 && std::isspace(s[finish])) {
//         --finish;
//     }

//     if(start > finish) {
//         return "";
//     }

//     return std::string(s.begin() + start, s.begin() + finish + 1);
// }

}
