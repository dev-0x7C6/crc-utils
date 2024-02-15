#include "common.hpp"
#include "parser.hpp"

auto main(int argc, char *argv[]) -> int {
    auto ret = parser::parse(argc, argv);
    if (!ret)
        return 1;

    auto &&v = ret.value();
    return print_and_compute<::adler32>(v.file, std::move(v.opts));
}
