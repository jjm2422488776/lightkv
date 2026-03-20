#include <chrono>
#include <iostream>
#include <thread>

#include "common/config.h"
#include "common/logger.h"
#include "core/kv_store.h"

int main() {
    using namespace lightkv;

    Config config = ConfigManager::default_config();

    Logger::set_level(config.enable_log_debug ? LogLevel::DEBUG : LogLevel::INFO);
    Logger::info("Starting " + config.app_name + " stage-2 demo...");

    KVStore kv;

    Logger::info("Set key=name, value=lightkv");
    kv.set("name", "lightkv");

    Logger::info("Set key=session, value=token_123, ttl=1500ms");
    kv.set("session", "token_123", 1500);

    auto name = kv.get("name");
    if (name.has_value()) {
        Logger::info("Get key=name success, value=" + name.value());
    }

    auto session = kv.get("session");
    if (session.has_value()) {
        Logger::info("Get key=session success, value=" + session.value());
    }

    Logger::info("TTL key=name => " + std::to_string(kv.ttl("name")));
    Logger::info("TTL key=session => " + std::to_string(kv.ttl("session")) + " ms");

    Logger::info("Sleep 1700ms to wait for key=session expire...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1700));

    auto session_after = kv.get("session");
    if (!session_after.has_value()) {
        Logger::warn("Get key=session after sleep failed: expired or not found");
    }

    Logger::info("TTL key=session after sleep => " + std::to_string(kv.ttl("session")));
    Logger::info(std::string("Exists key=session after sleep => ") + (kv.exists("session") ? "true" : "false"));

    Logger::info("Set key=user, value=fusu");
    kv.set("user", "fusu");

    Logger::info("Expire key=user, ttl=1000ms");
    const bool expire_ok = kv.expire("user", 1000);
    Logger::info(std::string("Expire result => ") + (expire_ok ? "true" : "false"));
    Logger::info("TTL key=user => " + std::to_string(kv.ttl("user")) + " ms");

    Logger::info("Final kv size => " + std::to_string(kv.size()));
    Logger::info("lightkv stage-2 demo finished.");

    return 0;
}