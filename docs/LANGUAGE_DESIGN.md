# Language Design Specification

**Version:** 0.1-draft  
**Date:** 2026-11-24  
**Status:** Initial design requirements; not a claim of implemented functionality  
**Working name:** Undecided

## 1. Purpose and scope

Develop a general-purpose programming language with predictable semantics, strong type safety, readable expression-oriented syntax and reasonably high native performance. The initial compiler evolves from the LLVM Kaleidoscope tutorial; the long-term objective is a standalone, publicly distributable language and toolchain rather than a disposable demonstration.

This document describes the intended language, not the current implementation, and is separate from any academic technical report.

The keywords MUST, SHOULD and MAY distinguish committed requirements, preferred defaults and permitted future choices. Examples of the proposed syntax are illustrative until a grammar and implementation are finalized.

## 2. Design philosophy

- **Correctness and clarity:** types, control flow and conversions should have explicit, comprehensible meanings. Reject incompatible operations rather than guessing the programmer's intent.
- **Performance without mandatory hardware control:** favor efficient native representations and optimizable code. Low-level facilities are optional future features, not a prerequisite for ordinary programming.
- **Readability over effortless authoring:** code should be intuitive to read even if writing it requires discipline and technical knowledge. Brevity is desirable when it preserves meaning.
- **Expression-oriented computation:** compose smaller value-producing expressions into larger ones. Prefer meaningful transformations over verbose boilerplate.
- **Functional-first, not purely functional:** support functions and composition without requiring recursion or banning mutation and side effects. Iteration must remain practical.
- **Opinionated design:** choose one canonical syntax or idiom for each language facility where practical; avoid synonyms, redundant constructs, implicit conversions and arbitrary user-defined operators. Equivalent algorithms cannot and need not be prohibited.
- **No conventional OOP initially:** no classes, inheritance hierarchies, virtual method dispatch or implicit object receivers. Structs, namespaces and dot notation do not imply OOP. A later, deliberately designed abstraction model remains possible.
- **Pragmatic safety:** enforce the declared type rules statically. Do not attempt to prove all program behavior at compile time. Ordinary runtime errors are permitted and should have defined, diagnosable behavior rather than becoming silently incorrect execution.

## 3. Surface syntax

### 3.1 Blocks and layout

- Blocks MUST be indentation-sensitive. Curly braces are not block delimiters.
- The preferred initial layout is four spaces per indentation level; tabs in indentation are rejected.
- A colon introduces an indented block. A logical newline ends a statement unless the grammar permits continuation (e.g. within parentheses or brackets). No mandatory semicolons in the target language.
- The lexer should emit NEWLINE, INDENT and DEDENT tokens for significant layout. Blank and comment-only lines do not change indentation. Dedents must match an established indentation level.
- Identifier, comment, Unicode and multiline continuation policies beyond these initial rules remain to be specified.

### 3.2 Declarations and functions

The target language uses one function declaration form, with explicit types and an explicit return arrow:

```text
fn square(x: int) -> int:
    return x * x

fn average(a: float, b: float) -> float:
    return (a + b) / 2.0

let threshold: int = 10
```

- All named function parameters, return types and ordinary variable declarations MUST have explicit types. No implicit declaration-level type inference.
- The compiler MUST still determine and check the types of subexpressions from the explicit declarations, literals and operator rules. This is normal type checking, not optional declaration annotations.
- `let` is the proposed canonical declaration keyword. Binding mutability and reassignment syntax are not finalized; immutable-by-default is preferred.
- One canonical function call syntax for ordinary free functions; no implicit receiver or hidden dispatch.

### 3.3 Explicit returns and unit

- A function MUST declare its return type, including when it produces no meaningful data.
- Every normally completing path MUST execute an explicit `return expression` with a value compatible with the declared return type.
- Implicit last-expression function returns, bare `return`, implicit fallthrough and a source-language `void` type are prohibited.
- `unit` is a real type containing exactly one value, written `()`. It represents normal completion without additional information.
- A unit-returning function may have side effects. It must write `return ()` explicitly.
- Trapping, nontermination and future explicitly non-returning operations are not normal returns. Their exact typing and control-flow rules are deferred.

```text
fn greet(name: string) -> unit:
    print("Hello, " + name)
    return ()

fn eligible(age: int, score: float) -> bool:
    return age >= 18 and score >= 75.0
```

The compiler may lower unit-returning functions to LLVM void or another appropriate ABI representation. Source-language unit is nevertheless a value. Do not force callers or callees to invent dummy Boolean or integer results.

This combines the unit-type idea found in Rust and Kotlin with a stricter explicit-return requirement.

## 4. Type system

### 4.1 Core rules

The language MUST be statically and strongly typed, with no implicit conversion between unrelated types. Dynamic typing, gradual typing, truthiness and implicit string-to-number conversion are not initial features.

Proposed core types:

| Type | Meaning | Initial LLVM representation |
| --- | --- | --- |
| int | Signed 64-bit integer | i64 |
| float | IEEE 754 binary64 floating-point number | double |
| bool | Distinct Boolean | i1 |
| unit | Singleton completion value | Context-dependent; may be erased |
| string | Text value | Deferred until runtime and representation are designed |

Numeric values are a deliberate exception to strict type separation. Retain separate integer and floating-point representations rather than introducing a universal dynamically tagged number in the initial design.

### 4.2 Numeric semantics

- Integer and floating-point operands MUST interoperate for the supported arithmetic operations and numeric comparisons.
- For mixed arithmetic, promote the integer operand to float; do not silently narrow floats to integers.
- Integer-only addition, subtraction and multiplication produce int; floating-point and mixed arithmetic produce float.
- Proposed division rule: `/` produces float, including integer/integer division; `//` is explicit integer division (proposed truncation toward zero, with integer operands only).
- A numeric expression's result type follows from the operator and operands, not from a dynamically changing variable type.
- Boolean values are not integers. Conditions require bool.
- String concatenation with other strings is permitted; arithmetic between strings and numbers is rejected. Formatting and explicit conversions may be added later.
- Fixed-width overflow, division by zero, NaN, infinity, out-of-range conversions and exact mixed numeric comparison behavior need an explicit specification before claiming the numerical implementation is complete. Prefer diagnosed integer overflow to accidental wraparound by default. Binary64 cannot exactly represent all signed 64-bit integers; mixed arithmetic may round, and comparisons should avoid gratuitous precision loss.

| Expression | Proposed result |
| --- | --- |
| 5 + 3 | int: 8 |
| 5 + 3.5 | float: 8.5 |
| 5 / 2 | float: 2.5 |
| 5 // 2 | int: 2 |
| 5 > 3.5 | bool: true |
| true + 5 | Type error |
| "5" + 3 | Type error |

The initial Kaleidoscope-compatible milestone may retain its double-only semantics. This is an implementation limitation, not the final language's numeric specification.

## 5. Expressions, control flow and functions

- Arithmetic, comparisons, Boolean operations and function calls produce values.
- Boolean operators should be short-circuiting with a documented evaluation order. Prefer left-to-right evaluation of operands and call arguments where side effects are observable.
- A conditional can be an expression. When used as a value, all reachable result branches must supply compatible values and the condition must be Boolean. An else branch is required when no alternative result exists.
- Function return is distinct from a block's expression value: evaluating a branch or expression does not implicitly return from a function.
- Support conventional iterative computation. Higher-order functions and collection transformations are preferred future features; recursion is allowed but not required as an idiom. Avoid forcing recursion for ordinary repetition.
- Pure computation is encouraged, but side effects and local mutation are not categorically banned.

Illustrative conditional-expression syntax (subject to grammar work):

```text
fn absolute(x: int) -> int:
    let magnitude: int = if x >= 0:
        x
    else:
        -x
    return magnitude
```

The negation in this example is subject to the integer overflow policy (the minimum representable signed integer has no positive int equivalent).

## 6. Data and dot notation

### 6.1 Structs

Structs are planned as named, typed product values with fields, independently of any class system:

```text
struct Point:
    x: float
    y: float

fn squared_length(p: Point) -> float:
    return p.x * p.x + p.y * p.y
```

The constructor syntax, value/reference semantics, layout, visibility and mutability of structs are deferred. No inheritance or virtual dispatch is implied by structs.

### 6.2 Dot notation

Dot notation is a first-class syntax decision, not an OOP commitment.

- `point.x`: access a declared struct field.
- `math.sqrt(25.0)`: qualify a function by its module or namespace.
- `numbers.map(square).filter(is_positive).sum()`: planned statically resolved collection transformation syntax.

The last form may be implemented as statically resolved receiver-style functions, not virtual methods. Their receiver is an ordinary typed input at the semantic level. Their canonical declaration and invocation rules must be specified before implementation. Avoid providing both receiver-style and free-function spellings for the same operation merely as aliases. Prefer the dot-chain idiom over introducing a competing general pipeline operator in the initial language.

Lists, generics, function values, collection APIs and their runtime representations are planned, not implemented requirements for the first compiler milestone.

## 7. Deferred decisions

Do not commit to these until the relevant features are needed and their trade-offs are understood:

- Heap allocation and memory lifetime: garbage collection, reference counting, ownership or another model. Avoid prematurely choosing a general heap model while the compiler only supports scalar values.
- Primary deployment workflow: ahead-of-time native compilation, JIT execution or both. Retain the LLVM backend and follow the tutorial's applicable execution stages first.
- Strings and collections: allocation, encoding, layout, equality, indexing, mutation and lifetime.
- Precise module/import syntax, struct construction, generics, closures, error/result types, concurrency, interoperability and package management.
- Full numeric edge-case semantics, operator precedence table, complete grammar, formal type rules and portability targets.
- Whether to add a restricted, opinionated object abstraction model after the non-OOP core is established.
- Public name, release targets and distribution mechanism.

## 8. Compiler architecture and engineering

Use the repository's existing C++ / LLVM 21 toolchain. Preserve the existing build workflow; modularize incrementally rather than introducing a parallel compiler or changing build systems merely for appearance.

The intended pipeline is:

```text
Source -> Lexer -> Parser -> AST -> Semantic analysis -> LLVM IR
                                                        |
                                               LLVM backend
                                               /          \
                                             JIT        Native code
```

Requirements for a reusable foundation:

- Keep source input and lexer/parser state explicit and independently testable.
- Use structured tokens with kinds, original lexemes and source spans; preserve literal spelling until semantic interpretation. The source-language dot and decimal literal rules must be unambiguous.
- Keep AST nodes independent of LLVM types and instructions where practical. Separate semantic checking from lowering to LLVM IR.
- Represent source-language types independently of LLVM representations. Start small rather than building an unused abstraction hierarchy.
- Support source-located diagnostics and reject malformed input reliably.
- Test tokenization, indentation, parsing/precedence, type rules, generated IR and end-to-end execution as features become available.
- Keep commits incremental; pin the tested LLVM version and document build/run instructions.
- Preserve upstream licensing and attribution when incorporating tutorial code. Before public release, choose a project license and verify dependency obligations.

## 9. Implementation roadmap

### Milestone A — Tutorial foundation / academic submission

Implement the assigned Kaleidoscope tutorial, using the existing repository and preserving recognizable tutorial functionality and theory. Establish a working lexer, parser, AST, IR generation and the subsequent required tutorial features. Improve internal organization and diagnostics where practical.

Keep the tutorial-compatible behavior independently runnable and testable; tag the exact submission snapshot. Any extensions must not silently replace required tutorial functionality. Confirm the permitted scope with the course staff. The academic report is a separate, independently authored account of the actual implementation and understanding, not this design specification.

### Milestone B — Target language frontend

Add explicit source types, typed function signatures, required returns, unit, Boolean semantics, indentation-sensitive blocks and the numeric rules. Extend the parser and semantic analysis incrementally. Keep tests for baseline and new behavior.

### Milestone C — Practical language

Implement structs, modules, a chosen string/collection runtime, iteration, higher-order functions, dot-chain APIs, useful diagnostics and a standalone driver. Establish memory and execution decisions before introducing features that depend on them.

### Milestone D — Distributable toolchain

Define a stable specification and compatibility policy; provide reproducible builds, tests, a CLI, documentation, examples, standard library, installers and supported target platforms. Do not equate passing tutorial examples with production readiness.

## 10. References and precedents

- [LLVM Kaleidoscope tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html) — implementation foundation.
- [Rust unit type](https://doc.rust-lang.org/core/primitive.unit.html) — singleton completion value.
- [Kotlin Unit](https://kotlinlang.org/api/core/kotlin-stdlib/kotlin/-unit/) — another unit-type precedent.
- [Rust field access expressions](https://doc.rust-lang.org/reference/expressions/field-expr.html) — field selection syntax.

The precedents motivate individual design choices; this language is not intended to be compatible with them.

## 11. Design status

Confirmed direction: strong explicit static typing; numeric interoperability; indentation; explicit function returns with unit instead of void; functional-first expression semantics; no conventional OOP initially; structs and dot notation as planned features; one canonical idiom where practical; reasonably high native performance.

The exact syntax and semantics of unfinished features remain proposals. Update this document as decisions are validated by implementation and tests.
