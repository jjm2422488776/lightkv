#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace lightkv {

struct Entry {
    std::string value;
    std::int64_t created_at_ms{0};
    std::int64_t updated_at_ms{0};

    bool has_expire{false};
    std::int64_t expire_at_ms{0};

    Entry() = default;

    Entry(std::string val,
          std::int64_t created_ms,
          std::int64_t updated_ms,
          bool expire = false,
          std::int64_t expire_at = 0)
        : value(std::move(val)),
          created_at_ms(created_ms),
          updated_at_ms(updated_ms),
          has_expire(expire),
          expire_at_ms(expire_at) {}
};

}  // namespace lightkv