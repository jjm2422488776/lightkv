#include "core/kv_store.h"

#include <stdexcept>

namespace lightkv {

KVStore::KVStore(std::size_t shard_count) {
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
}

void KVStore::set(const std::string& key, const std::string& value, std::int64_t ttl_ms) {
    shard_for(key).set(key, value, ttl_ms);
}

std::optional<std::string> KVStore::get(const std::string& key) {
    return shard_for(key).get(key);
}

bool KVStore::del(const std::string& key) {
    return shard_for(key).del(key);
}

bool KVStore::exists(const std::string& key) {
    return shard_for(key).exists(key);
}

bool KVStore::expire(const std::string& key, std::int64_t ttl_ms) {
    return shard_for(key).expire(key, ttl_ms);
}

std::int64_t KVStore::ttl(const std::string& key) {
    return shard_for(key).ttl(key);
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
}

std::size_t KVStore::shard_count() const {
    return shards_.size();
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