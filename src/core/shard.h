#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "core/entry.h"

namespace lightkv {

class Shard {
public:
    Shard() = default;
    ~Shard() = default;

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const std::string& value, std::int64_t ttl_ms);

    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);

    bool expire(const std::string& key, std::int64_t ttl_ms);
    std::int64_t ttl(const std::string& key);

    std::size_t size() const;
    bool empty() const;
    void clear();

private:
    bool is_expired_unsafe(const Entry& entry, std::int64_t now_ms) const;
    bool erase_if_expired_with_unique_lock(const std::string& key);

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, Entry> store_;
};

}  // namespace lightkv