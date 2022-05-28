/**
 * @file   format_backup.cpp
 * @author Dennis Sitelew
 * @date   Apr. 17, 2022
 */

#include <catch2/catch_test_macros.hpp>

#include <shp/shp.h>

#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

TEST_CASE("Make sure that stream format is properly restored", "[format_backup]") {
   SECTION("Fill should be sticky") {
      ostringstream os;
      REQUIRE(os.fill() == ' ');

      os << 11 << setfill('0') << setw(3) << 22;
      REQUIRE(os.str() == "11022");
      REQUIRE(os.fill() == '0');

      os << setw(3) << 33;
      REQUIRE(os.str() == "11022033");
      REQUIRE(os.fill() == '0');
   }

   SECTION("Format backup should restore fill property") {
      ostringstream os;
      REQUIRE(os.fill() == ' ');

      {
         shp::format_backup backup{os};
         os << 11 << setfill('0') << setw(3) << 22;
         REQUIRE(os.str() == "11022");
         REQUIRE(os.fill() == '0');
      }

      os << setw(3) << 33;
      REQUIRE(os.fill() == ' ');
      REQUIRE(os.str() == "11022 33");
   }
}