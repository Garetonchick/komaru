#pragma once
#include <filesystem>
#include <expected>
#include <system_error>

namespace komaru::util {

// Thread-safe
std::filesystem::path GenTmpFilepath();
std::expected<std::string, std::error_code> ReadFile(const std::filesystem::path& path);
std::error_code WriteFile(const std::filesystem::path& path, const std::string& content);

}
