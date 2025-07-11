# Chemical Formula Parser

> Lexing and parsing chemical formulas into element-count mappings, with support for error handling and nested groups.

[![CI](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/ci.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/ci.yml)
[![Clang-Format](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-format.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-format.yml)
[![Clang-Tidy](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/ramsafin/chemical-formula-parser/actions/workflows/clang-tidy.yml)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)

## Table of Contents

- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Build Presets](#build-presets)
- [Workflow Presets](#workflow-presets)
- [Building and Testing](#building-and-testing)
- [Developer Tooling](#developer-tooling)
  - [Code Formatting](#code-formatting)
  - [Static Analysis](#static-analysis)
- [Installation](#installation)
- [License](#license)

## Project Structure

```text
chemical-formula-parser/
├── app/                 # Demo application
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

| **Preset**            | **Notes**                                           |
| :-------------------- |:--------------------------------------------------- |
| `check-sanitize-*`    | `Sanitize` → build → run tests (GCC or Clang)       |

List available workflow presets:
```bash
cmake --workflow --list-presets
```

Run workflows with:
```bash
cmake --workflow --preset check-sanitize
```

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

## Developer Tooling

### Code Formatting

Format all C++ files using `.clang-format`.

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

> Configurable via `.clang-tidy`, with selective checks enabled.

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
find_package(chemical_formula_parser REQUIRED)
add_executable(app ...)
target_link_libraries(app PRIVATE chemical_formula_parser::chemical_formula_parser)
```

Make sure CMake knows where to find the installed package:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/install ..
```

## License

This project is licensed under the **Apache License 2.0**.

You are free to use, modify, distribute, and include this code in commercial or open-source projects.
