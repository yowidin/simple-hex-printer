include(CTest)

Include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
   Catch2
   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
   GIT_TAG v3.0.0-preview4
)

FetchContent_MakeAvailable(Catch2)
message(STATUS "Catch2: ${Catch2_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

add_executable(shp_tests
   src/format_backup.cpp
   src/integral_hex_writer.cpp
   src/iterator_hex_writer.cpp
)

set_target_properties(shp_tests PROPERTIES CXX_STANDARD 11)

target_link_libraries(shp_tests
   PRIVATE SimpleHexPrinter::library
   PRIVATE Catch2::Catch2WithMain
)

add_test(NAME Catch2Tests COMMAND "shp_tests")