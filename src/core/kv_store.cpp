#include "core/kv_store.h"

#include "core/expire_manager.h"

namespace lightkv {

void KVStore::set(const std::string& key, const std::string& value) {
    const auto ts = ExpireManager::now_ms();

    auto it = store_.find(key);
    if (it == store_.end()) {
        store_.emplace(key, Entry{value, ts, ts, false, 0});
    } else {
        it->second.value = value;
        it->second.updated_at_ms = ts;
        it->second.has_expire = false;
        it->second.expire_at_ms = 0;
    }
}

void KVStore::set(const std::string& key, const std::string& value, std::int64_t ttl_ms) {
    if (ttl_ms <= 0) {
        del(key);
        return;
    }

    const auto ts = ExpireManager::now_ms();
    const auto expire_at = ExpireManager::calc_expire_at(ttl_ms);

    auto it = store_.find(key);
    if (it == store_.end()) {
        store_.emplace(key, Entry{value, ts, ts, true, expire_at});
    } else {
        it->second.value = value;
        it->second.updated_at_ms = ts;
        it->second.has_expire = true;
        it->second.expire_at_ms = expire_at;
    }
}

std::optional<std::string> KVStore::get(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return std::nullopt;
    }

    if (erase_if_expired(it)) {
        return std::nullopt;
    }

    return it->second.value;
}

bool KVStore::del(const std::string& key) {
    return store_.erase(key) > 0;
}

bool KVStore::exists(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return false;
    }

    if (erase_if_expired(it)) {
        return false;
    }

    return true;
}

bool KVStore::expire(const std::string& key, std::int64_t ttl_ms) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return false;
    }

    if (erase_if_expired(it)) {
        return false;
    }

    if (ttl_ms <= 0) {
        store_.erase(it);
        return true;
    }

    it->second.has_expire = true;
    it->second.expire_at_ms = ExpireManager::calc_expire_at(ttl_ms);
    it->second.updated_at_ms = ExpireManager::now_ms();

    return true;
}

std::int64_t KVStore::ttl(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return -2;
    }

    if (erase_if_expired(it)) {
        return -2;
    }

    const auto now = ExpireManager::now_ms();
    return ExpireManager::ttl_ms(it->second.has_expire, it->second.expire_at_ms, now);
}

std::size_t KVStore::size() const {
    return store_.size();
}

bool KVStore::empty() const {
    return store_.empty();
}

void KVStore::clear() {
    store_.clear();
}

bool KVStore::erase_if_expired(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return false;
    }
    return erase_if_expired(it);
}

bool KVStore::erase_if_expired(std::unordered_map<std::string, Entry>::iterator it) {
    const auto now = ExpireManager::now_ms();
    if (ExpireManager::is_expired(it->second.has_expire, it->second.expire_at_ms, now)) {
        store_.erase(it);
        return true;
    }
    return false;
}

}  // namespace lightkv