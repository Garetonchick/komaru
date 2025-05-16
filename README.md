# Komaru

Komaru 🐱 is a novel graphical programming language inspired by
category theory, a branch of mathematics that studies diagrams.
It is high-level, pure and statically typed graphical programming language.

Currently Komaru supports transcompilation into C++ or Haskell. The later implements
3rd party package loading.

Program examples in the Komaru language can be found in the `examples` folder.

![](/preview.png?raw=true "Komaru Editor")

## Build & Run

Building requires CMake and currently only the Clang compiler is officially supported, although other
compilers should work too with minor `CMakeLists.txt` adjustments. You will need Qt6 and [QTermWidget](https://github.com/lxqt/qtermwidget) to be installed in your environment.

### Linux

    cmake --preset=release
    cmake --build build/release
    ./build/release/editor

