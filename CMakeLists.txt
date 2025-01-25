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

set(IMNODE_SOURCES
    vendor/imnodes/imnodes.cpp
)
file(GLOB EDITOR_SOURCES
    komaru/editor/*.cpp komaru/editor/*.hpp
    komaru/editor/kimgui/*.cpp komaru/editor/kimgui/*.hpp
    ${IMNODE_SOURCES}
)
file(GLOB UTIL_SOURCES komaru/util/*.cpp komaru/util/*.hpp)
file(GLOB LANG_SOURCES komaru/lang/*.cpp komaru/lang/*.hpp)

include_directories(komaru)
add_library(util STATIC ${UTIL_SOURCES})
add_library(lang STATIC ${LANG_SOURCES})
add_executable(editor ${EDITOR_SOURCES})
add_executable(test-node-graph-editor test/imgui_node_graph_test.cpp)
set(IMNODE_EXAMPLE_SOURCES
    vendor/imnodes/imnodes.cpp
    vendor/imnodes/example/main.cpp
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    vendor/imgui/backends/imgui_impl_sdl2.cpp
)
add_executable(imnodes-hello
    ${IMNODE_EXAMPLE_SOURCES}
    vendor/imnodes/example/hello.cpp
)
add_executable(imnodes-multi-editor
    ${IMNODE_EXAMPLE_SOURCES}
    vendor/imnodes/example/multi_editor.cpp
)
add_executable(imnodes-color-node-editor
    ${IMNODE_EXAMPLE_SOURCES}
    vendor/imnodes/example/color_node_editor.cpp
)
add_executable(imnodes-save-load
    ${IMNODE_EXAMPLE_SOURCES}
    vendor/imnodes/example/save_load.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(editor PRIVATE util lang imgui glfw OpenGL::GL glad)
target_link_libraries(test-node-graph-editor PRIVATE imgui glfw OpenGL::GL glad)

target_link_libraries(imnodes-hello PRIVATE OpenGL::GL SDL2::SDL2)
target_link_libraries(imnodes-multi-editor PRIVATE OpenGL::GL SDL2::SDL2)
target_link_libraries(imnodes-color-node-editor PRIVATE OpenGL::GL SDL2::SDL2)
target_link_libraries(imnodes-save-load PRIVATE OpenGL::GL SDL2::SDL2)

target_include_directories(imnodes-hello PUBLIC vendor/imgui vendor/imgui/backends vendor/imnodes)
target_include_directories(imnodes-multi-editor PUBLIC vendor/imgui vendor/imgui/backends vendor/imnodes)
target_include_directories(imnodes-color-node-editor PUBLIC vendor/imgui vendor/imgui/backends vendor/imnodes)
target_include_directories(imnodes-save-load PUBLIC vendor/imgui vendor/imgui/backends vendor/imnodes)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
  test/lang/test_type_registry.cpp
)

target_link_libraries(
  tests
  GTest::gtest_main
  util
  lang
)

include(GoogleTest)
gtest_discover_tests(tests)
