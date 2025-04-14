# Setup compilation database for clangd and clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup clang-tidy
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

# Common options for C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
