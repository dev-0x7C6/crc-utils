#pragma once

#include <fcntl.h>
#include <zlib.h>

#include <cstdint>
#include <format>
#include <optional>
#include <utility>
#include <vector>

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

namespace raii {
struct open {
    struct system_io {};

    template <typename... Ts>
    open(Ts &&...args)
            : descriptor(::open(std::forward<Ts>(args)...)) {}
    open(system_io, const int fd)
            : descriptor(fd)
            , closeable(false) {}

    ~open() {
        if (closeable && descriptor != -1)
            ::close(descriptor);
    }

    operator auto() const noexcept {
        return descriptor;
    }

private:
    const int descriptor{};
    const bool closeable{true};
};
} // namespace raii

template <auto fn, typename hash_t = std::uint32_t>
auto compute(const int fd) -> std::optional<hash_t> {
    std::vector<std::uint8_t> buffer(4096);
    hash_t ret{};

    ::lseek64(fd, 0, SEEK_SET);
    for (;;) {
        const auto size = ::read(fd, buffer.data(), buffer.size());
        if (size == 0) break;
        if (size <= 0) return {};

        ret = fn(ret, buffer.data(), size);
    }

    return ret;
}

struct options {
    std::string path;
    bool prefix_0x{true};
    bool show_path{true};
    bool as_decimal{false};
    bool as_uppercase{false};
};

constexpr auto to_digest(std::integral auto value, const options &options) -> std::string {
    if (options.as_decimal)
        return std::format("{}", value);

    const auto prefix = options.prefix_0x ? "0x" : "";
    const auto padding = sizeof(value) * 2;

    if (options.as_uppercase)
        return std::format("{}{:0{}X}", prefix, value, padding);

    return std::format("{}{:0{}x}", prefix, value, padding);
}

template <auto fn, typename hash_t = std::uint32_t>
auto print_and_compute(const int fd, const options &options = {}) -> hash_t {
    const auto ret = compute<fn, hash_t>(fd);
    if (!ret) return 1;

    const auto digest = to_digest(ret.value(), options);

    auto logger = spdlog::stdout_logger_st("console");
    logger->set_pattern("%v");

    if (options.show_path)
        logger->info("{}  {}", digest, options.path);
    else
        logger->info("{}", digest);

    return 0;
}
