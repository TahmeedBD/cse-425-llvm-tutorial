#include "../src/lexer.h"

#include <cstdlib>
#include <iostream>
#include <string>

using toy::Lexer;
using toy::TokenKind;

namespace {

void require(bool condition, const char *message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

void expect(Lexer &lexer, TokenKind kind, const std::string &lexeme) {
  const auto token = lexer.next();
  require(token.kind == kind && token.lexeme == lexeme, "token mismatch");
}

void expectNumber(const char *source, double value) {
  Lexer lexer(source);
  const auto token = lexer.next();
  require(token.kind == TokenKind::Number && token.number == value &&
              token.lexeme == source,
          "valid numeric literal");
  expect(lexer, TokenKind::Eof, "");
}

void expectError(const char *source) {
  Lexer lexer(source);
  const auto token = lexer.next();
  require(token.kind == TokenKind::Error && !token.diagnostic.empty(),
          "expected lexical diagnostic");
  expect(lexer, TokenKind::Eof, "");
}

} // namespace

int main() {
  Lexer a("function square(x) return x*x;"), b("extern _id2 fn func");
  expect(a, TokenKind::Function, "function");
  expect(b, TokenKind::Identifier, "extern");
  expect(a, TokenKind::Identifier, "square");
  expect(b, TokenKind::Identifier, "_id2");
  expect(b, TokenKind::Identifier, "fn");
  expect(b, TokenKind::Identifier, "func");
  expect(b, TokenKind::Eof, "");
  expect(a, TokenKind::Symbol, "(");
  expect(a, TokenKind::Identifier, "x");
  expect(a, TokenKind::Symbol, ")");
  expect(a, TokenKind::Return, "return");
  expect(a, TokenKind::Identifier, "x");
  expect(a, TokenKind::Symbol, "*");
  expect(a, TokenKind::Identifier, "x");
  expect(a, TokenKind::Symbol, ";");
  expect(a, TokenKind::Eof, "");
  expect(a, TokenKind::Eof, "");

  expectNumber("0", 0.0);
  expectNumber("42", 42.0);
  expectNumber(".5", 0.5);
  expectNumber("3.25", 3.25);
  expectNumber("1e3", 1000.0);
  expectNumber("2.5E-3", 0.0025);
  expectError("1.23.45.67");
  expectError("1..2");
  expectError("1e+");
  expectError("1e");
  expectError("1e999");
  expectError("1e-999");
  expectError("1abc");
  expectError("/* unfinished");
  expectError("\x01");

  Lexer dot("point.x 1.foo . -2");
  expect(dot, TokenKind::Identifier, "point");
  expect(dot, TokenKind::Symbol, ".");
  expect(dot, TokenKind::Identifier, "x");
  expect(dot, TokenKind::Number, "1");
  expect(dot, TokenKind::Symbol, ".");
  expect(dot, TokenKind::Identifier, "foo");
  expect(dot, TokenKind::Symbol, ".");
  expect(dot, TokenKind::Symbol, "-");
  expect(dot, TokenKind::Number, "2");
  expect(dot, TokenKind::Eof, "");

  Lexer comments("// first\r\n/* x\n y */  function / // last");
  const auto keyword = comments.next();
  require(keyword.kind == TokenKind::Function && keyword.location.line == 3 &&
              keyword.location.column == 8,
          "comment skipping and source positions");
  expect(comments, TokenKind::Symbol, "/");
  expect(comments, TokenKind::Eof, "");

  std::cout << "All lexer tests passed.\n";
}
