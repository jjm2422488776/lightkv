#include "core/kv_store.h"

#include <stdexcept>

namespace lightkv {

KVStore::KVStore(std::size_t shard_count, std::size_t lru_capacity)
    : cache_(lru_capacity) {
    if (shard_count == 0) {
        throw std::invalid_argument("shard_count must be greater than 0");
    }

    shards_.reserve(shard_count);
    for (std::size_t i = 0; i < shard_count; ++i) {
        shards_.push_back(std::make_unique<Shard>());
    }
}

void KVStore::set(const std::string& key, const std::string& value) {
    shard_for(key).set(key, value);
    cache_.put(key, value);
}

void KVStore::set(const std::string& key, const std::string& value, std::int64_t ttl_ms) {
    shard_for(key).set(key, value, ttl_ms);

    if (ttl_ms <= 0) {
        cache_.erase(key);
        return;
    }

    // 带 TTL 的 key 不进入 cache，避免过期后 cache 脏命中
    cache_.erase(key);
}

std::optional<std::string> KVStore::get(const std::string& key) {
    auto cached = cache_.get(key);
    if (cached.has_value()) {
        return cached;
    }

    auto result = shard_for(key).get_with_meta(key);
    if (!result.found) {
        cache_.erase(key);
        return std::nullopt;
    }

    // 只缓存无 TTL 的 key
    if (result.cacheable) {
        cache_.put(key, result.value);
    } else {
        cache_.erase(key);
    }

    return result.value;
}

bool KVStore::del(const std::string& key) {
    const bool removed = shard_for(key).del(key);
    cache_.erase(key);
    return removed;
}

bool KVStore::exists(const std::string& key) {
    const bool ok = shard_for(key).exists(key);
    if (!ok) {
        cache_.erase(key);
    }
    return ok;
}

bool KVStore::expire(const std::string& key, std::int64_t ttl_ms) {
    const bool ok = shard_for(key).expire(key, ttl_ms);
    cache_.erase(key);
    return ok;
}

std::int64_t KVStore::ttl(const std::string& key) {
    const auto ttl_ms = shard_for(key).ttl(key);
    if (ttl_ms == -2) {
        cache_.erase(key);
    }
    return ttl_ms;
}

std::size_t KVStore::size() const {
    std::size_t total = 0;
    for (const auto& shard : shards_) {
        total += shard->size();
    }
    return total;
}

bool KVStore::empty() const {
    return size() == 0;
}

void KVStore::clear() {
    for (auto& shard : shards_) {
        shard->clear();
    }
    cache_.clear();
}

std::size_t KVStore::shard_count() const {
    return shards_.size();
}

std::size_t KVStore::cache_size() const {
    return cache_.size();
}

std::size_t KVStore::cache_capacity() const {
    return cache_.capacity();
}

std::size_t KVStore::shard_index_for(const std::string& key) const {
    return hasher_(key) % shards_.size();
}

Shard& KVStore::shard_for(const std::string& key) {
    return *shards_[shard_index_for(key)];
}

const Shard& KVStore::shard_for(const std::string& key) const {
    return *shards_[shard_index_for(key)];
}

}  // namespace lightkv