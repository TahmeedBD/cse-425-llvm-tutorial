#ifndef TOY_LEXER_H
#define TOY_LEXER_H

#include <cstddef>
#include <string>

namespace toy {

enum class TokenKind { Eof, Function, Return, Identifier, Number, Symbol, Error };

struct SourceLocation {
  std::size_t offset = 0;
  std::size_t line = 1;
  std::size_t column = 1;
};

struct Token {
  TokenKind kind;
  std::string lexeme;
  SourceLocation location;
  double number = 0.0; // Meaningful only for Number.
  std::string diagnostic; // Meaningful only for Error.
};

const char *tokenName(TokenKind kind);

class Lexer {
public:
  explicit Lexer(std::string source);
  Token next();

private:
  bool atEnd() const;
  char peek(std::size_t lookahead = 0) const;
  char advance();
  Token make(TokenKind kind, SourceLocation start) const;
  Token error(SourceLocation start, const char *message) const;
  Token number();

  std::string source_;
  SourceLocation cursor_;
};

} // namespace toy

#endif
