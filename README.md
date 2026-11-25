# LLVM Kaleidoscope Tutorial

This project follows the [LLVM Kaleidoscope tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html) while developing a reusable language implementation. See the [language design specification](docs/LANGUAGE_DESIGN.md) and [Chapter 1 implementation notes](docs/CHAPTER_01.md).

## Current milestone: Chapter 1 — Lexer

The executable currently tokenizes input and reports lexical diagnostics. It does not parse, evaluate, or generate LLVM IR yet.

### Build and test

The intended WSL toolchain uses Clang/LLVM 21, CMake, Ninja and clang-format. Chapter 1 requires only C++17 and a standard library supporting floating-point std::from_chars.

```sh
make CXX=clang++
make test CXX=clang++
```

Or use CMake:

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

### Run

```sh
printf 'function square(x)\n    return x * 2.5e-3 // comment\n' | ./toy
./toy path/to/source
```

Without a file argument, the driver reads standard input until EOF. It prints token kinds, lexemes and source positions. Lexical errors produce diagnostics and a nonzero exit status.

### Formatting

```sh
make format
```

The existing .clang-format defines LLVM style, two-space indentation and an 80-column limit.

### Subsequent chapters

The Makefile retains llvm-ch3 and llvm-ch4 targets for later LLVM linking. CMake can enable the LLVM libraries with -DTOY_WITH_LLVM=ON when needed. Those targets are not needed for the current chapter.

## Structure

- toy.cpp — command-line driver
- src/lexer.h and src/lexer.cpp — token data and lexical analysis
- tests/lexer_test.cpp — regression tests
- docs/ — project design and milestone notes
