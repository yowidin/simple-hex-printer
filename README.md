# Simple Hex Printer (shp) ![status](https://github.com/yowidin/simple-hex-printer/actions/workflows/main.yml/badge.svg)

A simple library for printing your values as hex strings without an extra hassle of keeping track of your I/O flags. 
This library doesn't try to be the fastest one, or the most flexible one. Instead it tries to be a simple one with some 
(hopefully) reasonable defaults.


## Usage 

The recommended way to include this library in your project is to use the CMake's `FetchContent` functionality, for example:

```cmake
cmake_minimum_required(VERSION 3.16)
project(integration_test)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
   shp
   SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../
)
FetchContent_MakeAvailable(shp)

add_executable(test main.cpp)
target_link_libraries(test PRIVATE simple_hex_printer)
```

When using the library from your code is fairly simple:

```c++
#include <shp/shp.h>

#include <array>
#include <cstdint>
#include <iostream>

int main() {
   struct foo {
      int bar;
      int baz;
   } pod = {10, 20};
   std::cout << "POD: " << shp::hex(pod, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{})
             << std::endl;

   uint16_t integer = 0xBEEF;
   std::cout << "Integer: " << shp::hex(integer) << std::endl;

   std::array<foo, 4> array{{{30, 40}, {50, 60}, {70, 80}, {90, 100}}};
   std::cout << "Array:\n" << shp::hex(array) << std::endl;
}
```

This will result in an output, similar to the one presented below.

```text
POD: 0A00000014000000
Integer: 0xBEEF
Array:
0x00: 1E 00 00 00 28 00 00 00 32 00 00 00 3C 00 00 00  ....(...2...<...
0x10: 46 00 00 00 50 00 00 00 5A 00 00 00 64 00 00 00  F...P...Z...d...
```