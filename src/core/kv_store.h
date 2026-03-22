#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core/shard.h"

namespace lightkv {

class KVStore {
public:
    explicit KVStore(std::size_t shard_count = 16);
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

    std::size_t shard_count() const;

private:
    std::size_t shard_index_for(const std::string& key) const;
    Shard& shard_for(const std::string& key);
    const Shard& shard_for(const std::string& key) const;

private:
    std::vector<std::unique_ptr<Shard>> shards_;
    std::hash<std::string> hasher_;
};

}  // namespace lightkv