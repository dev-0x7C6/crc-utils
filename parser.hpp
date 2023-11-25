#pragma once

#include "common.hpp"

#include <CLI/CLI.hpp>

namespace parser {
struct result {
    raii::open file;
    std::string path;
    options options;
};

auto parse(int argc, char *argv[]) -> result {
    CLI::App app{"Usage:"};

    options options;
    app.add_option("path", options.path, "path to file");
    app.add_flag("--prefix-0x,!--no-prefix-0x", options.prefix_0x, "show 0x prefix");
    app.add_flag("--show-path,!--no-show-path", options.show_path, "show path after hash");
    app.add_flag("--upper-case,!--no-upper-case", options.as_uppercase, "uppercase");
    app.add_flag("--decimal,!--no-decimal", options.as_decimal, "print as decimal number");
    app.parse(argc, argv);

    if (options.path.empty())
        return result{
            .file{raii::open::system_io{}, STDIN_FILENO}, //
            .options{std::move(options)}, //
        };

    return result{
        .file{options.path.c_str(), O_RDONLY}, //
        .options{std::move(options)}, //
    };
}

} // namespace parser
