CXX ?= clang++
CXXFLAGS ?= -g -O3 -Wall -Wextra -std=c++17

# Target binary
TARGET = toy
SRC = toy.cpp

# Default target for Chapters 1 & 2 (pure C++ lexer and parser)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Chapter 3 target (linking LLVM Core for AST -> IR code generation)
llvm-ch3: $(SRC)
	$(CXX) $(CXXFLAGS) $(shell llvm-config --cxxflags) $(SRC) $(shell llvm-config --ldflags --system-libs --libs core) -o $(TARGET)

# Chapter 4+ target (linking LLVM Core, OrcJIT, and Native target code generation)
llvm-ch4: $(SRC)
	$(CXX) $(CXXFLAGS) $(shell llvm-config --cxxflags) $(SRC) $(shell llvm-config --ldflags --system-libs --libs core orcjit native) -o $(TARGET)

format:
	@find . -maxdepth 2 -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +
	@echo "Formatted all C++ source files."

clean:
	rm -f $(TARGET) *.o *.ll *.s *.bc

.PHONY: all llvm-ch3 llvm-ch4 format clean
