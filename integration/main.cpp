/**
 * @file   main.cpp
 * @author Dennis Sitelew
 * @date   May. 28, 2022
 */

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
