/**
 * @file   iterator_hex_writer.cpp
 * @author Dennis Sitelew
 * @date   Apr. 18, 2022
 */

#include <catch2/catch_test_macros.hpp>

#include <shp/shp.h>

#include <numeric>
#include <string>

using namespace std;

TEST_CASE("Full multiline printing", "[iterator_hex_writer]") {
   ostringstream os;

   SECTION("char[]") {
      std::array<std::uint8_t, 16> v{};
      std::iota(std::begin(v), std::end(v), 0);

      const std::string expected{"000102030405060708090A0B0C0D0E0F"};

      SECTION("explicit") {
         os << shp::iterator_hex_writer<decltype(std::cbegin(v)), shp::NoOffsets, shp::NoNibbleSeparation,
                                        shp::SingleRow, shp::NoASCII, shp::UpperCase>{std::cbegin(v), std::cend(v)};
         REQUIRE(os.str() == expected);
      }

      SECTION("with helper") {
         os << shp::hex(v, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                        shp::UpperCase{});
         REQUIRE(os.str() == expected);
      }

      SECTION("Directly to string") {
         auto result = shp::hex_str(v, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                                    shp::UpperCase{});
         REQUIRE(result == expected);
      }
   }

   SECTION("type[]") {
      struct foo {
         uint8_t a;
         uint8_t b;
      };
      static_assert(std::is_standard_layout<foo>::value, "Standard layout expected!");

      std::array<foo, 2> v{};
      int i = 2;
      for (auto &tmp : v) {
         tmp.a = i++;
         tmp.b = tmp.a * 10;
      }

      const std::string expected{"0214031E"};

      SECTION("explicit") {
         os << shp::iterator_hex_writer<decltype(std::cbegin(v)), shp::NoOffsets, shp::NoNibbleSeparation,
                                        shp::SingleRow, shp::NoASCII, shp::UpperCase>{std::cbegin(v), std::cend(v)};
         REQUIRE(os.str() == expected);
      }

      SECTION("with helper") {
         os << shp::hex(v, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                        shp::UpperCase{});
         REQUIRE(os.str() == expected);
      }

      SECTION("Directly to string") {
         auto result = shp::hex_str(v, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                                    shp::UpperCase{});
         REQUIRE(result == expected);
      }
   }

   SECTION("type") {
      struct foo {
         uint8_t a;
         uint8_t b;
      } value{0xBE, 0xEF};

      const std::string expected{"BEEF"};

      SECTION("explicit") {
         auto start = std::addressof(value);
         auto end = start + 1;
         os << shp::iterator_hex_writer<const foo *, shp::NoOffsets, shp::NoNibbleSeparation, shp::SingleRow,
                                        shp::NoASCII, shp::UpperCase>{start, end};
         REQUIRE(os.str() == expected);
      }

      SECTION("with helper") {
         os << shp::hex(value, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                        shp::UpperCase{});
         REQUIRE(os.str() == expected);
      }

      SECTION("Directly to string") {
         auto result = shp::hex_str(value, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{},
                                    shp::NoASCII{}, shp::UpperCase{});
         REQUIRE(result == expected);
      }
   }

   SECTION("initializer_list") {
      std::initializer_list<std::uint8_t> value = {0xDE, 0xAD, 0xBE, 0xEF};
      const std::string expected{"DEADBEEF"};

      SECTION("explicit") {
         auto start = std::cbegin(value);
         auto end = std::cend(value);
         os << shp::iterator_hex_writer<decltype(start), shp::NoOffsets, shp::NoNibbleSeparation, shp::SingleRow,
                                        shp::NoASCII, shp::UpperCase>{start, end};
         REQUIRE(os.str() == expected);
      }

      SECTION("with helper") {
         os << shp::hex(value, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{}, shp::NoASCII{},
                        shp::UpperCase{});
         REQUIRE(os.str() == expected);
      }

      SECTION("Directly to string") {
         auto result = shp::hex_str(value, shp::NoOffsets{}, shp::NoNibbleSeparation{}, shp::SingleRow{},
                                    shp::NoASCII{}, shp::UpperCase{});
         REQUIRE(result == expected);
      }
   }
}

TEST_CASE("Check ASCII Alignment", "[iterator_hex_writer]") {
   ostringstream os;

   SECTION("char[]") {
      std::array<std::uint8_t, 6> v{};
      std::iota(std::begin(v), std::end(v), 0x30);

      os << shp::iterator_hex_writer<decltype(std::cbegin(v)), shp::NoOffsets, shp::SeparateNibbles, shp::RowWidth<4>,
                                     shp::PrintASCII, shp::UpperCase>{std::cbegin(v), std::cend(v)};

      REQUIRE(os.str() == "30 31 32 33  0123\n"
                          "34 35        45");
   }
}

TEST_CASE("Check non ambiguous", "[iterator_hex_writer]") {
   // hex_str call should be non-ambiguous
   std::array<std::uint8_t, 2> v{};
   auto result = shp::hex_str(v);
   REQUIRE(!result.empty());
}

TEST_CASE("Initializer lists should work", "[iterator_hex_writer]") {
   auto result = shp::hex_str({0xDE, 0xAD, 0xBE, 0xEF});
   REQUIRE(!result.empty());
}
