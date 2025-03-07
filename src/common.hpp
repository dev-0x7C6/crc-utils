#pragma once

#include <algorithm>
#include <fcntl.h>
#include <sys/types.h>
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
    open() = default;
    open(open &&other)
            : descriptor(other.descriptor)
            , closeable(other.closeable) {
        other.closeable = false;
        other.descriptor = 0;
    }

    template <typename T, typename... Ts>
    open(T &&arg, Ts &&...args)
            : descriptor(::open(std::forward<T>(arg), std::forward<Ts>(args)...)) {}
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
    int descriptor{};
    bool closeable{true};
};
} // namespace raii

struct options {
    std::string path;
    bool prefix_0x{false};
    bool show_path{false};
    bool as_decimal{false};
    bool as_uppercase{false};

    std::optional<std::uint64_t> offset;
    std::optional<std::uint64_t> size;
};

template <auto fn, typename hash_t = std::uint32_t>
auto compute(const int fd, const options &options) -> std::optional<hash_t> {
    std::vector<std::uint8_t> buffer(4096);
    auto ret = fn(0, nullptr, 0);

    const auto seek = ::lseek64(fd, options.offset.value_or(0), SEEK_SET);
    if (seek == static_cast<__off64_t>(-1))
        return {};

    auto size_limit = options.size;

    for (;;) {
        std::uint64_t size = ::read(fd, buffer.data(), buffer.size());
        if (size == 0) break;
        if (size <= 0) return {};

        if (size_limit) {
            const auto chunk = std::min(size_limit.value(), size);
            size_limit.value() -= chunk;
            size = chunk;
        }

        ret = fn(ret, buffer.data(), size);

        if (size_limit == 0)
            return ret;
    }

    return ret;
}

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
    const auto ret = compute<fn, hash_t>(fd, options);
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
