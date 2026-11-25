CXX ?= clang++
CXXFLAGS ?= -g -O3 -Wall -Wextra -std=c++17

TARGET = toy
SRC = toy.cpp src/lexer.cpp
HEADERS = src/lexer.h
TEST = build/lexer-test

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

$(TEST): tests/lexer_test.cpp src/lexer.cpp $(HEADERS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/lexer_test.cpp src/lexer.cpp -o $(TEST)

test: $(TEST)
	./$(TEST)

# Retained for subsequent chapters that require LLVM libraries.
llvm-ch3: $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(shell llvm-config --cxxflags) $(SRC) $(shell llvm-config --ldflags --system-libs --libs core) -o $(TARGET)

llvm-ch4: $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(shell llvm-config --cxxflags) $(SRC) $(shell llvm-config --ldflags --system-libs --libs core orcjit native) -o $(TARGET)

format:
	clang-format -i $(SRC) $(HEADERS) tests/lexer_test.cpp

clean:
	rm -f $(TARGET) $(TEST) *.o *.ll *.s *.bc

.PHONY: all test llvm-ch3 llvm-ch4 format clean
