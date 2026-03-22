#include <chrono>
#include <thread>
#include <vector>

#include "common/config.h"
#include "common/logger.h"
#include "core/kv_store.h"

int main() {
    using namespace lightkv;

    Config config = ConfigManager::default_config();

    Logger::set_level(config.enable_log_debug ? LogLevel::DEBUG : LogLevel::INFO);
    Logger::info("Starting " + config.app_name + " stage-4 demo...");
    Logger::info("Configured shard count => " + std::to_string(config.shard_count));
    Logger::info("Configured lru capacity => " + std::to_string(config.lru_capacity));

    KVStore kv(config.shard_count, config.lru_capacity);

    Logger::info("Basic set/get demo...");
    kv.set("name", "lightkv");
    kv.set("lang", "cpp");

    auto name1 = kv.get("name");
    if (name1.has_value()) {
        Logger::info("First get key=name => " + name1.value());
    }

    auto name2 = kv.get("name");
    if (name2.has_value()) {
        Logger::info("Second get key=name => " + name2.value());
    }

    Logger::info("Cache size after hot reads => " + std::to_string(kv.cache_size()));

    Logger::info("TTL consistency demo...");
    kv.set("session", "token_abc", 1000);
    Logger::info("TTL key=session => " + std::to_string(kv.ttl("session")) + " ms");

    auto s1 = kv.get("session");
    if (s1.has_value()) {
        Logger::info("Get key=session before expire => " + s1.value());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    auto s2 = kv.get("session");
    if (!s2.has_value()) {
        Logger::warn("Get key=session after expiration => expired or not found");
    }

    Logger::info("TTL key=session after expiration => " + std::to_string(kv.ttl("session")));
    Logger::info("Cache size now => " + std::to_string(kv.cache_size()));

    Logger::info("Concurrent write demo...");
    constexpr int thread_count = 4;
    constexpr int per_thread_ops = 500;

    std::vector<std::thread> writers;
    writers.reserve(thread_count);

    for (int t = 0; t < thread_count; ++t) {
        writers.emplace_back([&kv, t]() {
            for (int i = 0; i < per_thread_ops; ++i) {
                kv.set("user:" + std::to_string(t) + ":" + std::to_string(i),
                       "value_" + std::to_string(i));
            }
        });
    }

    for (auto& th : writers) {
        th.join();
    }

    Logger::info("Concurrent writes done.");
    Logger::info("Current kv size => " + std::to_string(kv.size()));
    Logger::info("Current cache size => " + std::to_string(kv.cache_size()));

    auto sample = kv.get("user:1:42");
    if (sample.has_value()) {
        Logger::info("Sample read => key=user:1:42, value=" + sample.value());
    }

    Logger::info("lightkv stage-4 demo finished.");
    return 0;
}