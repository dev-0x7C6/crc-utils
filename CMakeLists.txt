cmake_minimum_required(VERSION 3.16...3.27)

project(crc-utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(ZLIB REQUIRED)

add_library(common INTERFACE src/common.hpp src/parser.hpp)
target_include_directories(common INTERFACE src)
target_link_libraries(common INTERFACE spdlog::spdlog ZLIB::ZLIB fmt::fmt
	CLI11::CLI11)

add_executable(adler32sum targets/adler32/main.cpp)
add_executable(crc32sum targets/crc32/main.cpp)

target_link_libraries(adler32sum PRIVATE common)
target_link_libraries(crc32sum PRIVATE common)

include(GNUInstallDirs)
install(
  TARGETS adler32sum
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  TARGETS crc32sum
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
