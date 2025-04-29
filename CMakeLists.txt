cmake_minimum_required(VERSION 3.27)

project(Komaru)

include(cmake/Utils.cmake)
include(cmake/CXXSetup.cmake)
include(cmake/CXXFlags.cmake)

include_directories(.)

# file(GLOB EDITOR_SOURCES
#     komaru/editor/*.cpp komaru/editor/*.hpp
#     komaru/editor/kimgui/*.cpp komaru/editor/kimgui/*.hpp
#     ${IMNODE_SOURCES}
#
file(GLOB UTIL_SOURCES komaru/util/*.cpp komaru/util/*.hpp)
file(GLOB LANG_SOURCES komaru/lang/*.cpp komaru/lang/*.hpp)
file(GLOB_RECURSE TRANSLATE_SOURCES komaru/translate/*.cpp komaru/translate/*.hpp)

add_library(util STATIC ${UTIL_SOURCES})
add_library(lang STATIC ${LANG_SOURCES})
add_library(translate STATIC ${TRANSLATE_SOURCES})
# add_executable(editor ${EDITOR_SOURCES})

# target_link_libraries(editor PRIVATE util lang translate)

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
add_library(playlib
            catlib/cpp/catlib.hpp
            test/translate/programs.hpp
            test/translate/programs.cpp
            test/translate/program_utils.cpp
            test/translate/program_utils.hpp
)

target_link_libraries(playground PRIVATE
    playlib
    translate
    lang
    util
)
