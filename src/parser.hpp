#pragma once

#include "common.hpp"

#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>

#include <CLI/FormatterFwd.hpp>
#include <filesystem>
#include <format>
#include <optional>
#include <ostream>

namespace parser {
struct result {
    raii::open file;
    std::string path;
    options opts;
};

auto parse(int argc, char *argv[]) -> std::optional<result> {
    const auto name = std::filesystem::path(argv[0]).filename().string();
    CLI::App app(std::format("Hash computation for {} algorithm\n", name), name);
    try {
        options opts;

        const auto enablers_group = "Enable";
        const auto disablers_group = "Disable";
        const auto ranges_group = "Ranges";

        app.add_option("path", opts.path, "Path to file");
        app.add_flag("-x,--prefix-0x", opts.prefix_0x, "Show 0x prefix")->group(enablers_group);
        app.add_flag("-p,--show-path", opts.show_path, "Show path after hash")->group(enablers_group);
        app.add_flag("-u,--upper-case", opts.as_uppercase, "Use uppercase")->group(enablers_group);
        app.add_flag("-d,--dec,--decimal", opts.as_decimal, "Print as decimal number")->group(enablers_group);

        app.add_flag("!--no-prefix-0x", opts.prefix_0x, "Hide 0x prefix")->group(disablers_group);
        app.add_flag("!--no-show-path", opts.show_path, "Hide path after hash")->group(disablers_group);
        app.add_flag("!--no-upper-case", opts.as_uppercase, "Use lowercase\n")->group(disablers_group);
        app.add_flag("!--no-dec", opts.as_decimal, "Print as hex number")->group(disablers_group);
        app.add_flag("!--no-decimal", opts.as_decimal)->group(disablers_group);

        app.add_option("-o,--offset,--from-offset", opts.offset, "Calulcate from offset")->group(ranges_group);
        app.add_option("-s,--size,--n-bytes", opts.size, "Calculate N bytes")->group(ranges_group);

        app.parse(argc, argv);

        if (opts.path.empty()) {
            opts.path = "-";
            return result{
                .file{raii::open::system_io{}, STDIN_FILENO}, //
                .opts{std::move(opts)}, //
            };
        }

        return result{
            .file{opts.path.c_str(), O_RDONLY}, //
            .opts{std::move(opts)}, //
        };
    } catch (...) {
        std::cerr << app.help("", CLI::AppFormatMode::All) << std::endl;
    }

    return {};
}

} // namespace parser
