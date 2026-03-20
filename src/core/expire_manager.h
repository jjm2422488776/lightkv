#pragma once

#include <cstdint>

namespace lightkv {

class ExpireManager {
public:
    static std::int64_t now_ms();

    static std::int64_t calc_expire_at(std::int64_t ttl_ms);

    static bool is_expired(bool has_expire, std::int64_t expire_at_ms, std::int64_t now_ms);

    // return:
    // -2 => key not found / already expired
    // -1 => key exists but has no expire
    // >=0 => remaining ttl in ms
    static std::int64_t ttl_ms(bool has_expire, std::int64_t expire_at_ms, std::int64_t now_ms);
};

}  // namespace lightkv