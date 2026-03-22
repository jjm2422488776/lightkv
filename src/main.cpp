#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "common/config.h"
#include "common/logger.h"
#include "core/kv_store.h"

int main() {
    using namespace lightkv;

    Config config = ConfigManager::default_config();

    Logger::set_level(config.enable_log_debug ? LogLevel::DEBUG : LogLevel::INFO);
    Logger::info("Starting " + config.app_name + " stage-3 demo...");
    Logger::info("Configured shard count => " + std::to_string(config.shard_count));

    KVStore kv(config.shard_count);

    Logger::info("Basic operations demo...");
    kv.set("name", "lightkv");
    kv.set("session", "token_123", 1200);

    auto name = kv.get("name");
    if (name.has_value()) {
        Logger::info("Get key=name success, value=" + name.value());
    }

    Logger::info("TTL key=session => " + std::to_string(kv.ttl("session")) + " ms");

    std::this_thread::sleep_for(std::chrono::milliseconds(1300));

    auto session_after = kv.get("session");
    if (!session_after.has_value()) {
        Logger::warn("Get key=session failed after expiration");
    }

    Logger::info("Concurrent write demo...");

    constexpr int thread_count = 4;
    constexpr int per_thread_ops = 1000;

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

    Logger::info("Concurrent write finished.");
    Logger::info("Current shard count => " + std::to_string(kv.shard_count()));
    Logger::info("Current kv size => " + std::to_string(kv.size()));

    auto sample = kv.get("user:2:42");
    if (sample.has_value()) {
        Logger::info("Sample read success: key=user:2:42, value=" + sample.value());
    } else {
        Logger::warn("Sample read failed: key=user:2:42");
    }

    Logger::info("lightkv stage-3 demo finished.");
    return 0;
}