#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "core/entry.h"

namespace lightkv {

class KVStore {
public:
    KVStore() = default;
    ~KVStore() = default;

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
    bool erase_if_expired(const std::string& key);
    bool erase_if_expired(std::unordered_map<std::string, Entry>::iterator it);

private:
    std::unordered_map<std::string, Entry> store_;
};

}  // namespace lightkv