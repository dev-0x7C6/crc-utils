cmake_minimum_required(VERSION 3.16...3.27)

project(crc-utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(adler32 adler32.cpp common.hpp parser.hpp)
add_executable(crc32 crc32.cpp common.hpp parser.hpp)

set(public-libraries spdlog::spdlog ZLIB::ZLIB fmt::fmt CLI11::CLI11)

target_link_libraries(adler32 ${public-libraries})
target_link_libraries(crc32 ${public-libraries})

include(GNUInstallDirs)
install(
  TARGETS adler32
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
  TARGETS crc32
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
