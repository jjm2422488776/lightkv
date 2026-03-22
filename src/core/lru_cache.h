#pragma once

#include <cstddef>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace lightkv {

class LRUCache {
public:
    explicit LRUCache(std::size_t capacity);
    ~LRUCache() = default;

    std::optional<std::string> get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    bool erase(const std::string& key);

    bool exists(const std::string& key);
    void clear();

    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;

private:
    using ListNode = std::pair<std::string, std::string>;
    using ListType = std::list<ListNode>;
    using ListIt = ListType::iterator;

private:
    void touch(ListIt it);

private:
    std::size_t capacity_;
    mutable std::mutex mutex_;
    ListType items_;
    std::unordered_map<std::string, ListIt> index_;
};

}  // namespace lightkv