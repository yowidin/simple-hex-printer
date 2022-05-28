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
target_link_libraries(test PRIVATE SimpleHexPrinter::library)
