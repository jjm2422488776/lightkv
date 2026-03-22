#include "core/lru_cache.h"

#include <stdexcept>

namespace lightkv {

LRUCache::LRUCache(std::size_t capacity) : capacity_(capacity) {
    if (capacity_ == 0) {
        throw std::invalid_argument("lru cache capacity must be greater than 0");
    }
}

std::optional<std::string> LRUCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = index_.find(key);
    if (it == index_.end()) {
        return std::nullopt;
    }

    touch(it->second);
    return it->second->second;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = index_.find(key);
    if (it != index_.end()) {
        it->second->second = value;
        touch(it->second);
        return;
    }

    items_.emplace_front(key, value);
    index_[key] = items_.begin();

    if (items_.size() > capacity_) {
        auto last = std::prev(items_.end());
        index_.erase(last->first);
        items_.pop_back();
    }
}

bool LRUCache::erase(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = index_.find(key);
    if (it == index_.end()) {
        return false;
    }

    items_.erase(it->second);
    index_.erase(it);
    return true;
}

bool LRUCache::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    return index_.find(key) != index_.end();
}

void LRUCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    items_.clear();
    index_.clear();
}

std::size_t LRUCache::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return index_.size();
}

std::size_t LRUCache::capacity() const {
    return capacity_;
}

bool LRUCache::empty() const {
    return size() == 0;
}

void LRUCache::touch(ListIt it) {
    if (it == items_.begin()) {
        return;
    }
    items_.splice(items_.begin(), items_, it);
}

}  // namespace lightkv