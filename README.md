# Chemical Formula Parser

> Lexing and parsing chemical formulas with support for error handling and nested groups.

[![CI](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/ci.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/ramsafin/chemical-formula-parser/graph/badge.svg?token=5M7YCKRVJO)](https://codecov.io/gh/ramsafin/chemical-formula-parser)
[![Clang-Format](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-format.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-format.yml)
[![Clang-Tidy](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-tidy.yml)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)

## Table of Contents

- [Task Overview](#task-overview)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Build Presets](#build-presets)
- [Workflow Presets](#workflow-presets)
- [Building and Testing](#building-and-testing)
- [Application](#application)
- [Parsing Rules](#parsing-rules)
- [Developer Tooling](#developer-tooling)
- [Installation](#installation)
- [License](#license)

## Task Overview

Implement a program that reads a chemical-formula-like string, parses it according to the following rules, and outputs the counts of each element. The output format is free: simply print all found elements and their counts. The program should be designed to be easily extendable as new parts are added.

### Part 1. Single-element tokens

The input is a single element token: an uppercase letter followed by optional lowercase letters, with an optional positive integer count (default = 1). Invalid tokens (zero or negative counts, incorrect casing, leading digits) must be rejected with a clear error.

Examples:
```text
Fe   # => Fe: 1
Fe2  # => Fe: 2
Ag12 # => Ag: 12
N0   # => Error
N-1  # => Error
cl   # => Error
2F   # => Error
```

### Part 2. Multiple elements

The input may contain a sequence of element tokens.
Counts of repeated elements are accumulated.

Examples:
```text
Fe2O3  # => Fe: 2, O: 3
H2SO4  # => H: 2, S: 1, O: 4
HOH    # => H: 2, O: 1
```

### Part 3. Bracketed groups

Support grouping with parentheses `()` and square brackets `[]`, each optionally followed by a multiplier.
Groups may nest and multiply all contained element counts.

Examples:
```text
Fe2(SO4)3         # => Fe: 2, S: 3, O: 12
K[Fe(NO3)2]4      # => K: 1, Fe: 4, N: 8, O: 24
```

### Part 4. Ligand groups (hydrate notation)

Support ligand groups separated by `*` (asterisk or star). Both sides of the `*` may themselves be full formulas, optionally prefixed by a multiplier.

Example:
```text
CuSO4*5H2O      # => Cu: 1, S: 1, O: (4 + 5), H: 10
```

## Features

- C++20 compatible (uses `std::string_view`)
- AST-based parser for chemical formulas:
  - Single‐element tokens (e.g. `Fe`, `O2`)
  - Multi‐element accumulation (e.g. `H2SO4`, `Fe2Fe3`)
  - Bracketed groups with multipliers and nesting:
    - Parentheses `(...)` and square brackets `[...]`
    - Arbitrary nesting depth (e.g. `K[Fe(NO3)2]4`)
- Exception-based error handling:
  - `TokenizerError` for lexing issues (invalid characters, zero or leading-zero counts, empty input)
  - `ParserError` for grammar errors (unexpected tokens, mismatched or empty groups)
- CLI application (for demo purposes)
- Comprehensive unit tests

## Project Structure

```text
chemical-formula-parser/
├── app/                 # CLI application
├── cmake/               # Custom CMake modules (warnings, sanitizers, tooling)
├── include/             # Public headers
├── src/                 # Library source files
├── tests/               # Unit tests using GoogleTest
├── .clang-format        # Formatting rules
├── .clang-tidy          # Static analysis configuration
├── CMakeLists.txt       # Top-level CMake build configuration
├── CMakePresets.json    # Build, test, and workflow presets
└── README.md
```

## Prerequisites

Before building the project, make sure the following tools are installed on your system:
- **CMake ≥ 3.23** (tested with 3.28+)
- **C++ Compiler**: GCC ≥ 10 or Clang ≥ 12
- **Ninja** (optional; recommended), or your generator of choice
- **gcovr** (for code coverage reports)
- **clang-format**, **clang-tidy**, **cppcheck** (optional diagnostics)

> Tools not found are skipped with a warning; features gated by CMake option.

## Build Presets

This project uses [**CMake Presets**](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) to simplify configuration.

| **Preset**          | **Notes**                                              |
| :------------------ |:------------------------------------------------------ |
| `Debug`             | Debugging with no optimizations                        |
| `Release`           | Optimized builds without debug symbols                 |
| `RelWithDebInfo`    | Optimized build with debug symbols (**recommended**)   |
| `Sanitize`          | Builds with runtime checks enabled (ASan/UBSan)        |

List available presets:
```bash
cmake --list-presets
```

## Workflow Presets

| **Preset**            | **Notes**                                                         |
| :-------------------- |:----------------------------------------------------------------- |
| `check-sanitize-*`    | `Sanitize` → build → run tests (GCC or Clang)                     |
| `coverage-report`     | `Coverage` → build → run tests (GCC) + generate report (manually) |

List available workflow presets:
```bash
cmake --workflow --list-presets
```

Run workflows with:
```bash
cmake --workflow --preset check-sanitize
```

### Coverage Report

```bash
cmake --workflow --preset coverage-report
cmake --build --preset gcc-Coverage --target coverage
```
> This will generate an XML coverage report `./build/gcc-Coverage/coverage.xml`

## Building and Testing

Configure and build:
```bash
cmake --preset gcc-RelWithDebInfo
cmake --build --preset gcc-RelWithDebInfo
```

Run unit tests:
```bash
ctest --preset gcc-RelWithDebInfo
```

## Application

A minimal CLI app is included (see [`app/main.cpp`](app/main.cpp)) that shows how to use the parser and handle errors. You can either pass one or more formulas as arguments, or run it interactively.

```bash
# escape paren, brackets, and `*` operator!
./build/gcc-RelWithDebInfo/cfp_app H2O Fe2\(SO4\)3 CuSO4\*5H2O
Formula: H2O
O: 1
H: 2
----
Formula: Fe2(SO4)3
O: 12
S: 3
Fe: 2
----
Formula: CuSO4*5H2O
H: 10
O: 9
S: 1
Cu: 1
----
```

Interactive REPL:
```bash
./build/gcc-RelWithDebInfo/cfp_app
Enter formula (Ctrl-D to quit):
> K[Fe(NO3)2]4
O: 24
N: 8
Fe: 4
K: 1
> CuSO4*5H2O
H: 10
O: 9
S: 1
Cu: 1
```

## Parsing Rules

Two‐phase approach:
1. Lexing into tokens.
2. Recursive‐descent parsing into AST.

```bnf
<ligand>  ::= <unit> ( "*" <unit> )*
<unit>    ::= [ <Number> ] <formula>
<formula> ::= <group>*
<group>   ::= <Element> [ <Number> ]
            | "(" <formula> ")" [ <Number> ]
            | "[" <formula> "]" [ <Number> ]

<Element> ::= UppercaseLetter { LowercaseLetter }
<Number>  ::= Digit { Digit }   ; positive integer, no leading zeros

; single-character tokens
LParen   ::= "("
RParen   ::= ")"
LBracket ::= "["
RBracket ::= "]"
Star     ::= "*"            ; ligand operator
End      ::= end‐of‐input   ; EOF marker
```

## Developer Tooling

### Code Formatting

Format all C++ files using [`.clang-format`](.clang-format).

Check formatting:
```bash
cmake --preset gcc-RelWithDebInfo
cmake --build --preset gcc-RelWithDebInfo --target format-check
```

Reformat everything:
```bash
cmake --build --preset gcc-RelWithDebInfo --target format
```

### Static Analysis

Run `clang-tidy` analysis:
```bash
cmake --preset gcc-RelWithDebInfo
cmake --build --preset gcc-RelWithDebInfo --target clang-tidy
```

> Configurable via [`.clang-tidy`](.clang-tidy), with selective checks enabled.

Run `cppcheck` analysis:
```bash
cmake --preset gcc-RelWithDebInfo
cmake --build --preset gcc-RelWithDebInfo --target cppcheck
```

## Installation

```bash
cmake --preset gcc-RelWithDebInfo
cmake --build --preset gcc-RelWithDebInfo
cmake --install build/gcc-RelWithDebInfo --prefix install  # or /usr/local
```

- This installs the library to the `install/` directory.
- You can also install system-wide with `--prefix /usr/local` (requires `sudo`).

After installation, consume the library like this in another CMake project:
```bash
find_package(cfp REQUIRED)
add_executable(app ...)
target_link_libraries(app PRIVATE cfp::cfp)
```

Make sure CMake knows where to find the installed package:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/install ..
```

## License

This project is licensed under the **Apache License 2.0**.

You are free to use, modify, distribute, and include this code in commercial or open-source projects.
