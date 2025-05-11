cmake_minimum_required(VERSION 3.27)

project(Komaru)

include(cmake/Utils.cmake)
include(cmake/CXXSetup.cmake)
include(cmake/CXXFlags.cmake)

# TODO: cleanup cmake

include_directories(.)

file(GLOB EDITOR_SOURCES komaru/editor/*.cpp komaru/editor/*.hpp)
file(GLOB UTIL_SOURCES komaru/util/*.cpp komaru/util/*.hpp)
file(GLOB LANG_SOURCES komaru/lang/*.cpp komaru/lang/*.hpp)
file(GLOB PARSERS_SOURCES komaru/parsers/*.cpp komaru/parsers/*.hpp)
file(GLOB_RECURSE TRANSLATE_SOURCES komaru/translate/*.cpp komaru/translate/*.hpp)

add_library(komarulib ${UTIL_SOURCES} ${LANG_SOURCES} ${TRANSLATE_SOURCES} ${PARSERS_SOURCES})

# Setup Qt
list(APPEND CMAKE_PREFIX_PATH "/home/gareton/Qt/6.9.0/gcc_64/lib/cmake/") # TODO: more elegant solution

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(qtermwidget6 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(editor ${EDITOR_SOURCES})

target_link_libraries(editor PRIVATE qtermwidget6 Qt6::Core Qt6::Gui Qt6::Widgets komarulib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(ENV{ASAN_OPTIONS} "alloc_dealloc_mismatch=0")

file(GLOB_RECURSE TEST_SOURCES test/*.cpp test/*.hpp)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests GTest::gtest_main komarulib)

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
target_link_libraries(playlib PUBLIC komarulib)
target_link_libraries(playground PRIVATE playlib)
