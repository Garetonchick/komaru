cmake_minimum_required(VERSION 3.27)

project(Komaru)

include(cmake/Utils.cmake)
include(cmake/CXXSetup.cmake)
include(cmake/CXXFlags.cmake)

include_directories(.)
include_directories(vendor/glfw/include)
include_directories(vendor/glad/include)
add_subdirectory(vendor)

find_package(OpenGL REQUIRED)

set(IMNODE_SOURCES
    vendor/imnodes/imnodes.cpp
)

# Disable linting for imnodes
set_source_files_properties(${IMNODE_SOURCES} PROPERTIES
    SKIP_LINTING ON
)

file(GLOB EDITOR_SOURCES
    komaru/editor/*.cpp komaru/editor/*.hpp
    komaru/editor/kimgui/*.cpp komaru/editor/kimgui/*.hpp
    ${IMNODE_SOURCES}
)
file(GLOB UTIL_SOURCES komaru/util/*.cpp komaru/util/*.hpp)
file(GLOB LANG_SOURCES komaru/lang/*.cpp komaru/lang/*.hpp)
file(GLOB_RECURSE TRANSLATE_SOURCES komaru/translate/*.cpp komaru/translate/*.hpp)

add_library(util STATIC ${UTIL_SOURCES})
add_library(lang STATIC ${LANG_SOURCES})
add_library(translate STATIC ${TRANSLATE_SOURCES})
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

# Disable linter for imnodes examples
set_target_properties(imnodes-hello PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(imnodes-multi-editor PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(imnodes-color-node-editor PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(imnodes-save-load PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(test-node-graph-editor PROPERTIES CXX_CLANG_TIDY "")

find_package(SDL2 REQUIRED)

target_link_libraries(editor PRIVATE util lang translate imgui glfw OpenGL::GL glad)
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

set(ENV{ASAN_OPTIONS} "alloc_dealloc_mismatch=0")

file(GLOB TEST_SOURCES
    test/lang/*.cpp
    test/lang/*.hpp
    test/translate/*.cpp
    test/translate/*.hpp
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(
  tests
  GTest::gtest_main
  translate
  lang
  util
)

include(GoogleTest)
gtest_discover_tests(tests)

add_executable(playground playground/playground.cpp)
add_library(test_programs
            test/translate/programs.hpp
            test/translate/programs.cpp
            test/translate/program_utils.cpp
            test/translate/program_utils.hpp
)

target_link_libraries(playground PRIVATE
    test_programs
    GTest::gtest
    translate
    lang
    util
)
