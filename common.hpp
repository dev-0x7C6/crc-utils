#pragma once

#include <fcntl.h>
#include <zlib.h>

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

namespace raii {
struct open {
    template <typename... Ts>
    open(Ts &&...args)
            : descriptor(::open(std::forward<Ts>(args)...)) {}
    ~open() {
        if (descriptor != -1)
            ::close(descriptor);
    }

    operator auto() const noexcept {
        return descriptor;
    }

private:
    const int descriptor{};
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

template <auto fn, typename hash_t = std::uint32_t>
auto print_and_compute(const int fd) -> hash_t {
    const auto ret = compute<fn, hash_t>(fd);
    if (!ret) return 1;
    auto logger = spdlog::stdout_logger_st("console");
    logger->set_pattern("%v");
    if constexpr (sizeof(hash_t) == 4)
        logger->info("0x{:08x}", ret.value());
    return 0;
}
