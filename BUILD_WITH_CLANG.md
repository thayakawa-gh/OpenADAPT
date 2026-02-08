# Building OpenADAPT with Clang

## Overview

OpenADAPT can be built with Clang instead of GCC. Clang often provides faster compilation times and better error messages for complex template-heavy code like OpenADAPT.

## Requirements

- Clang 18.0 or later (tested with Clang 18.1.3)
- CMake 3.28 or later
- C++20 support

## Building with Clang

### Method 1: Using Environment Variables

The simplest way to use Clang is to set the `CC` and `CXX` environment variables before running CMake:

```bash
mkdir build
cd build
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Method 2: Using CMake Command Line

You can also specify the compiler directly in the CMake command:

```bash
mkdir build
cd build
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Method 3: Using the Build Script

For convenience, we provide a build script:

```bash
./build_with_clang.sh
```

This script will automatically configure and build OpenADAPT with Clang.

## Building Examples and Tests

To build with examples:

```bash
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
make -j$(nproc)
```

To build with tests (requires GTest):

```bash
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON
make -j$(nproc)
```

## Installation

After building, you can install OpenADAPT:

```bash
make install
```

Or specify a custom installation prefix:

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
make
make install
```

## Performance Comparison

In our tests, Clang often compiles OpenADAPT faster than GCC, especially for template-heavy translation units. Your mileage may vary depending on your specific hardware and code.

### Typical Build Times (approximate)

| Compiler | Build Type | Time (approx) |
|----------|------------|---------------|
| GCC 13   | Release    | Baseline      |
| Clang 18 | Release    | ~20-40% faster|

## Troubleshooting

### Clang not found

If CMake cannot find Clang, make sure it's installed:

```bash
# Ubuntu/Debian
sudo apt install clang

# Fedora/RHEL
sudo dnf install clang

# macOS (Xcode Command Line Tools)
xcode-select --install
```

### Module Support

C++20 modules are experimentally supported with Clang >= 20. To build with modules:

```bash
CC=clang CXX=clang++ cmake .. -DENABLE_MODULE=ON
make
```

Note: Module support is still experimental and may have issues.

## Additional Notes

- Both GCC and Clang produce functionally equivalent binaries
- Some compiler warnings may differ between GCC and Clang
- Clang typically provides more readable error messages for template errors
- The library is header-only, so compiler choice only affects build time, not runtime performance
