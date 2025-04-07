#include "filesystem.hpp"

#include <util/random.hpp>

#include <fstream>
#include <sstream>

namespace komaru::util {

template <typename T>
static std::expected<T, std::error_code> Error() {
    return std::unexpected(std::make_error_code(std::errc(errno)));
}

std::filesystem::path GenTmpFilepath() {
    return std::filesystem::temp_directory_path() / RandomAlphaNumString(16);
}

std::expected<std::string, std::error_code> ReadFile(const std::filesystem::path& path) {
    std::ifstream t(path);
    std::stringstream buf;
    errno = 0;
    buf << t.rdbuf();
    if(!t) {
        return Error<std::string>();
    }
    return buf.str();
}

std::error_code WriteFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream out(path);
    errno = 0;
    out << content;
    out.flush();

    if(!out) {
        return std::make_error_code(std::errc(errno));
    }

    return std::error_code{};
}

}
