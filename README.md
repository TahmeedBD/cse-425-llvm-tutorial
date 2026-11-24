# LLVM Kaleidoscope Tutorial Setup

This repository is configured for following the official [LLVM Tutorial: Implementing a Language with LLVM](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html).

---

## Environment & Installed Tools

- **Compiler**: Clang / Clang++ 21 (`clang++`)
- **LLVM Libraries & Config**: LLVM 21 (`llvm-config`)
- **Build Systems**: `make`, `cmake`, `ninja`
- **Debuggers**: `lldb`, `gdb`
- **Formatter**: `clang-format` (LLVM style via `.clang-format`)

---

## Code Formatting

You can format your C++ code at any time using:

```bash
# Format all source files in the project
make format

# Or directly with clang-format
clang-format -i toy.cpp
```

---

## How to Build & Run

### Chapter 1 & 2: Lexer and Parser (Pure C++)
In Chapters 1 and 2, LLVM libraries are not required. You can compile your `toy.cpp` using:

```bash
# Direct clang++ compilation
clang++ -g -O3 toy.cpp -o toy

# Or using Makefile
make

# Run
./toy
```

### Chapter 3: Code Generation to LLVM IR
Starting in Chapter 3, `toy.cpp` includes LLVM headers and generates LLVM IR:

```bash
# Direct command from the LLVM tutorial
clang++ -g -O3 toy.cpp $(llvm-config --cxxflags --ldflags --system-libs --libs core) -o toy

# Or using Makefile
make llvm-ch3

# Run
./toy
```

### Chapter 4+: JIT & Optimizer
In Chapter 4 and beyond, OrcJIT and native machine targets are added:

```bash
# Direct command
clang++ -g -O3 toy.cpp $(llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native) -o toy

# Or using Makefile
make llvm-ch4

# Run
./toy
```

### Using CMake (Optional)
If you prefer CMake:

```bash
cmake -B build -G Ninja
cmake --build build
./build/toy
```
