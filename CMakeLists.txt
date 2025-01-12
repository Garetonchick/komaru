cmake_minimum_required(VERSION 3.20)

project(Komaru)

# clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common compile options for C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -g -stdlib=libc++ -ftime-trace")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
endif()

include_directories(vendor)
include_directories(vendor/glfw/include)
include_directories(vendor/glad/include)
add_subdirectory(vendor)

find_package(OpenGL REQUIRED)

file(GLOB EDITOR_SOURCES
    komaru/editor/*.cpp komaru/editor/*.hpp
    komaru/editor/kimgui/*.cpp komaru/editor/kimgui/*.hpp
)
file(GLOB UTIL_SOURCES komaru/util/*.cpp komaru/util/*.hpp)

include_directories(komaru)
add_library(util STATIC ${UTIL_SOURCES})
add_executable(editor ${EDITOR_SOURCES})
add_executable(test-node-graph-editor test/imgui_node_graph_test.cpp)

target_link_libraries(editor PRIVATE util imgui glfw OpenGL::GL glad)
target_link_libraries(test-node-graph-editor PRIVATE imgui glfw OpenGL::GL glad)

