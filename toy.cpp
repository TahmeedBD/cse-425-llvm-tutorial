#include "src/lexer.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cerr << "usage: toy [source-file]\n";
    return 2;
  }

  std::ifstream file;
  std::istream *input = &std::cin;
  const char *name = "<stdin>";
  if (argc == 2) {
    name = argv[1];
    file.open(name, std::ios::binary);
    if (!file) {
      std::cerr << "cannot open " << name << '\n';
      return 2;
    }
    input = &file;
  }

  toy::Lexer lexer(std::string(std::istreambuf_iterator<char>(*input), {}));
  bool failed = false;
  for (;;) {
    const auto token = lexer.next();
    const auto &loc = token.location;
    if (token.kind == toy::TokenKind::Error) {
      std::cerr << name << ':' << loc.line << ':' << loc.column
                << ": error: " << token.diagnostic << '\n';
      failed = true;
      continue;
    }
    std::cout << loc.line << ':' << loc.column << ' '
              << toy::tokenName(token.kind);
    if (token.kind != toy::TokenKind::Eof)
      std::cout << ' ' << std::quoted(token.lexeme);
    if (token.kind == toy::TokenKind::Number)
      std::cout << " = "
                << std::setprecision(std::numeric_limits<double>::max_digits10)
                << token.number;
    std::cout << '\n';
    if (token.kind == toy::TokenKind::Eof)
      break;
  }
  return failed ? 1 : 0;
}
