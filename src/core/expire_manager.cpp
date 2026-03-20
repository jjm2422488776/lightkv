#include "core/expire_manager.h"

#include <chrono>

namespace lightkv {

std::int64_t ExpireManager::now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
               system_clock::now().time_since_epoch())
        .count();
}

std::int64_t ExpireManager::calc_expire_at(std::int64_t ttl_ms) {
    return now_ms() + ttl_ms;
}

bool ExpireManager::is_expired(bool has_expire, std::int64_t expire_at_ms, std::int64_t now_ms_value) {
    if (!has_expire) {
        return false;
    }
    return now_ms_value >= expire_at_ms;
}

std::int64_t ExpireManager::ttl_ms(bool has_expire, std::int64_t expire_at_ms, std::int64_t now_ms_value) {
    if (!has_expire) {
        return -1;
    }

    const auto remain = expire_at_ms - now_ms_value;
    if (remain < 0) {
        return 0;
    }
    return remain;
}

}  // namespace lightkv