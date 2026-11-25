#include "lexer.h"

#include <charconv>
#include <system_error>
#include <utility>

namespace toy {
namespace {

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isIdentifierStart(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdentifierPart(char c) { return isIdentifierStart(c) || isDigit(c); }

bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
         c == '\v';
}

} // namespace

const char *tokenName(TokenKind kind) {
  switch (kind) {
  case TokenKind::Eof:
    return "EOF";
  case TokenKind::Function:
    return "FUNCTION";
  case TokenKind::Return:
    return "RETURN";
  case TokenKind::Identifier:
    return "IDENTIFIER";
  case TokenKind::Number:
    return "NUMBER";
  case TokenKind::Symbol:
    return "SYMBOL";
  case TokenKind::Error:
    return "ERROR";
  }
  return "UNKNOWN";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

bool Lexer::atEnd() const { return cursor_.offset == source_.size(); }

char Lexer::peek(std::size_t lookahead) const {
  if (lookahead >= source_.size() - cursor_.offset)
    return '\0';
  return source_[cursor_.offset + lookahead];
}

char Lexer::advance() {
  const char c = source_[cursor_.offset++];
  if (c == '\r' || c == '\n') {
    if (c != '\n' || cursor_.offset < 2 || source_[cursor_.offset - 2] != '\r')
      ++cursor_.line;
    cursor_.column = 1;
  } else {
    ++cursor_.column;
  }
  return c;
}

Token Lexer::make(TokenKind kind, SourceLocation start) const {
  return {kind, source_.substr(start.offset, cursor_.offset - start.offset),
          start, 0.0, {}};
}

Token Lexer::error(SourceLocation start, const char *message) const {
  Token token = make(TokenKind::Error, start);
  token.diagnostic = message;
  return token;
}

Token Lexer::number() {
  const auto start = cursor_;
  while (isDigit(peek()))
    advance();
  if (peek() == '.' && isDigit(peek(1))) {
    advance();
    while (isDigit(peek()))
      advance();
  }
  if (peek() == 'e' || peek() == 'E') {
    advance();
    if (peek() == '+' || peek() == '-')
      advance();
    if (!isDigit(peek()))
      return error(start, "expected digits in numeric exponent");
    while (isDigit(peek()))
      advance();
  }
  if ((peek() == '.' && (isDigit(peek(1)) || peek(1) == '.')) ||
      isIdentifierStart(peek())) {
    do {
      advance();
    } while (isIdentifierPart(peek()) || peek() == '.');
    return error(start, "malformed numeric literal");
  }

  Token token = make(TokenKind::Number, start);
  const char *begin = token.lexeme.data();
  const char *end = begin + token.lexeme.size();
  const auto result = std::from_chars(begin, end, token.number);
  if (result.ec == std::errc::result_out_of_range)
    return error(start, "numeric literal out of range for double");
  if (result.ec != std::errc{} || result.ptr != end)
    return error(start, "malformed numeric literal");
  return token;
}

Token Lexer::next() {
  // Skip whitespace and comments iteratively, without recursive lexer calls.
  for (;;) {
    while (isSpace(peek()))
      advance();
    const auto start = cursor_;
    if (peek() == '/' && peek(1) == '/') {
      while (!atEnd() && peek() != '\n' && peek() != '\r')
        advance();
    } else if (peek() == '/' && peek(1) == '*') {
      advance();
      advance();
      while (!atEnd() && !(peek() == '*' && peek(1) == '/'))
        advance();
      if (atEnd())
        return error(start, "unterminated block comment");
      advance();
      advance();
    } else {
      break;
    }
  }

  const auto start = cursor_;
  if (atEnd())
    return make(TokenKind::Eof, start);
  if (isIdentifierStart(peek())) {
    do {
      advance();
    } while (isIdentifierPart(peek()));
    Token token = make(TokenKind::Identifier, start);
    if (token.lexeme == "function")
      token.kind = TokenKind::Function;
    else if (token.lexeme == "return")
      token.kind = TokenKind::Return;
    return token;
  }
  if (isDigit(peek()) || (peek() == '.' && isDigit(peek(1))))
    return number();

  const char c = advance();
  if (c < 33 || c > 126)
    return error(start, "unsupported source character");
  return make(TokenKind::Symbol, start);
}

} // namespace toy
