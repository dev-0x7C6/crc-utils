#pragma once

#include "common.hpp"

namespace parser {
struct result {
    raii::open file;
    std::string path;
};

auto parse(int argc, char *argv[]) -> result {
    if (argc == 1)
        return result{
            .file{raii::open::system_io{}, STDIN_FILENO}, //
            .path{"-"} //
        };

    return result{
        .file{argv[1], O_RDONLY}, //
        .path{argv[1]} //
    };
}

} // namespace parser
