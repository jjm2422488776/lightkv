#include "core/shard.h"

#include <mutex>
#include <shared_mutex>

#include "core/expire_manager.h"

namespace lightkv {

void Shard::set(const std::string& key, const std::string& value) {
    const auto ts = ExpireManager::now_ms();

    std::unique_lock<std::shared_mutex> lock(mutex_);

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

void Shard::set(const std::string& key, const std::string& value, std::int64_t ttl_ms) {
    if (ttl_ms <= 0) {
        del(key);
        return;
    }

    const auto ts = ExpireManager::now_ms();
    const auto expire_at = ExpireManager::calc_expire_at(ttl_ms);

    std::unique_lock<std::shared_mutex> lock(mutex_);

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

std::optional<std::string> Shard::get(const std::string& key) {
    {
        std::shared_lock<std::shared_mutex> read_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return std::nullopt;
        }

        const auto now = ExpireManager::now_ms();
        if (!is_expired_unsafe(it->second, now)) {
            return it->second.value;
        }
    }

    {
        std::unique_lock<std::shared_mutex> write_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return std::nullopt;
        }

        const auto now = ExpireManager::now_ms();
        if (is_expired_unsafe(it->second, now)) {
            store_.erase(it);
            return std::nullopt;
        }

        return it->second.value;
    }
}

bool Shard::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    return store_.erase(key) > 0;
}

bool Shard::exists(const std::string& key) {
    {
        std::shared_lock<std::shared_mutex> read_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return false;
        }

        const auto now = ExpireManager::now_ms();
        if (!is_expired_unsafe(it->second, now)) {
            return true;
        }
    }

    {
        std::unique_lock<std::shared_mutex> write_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return false;
        }

        const auto now = ExpireManager::now_ms();
        if (is_expired_unsafe(it->second, now)) {
            store_.erase(it);
            return false;
        }

        return true;
    }
}

bool Shard::expire(const std::string& key, std::int64_t ttl_ms) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = store_.find(key);
    if (it == store_.end()) {
        return false;
    }

    const auto now = ExpireManager::now_ms();
    if (is_expired_unsafe(it->second, now)) {
        store_.erase(it);
        return false;
    }

    if (ttl_ms <= 0) {
        store_.erase(it);
        return true;
    }

    it->second.has_expire = true;
    it->second.expire_at_ms = ExpireManager::calc_expire_at(ttl_ms);
    it->second.updated_at_ms = now;

    return true;
}

std::int64_t Shard::ttl(const std::string& key) {
    {
        std::shared_lock<std::shared_mutex> read_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return -2;
        }

        const auto now = ExpireManager::now_ms();
        if (!is_expired_unsafe(it->second, now)) {
            return ExpireManager::ttl_ms(
                it->second.has_expire,
                it->second.expire_at_ms,
                now
            );
        }
    }

    {
        std::unique_lock<std::shared_mutex> write_lock(mutex_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            return -2;
        }

        const auto now = ExpireManager::now_ms();
        if (is_expired_unsafe(it->second, now)) {
            store_.erase(it);
            return -2;
        }

        return ExpireManager::ttl_ms(
            it->second.has_expire,
            it->second.expire_at_ms,
            now
        );
    }
}

std::size_t Shard::size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return store_.size();
}

bool Shard::empty() const {
    return size() == 0;
}

void Shard::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    store_.clear();
}

bool Shard::is_expired_unsafe(const Entry& entry, std::int64_t now_ms) const {
    return ExpireManager::is_expired(entry.has_expire, entry.expire_at_ms, now_ms);
}

bool Shard::erase_if_expired_with_unique_lock(const std::string& key) {
    auto it = store_.find(key);
    if (it == store_.end()) {
        return false;
    }

    const auto now = ExpireManager::now_ms();
    if (is_expired_unsafe(it->second, now)) {
        store_.erase(it);
        return true;
    }

    return false;
}

}  // namespace lightkv