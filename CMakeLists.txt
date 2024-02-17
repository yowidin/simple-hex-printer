cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(SimpleHexPrinter VERSION 0.1.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

set(SHP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(SHP_GENERATED_INCLUDE_DIR "${SHP_GENERATED_DIR}/include")
set(SHP_GENERATED_CMAKE_DIR "${SHP_GENERATED_DIR}/cmake")

set(SHP_GENERATED_EXPORT_HEADER "${SHP_GENERATED_INCLUDE_DIR}/shp/export.h")
file(MAKE_DIRECTORY ${SHP_GENERATED_INCLUDE_DIR})

add_library(simple_hex_printer INTERFACE)

target_include_directories(simple_hex_printer
   INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
   INTERFACE $<BUILD_INTERFACE:${SHP_GENERATED_INCLUDE_DIR}>

   INTERFACE $<INSTALL_INTERFACE:include>
)

set_target_properties(simple_hex_printer PROPERTIES
   OUTPUT_NAME simple-hex-printer

   CXX_STANDARD 14
)

if(BUILD_TESTS)
   include(CTest)
   enable_testing()
   add_subdirectory(test)
endif()

# --- Configure installation settings --- #
include(GenerateExportHeader)

include(cmake/install_helper.cmake)
include(cmake/package_helper.cmake)