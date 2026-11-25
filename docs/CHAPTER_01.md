# Chapter 1 — Lexer implementation

Reference: [LLVM Kaleidoscope, Chapter 1](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html).

## Scope

This milestone covers lexical analysis, not parsing, ASTs, static typing, indentation semantics, evaluation or LLVM code generation.

- Numeric tokens contain a double and retain their original spelling.
- function and return are reserved keywords. The tutorial's def spelling is replaced, and extern is not a reserved keyword.
- Identifiers use ASCII letters or underscores, followed by ASCII letters, digits or underscores.
- Printable ASCII punctuation is emitted as individual symbol tokens. The parser will later decide which symbols form legal syntax; multi-character operators are not yet recognized as single tokens.
- C-style line comments (//) and non-nested block comments (/* ... */) are skipped. Unterminated block comments are errors. Hash comments are not supported.
- Whitespace is currently skipped, with byte offsets, line and column retained on tokens. NEWLINE/INDENT/DEDENT handling and layout validation are deferred until the parser needs them. This lexer alone does not enforce indentation-sensitive grammar.

## Numeric literal grammar

```text
digit    = [0-9]
mantissa = digit+ ("." digit+)? | "." digit+
exponent = [eE] [+-]? digit+
number   = mantissa exponent?
```

The lexer scans the grammar before converting with std::from_chars and checks complete conversion and range errors. Malformed adjacent decimal fragments such as 1.23.45 and 1..2 are rejected. A standalone dot remains punctuation, and 1.foo tokenizes as 1, dot, foo to preserve future field-access syntax. A trailing dot is not part of a numeric literal. A leading minus is a separate operator, not part of a number.

## Design deviations and follow-up

- Lexer state belongs to an independent Lexer instance instead of global token metadata. Each token owns its lexeme and records its source location.
- The extern keyword is deliberately omitted for now. The tutorial uses it for external native functions and forward declarations. Later milestones must decide whether to support imports, an FFI declaration mechanism, and/or a signature collection pass before replacing tutorial examples that depend on extern. This is a known assignment-compatibility deviation, not a claim that external calls are unnecessary.
- // now means a line comment, so the previously proposed // integer division syntax is withdrawn; select a nonconflicting form when integer semantics are introduced.
- The initial number representation remains double. Integer, Boolean, string and unit semantics are not implemented in Chapter 1.

## Validation

Run make test or CTest. The regression suite covers independent lexer state, keywords, identifiers, symbols, EOF, numeric literals, malformed and out-of-range numbers, decimal/dot boundaries, comments and source positions.
