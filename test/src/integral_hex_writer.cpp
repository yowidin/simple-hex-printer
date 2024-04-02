/**
 * @file   integral_hex_writer.cpp
 * @author Dennis Sitelew
 * @date   Apr. 17, 2022
 */

#include <catch2/catch_test_macros.hpp>

#include <shp/shp.h>

#include <sstream>
#include <string>

using namespace std;

TEST_CASE("Full Printing", "[integral_hex_writer]") {
   ostringstream os;

   SECTION("bool") {
      const bool v = true;
      os << shp::integral_hex_writer<decltype(v)>{v};
      REQUIRE(os.str() == "0x01");
   }

   SECTION("char") {
      const char v = 'a';
      os << shp::integral_hex_writer<decltype(v)>{v};
      REQUIRE(os.str() == "0x61");
   }

   SECTION("unsigned char") {
      const unsigned char v = 'a';
      os << shp::integral_hex_writer<decltype(v)>{v};
      REQUIRE(os.str() == "0x61");
   }

   SECTION("fill i32") {
      const std::int32_t v = -16;
      os << shp::integral_hex_writer<decltype(v)>{v};
      REQUIRE(os.str() == "0xFFFFFFF0");
   }

   SECTION("fill u32") {
      const std::uint32_t v = 0x10;
      os << shp::integral_hex_writer<decltype(v)>{v};
      REQUIRE(os.str() == "0x00000010");
   }

   SECTION("u32 - NNL") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::NoPrefix, shp::NoFill, shp::LowerCase>{v};
      REQUIRE(os.str() == "fa");
   }

   SECTION("u32 - NNU") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::NoPrefix, shp::NoFill, shp::UpperCase>{v};
      REQUIRE(os.str() == "FA");
   }

   SECTION("u32 - NFL") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::NoPrefix, shp::Fill, shp::LowerCase>{v};
      REQUIRE(os.str() == "000000fa");
   }

   SECTION("u32 - NFU") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::NoPrefix, shp::Fill, shp::UpperCase>{v};
      REQUIRE(os.str() == "000000FA");
   }

   SECTION("u32 - PFL") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::Prefix, shp::Fill, shp::LowerCase>{v};
      REQUIRE(os.str() == "0x000000fa");
   }

   SECTION("u32 - PNU") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::Prefix, shp::NoFill, shp::UpperCase>{v};
      REQUIRE(os.str() == "0xFA");
   }

   SECTION("u32 - PFL") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::Prefix, shp::Fill, shp::LowerCase>{v};
      REQUIRE(os.str() == "0x000000fa");
   }

   SECTION("u32 - PFU") {
      const std::uint32_t v = 0xFA;
      os << shp::integral_hex_writer<decltype(v), shp::Prefix, shp::Fill, shp::UpperCase>{v};
      REQUIRE(os.str() == "0x000000FA");
   }
}

TEST_CASE("Simplified Printing", "[integral_hex_writer]") {
   ostringstream os;

   SECTION("bool") {
      os << shp::hex(true) << " " << shp::hex(false);
      REQUIRE(os.str() == "0x01 0x00");
   }

   SECTION("char") {
      const char v = 'a';
      os << shp::hex(v);
      REQUIRE(os.str() == "0x61");
   }

   SECTION("unsigned char") {
      const unsigned char v = 'a';
      os << shp::hex(v);
      REQUIRE(os.str() == "0x61");
   }

   SECTION("fill i32") {
      const std::int32_t v = -16;
      os << shp::hex(v);
      REQUIRE(os.str() == "0xFFFFFFF0");
   }

   SECTION("fill u32") {
      const std::uint32_t v = 0x10;
      os << shp::hex(v);
      REQUIRE(os.str() == "0x00000010");
   }

   SECTION("u32 format as function arguments") {
      const std::uint32_t v = 0xFA;
      os << shp::hex(v, shp::NoPrefix{}, shp::NoFill{}, shp::LowerCase{});
      REQUIRE(os.str() == "fa");
   }

   SECTION("u32 format as template arguments") {
      const std::uint32_t v = 0xFA;
      os << shp::hex<decltype(v), shp::NoPrefix, shp::NoFill, shp::LowerCase>(v);
      REQUIRE(os.str() == "fa");
   }
}

TEST_CASE("String conversion", "[integral_hex_writer]") {
   SECTION("bool") {
      const bool v = false;
      REQUIRE(shp::hex_str(v) == "0x00");
   }

   SECTION("char") {
      const char v = 'a';
      REQUIRE(shp::hex_str(v) == "0x61");
   }

   SECTION("unsigned char") {
      const unsigned char v = 'a';
      REQUIRE(shp::hex_str(v) == "0x61");
   }

   SECTION("fill i32") {
      const std::int32_t v = -16;
      REQUIRE(shp::hex_str(v) == "0xFFFFFFF0");
   }

   SECTION("fill u32") {
      const std::uint32_t v = 0x10;
      REQUIRE(shp::hex_str(v) == "0x00000010");
   }

   SECTION("u32 format as function arguments") {
      const std::uint32_t v = 0xFA;
      REQUIRE(shp::hex_str(v, shp::NoPrefix{}, shp::NoFill{}, shp::LowerCase{}) == "fa");
   }

   SECTION("u32 format as template arguments") {
      const std::uint32_t v = 0xFA;
      REQUIRE(shp::hex_str<decltype(v), shp::NoPrefix, shp::NoFill, shp::LowerCase>(v) == "fa");
   }

   SECTION("u32 format as template arguments") {
      const std::uint32_t v = 0xFA;
      const std::uint32_t &r = v;
      REQUIRE(shp::hex_str(r) == "0x000000FA");
   }
}
