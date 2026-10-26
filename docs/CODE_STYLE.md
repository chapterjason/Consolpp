# Code style

Formatting and most rules are enforced by `.clang-format` and `.clang-tidy`. This file sums up what they enforce and lists the rules no tool can enforce.

## Enforced by the tools (tldr)

- 4 spaces, 120 columns, LF, newline at end of file.
- Braces on their own line for namespaces, classes, structs, unions, enums and functions; on the same line for `if`, `for`, `while`, `switch`, `try`, lambdas, `} else {`, `} catch {`.
- Braces always, even around single statements. Nothing short on one line: functions, lambdas, blocks, enums, cases.
- Empty classes and functions are split: `{` and `}` on separate lines. Empty namespaces stay `{}`.
- Namespace contents indented. `public:` / `private:` indented one level, members two. `case` indented inside `switch`. `extern "C"` contents indented. Nested `#if` indented after the hash: `#    define`.
- `int* value`, `int& value`, `const int`.
- Wrapping: one parameter / argument per line, aligned after the opening bracket. Never break after the return type or after `=`.
- Initializer lists: `: first{first},` with the following initializers aligned below.
- Base classes on one line if they fit, otherwise `: public A,` with the next base aligned below.
- `template <...>` on its own line, `requires` on its own line, indented.
- Wrapped expressions: the operator starts the line, one operand of the lowest precedence operator per line.
- Attributes like `[[nodiscard]]` on their own line.
- Includes in three blocks: `"project.hpp"`, `<standard>`, `<other.h>`.
- Blank line between definitions, at most one blank line in a row.
- Naming: namespaces, types, enums, enum values, concepts and template parameters `PascalCase`; functions, methods, variables, parameters and members `camelBack`; macros `UPPER_CASE`.
- clang-tidy: bugprone, cert, clang-analyzer, concurrency, cppcoreguidelines, misc, modernize, performance, portability and readability, every finding is an error. Compiler warnings are errors too.

## Principle

- Explicit over implicit.

## Headers

- Headers use `#pragma once`, no include guards.

## Parameters

- Value parameters the function does not modify are `const`, e.g. `int direction(const char key)`.

## Variables

- Local values known at compile time are `constexpr`, not `const`.

## Blank lines

- A `return` that follows other statements in the same block is separated from them by a blank line.
- Control statement blocks (`if`, `for`, `while`, `do`, `switch`, `try`) are separated from the statements before and after them by a blank line.
- Statements are grouped by kind, and each group is separated by a blank line:
  - declaring new variables, e.g. `const std::size_t length = line.size();`
  - changing state of an outer scope, e.g. `this->descriptor = -1;` or `total += length;`
  - other statements such as calls, e.g. `close(this->descriptor);`

## Classes, structs and unions

- Members are always under an explicit `public:`, `protected:` or `private:`, also in structs and unions.
- Sections are ordered `private:`, `protected:`, `public:`.
- Members are always accessed through `this->`, e.g. `this->boxWidth`.
- Members are named for what they are, without prefixes or suffixes like `m_`, `_`, `m` or `_m`. Parameters and locals may share a member's name, `this->` keeps them apart.
- Members get no empty `{}` initializer: either an explicit value, e.g. `int count{0};`, or none, e.g. `Element content;`. Exception: members that may be left out in designated initialization, and members that must start zeroed like buffers, get `{}`, e.g. `std::string_view description{};` or `std::array<char, 65536> buffer{};`.

## Templates

- A constraint that is a single concept goes into the template parameter: `template <std::integral Value>`.
- `requires` clauses are only used for combined constraints, e.g. `requires std::integral<Value> && (sizeof(Value) <= 4)`.

## Naming

- Getters are named `get<Name>`, e.g. `getColumns()`.
- Functions that create and return a new object are named `create<Name>`, e.g. `createSquare()`.
- Functions that build an object from another representation are named `from<Name>`, e.g. `Color::fromHex()`.

## Errors

- Errors are thrown, never returned: no `std::variant`, `std::optional` or error codes for failures. The success path stays free of checks.

## C functions

- C and POSIX functions are called without `::`, e.g. `close(descriptor)`. Only where a Consolpp function of the same name hides them, `::` is required, e.g. `::read()` inside `Consolpp::System::Io::read()`.

## Files and folders

- One type (class, struct, union, enum, concept) per file. The file is named after the type, e.g. `Cursor.hpp` / `Cursor.cpp`.
- Free functions instead of classes that only hold static methods. They live in a namespace with its own folder like any other, and the file is named after the function, e.g. `Text/Utf8/Encode.hpp` contains `Consolpp::Text::Utf8::encode()`.
- Files are grouped into folders by topic; a folder holds what belongs together, not everything.
- Each library is its own target with its own folder in `libs/`, holding a `Consolpp/` folder for the root namespace `Consolpp`. `libs/Console/Consolpp/` is the root namespace itself, e.g. `libs/Console/Consolpp/Screen/Cursor.hpp` contains `Consolpp::Screen::Cursor`. The other libraries are their namespace inside it, e.g. `libs/Text/Consolpp/Text/Utf8/Encode.hpp` contains `Consolpp::Text::Utf8::encode()`.
- Headers are included with the `Consolpp/` prefix, e.g. `#include "Consolpp/Text/Utf8/Encode.hpp"`, also inside the same folder, so they never resolve to another project's header of the same path.
- Folders map 1:1 to namespaces and are `PascalCase` like them.

## Tests

- `tests/` is the root namespace `Consolpp::Tests` and is split by test type, each its own folder and namespace:
  - `tests/Unit/`: one class in isolation. Mirrors the namespaces of `libs/`, one test file per source file, named `<File>Test.cpp`, e.g. `tests/Unit/Screen/CursorTest.cpp` contains `Consolpp::Tests::Unit::Screen::CursorTest`.
  - `tests/Functional/`: several real classes working together, without external processes.
  - `tests/Integration/`: Consolpp's code against real system resources, e.g. a real pseudo terminal, signals, IPC sockets or a real shell running a generated script.
  - `tests/E2E/`: a built Consolpp program end to end, run the way a user runs it, e.g. completed through a real shell.
  - `tests/Fuzz/`: random input against parsers, e.g. escape sequences, key encoding or IPC frames. One fuzz target per parser.
  - `tests/Stress/`: high load and long runs, e.g. large output bursts, many resizes, reconnect loops.
  - `tests/Benchmark/`: measured performance of hot paths, e.g. screen updates or output throughput.
- Every test follows Arrange, Act, Assert: three blocks separated by a blank line, each starting with a `// Arrange`, `// Act` or `// Assert` comment, exactly one act per test, no helpers that hide the act.
- Functional, Integration, E2E, Stress and Benchmark tests are grouped into folders by feature. Fuzz tests by parser.
