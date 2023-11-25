#include "common.hpp"
#include "parser.hpp"

auto main(int argc, char *argv[]) -> int {
    auto output = parser::parse(argc, argv);

    return print_and_compute<::crc32>(output.file, //
        options{
            .path = std::move(output.path), //
            .show_path = true, //
            .as_decimal = false, //
        });
}
