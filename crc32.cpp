#include "common.hpp"

auto main(int argc, char *argv[]) -> int {
    const auto fd = raii::open(argv[1], O_RDONLY);
    return print_and_compute<::crc32>(fd);
}
