#include <exception>
#include <thread>

#include "common/config.h"
#include "common/logger.h"
#include "core/kv_store.h"
#include "net/server.h"

int main() {
    using namespace lightkv;

    try {
        Config config = ConfigManager::default_config();

        Logger::set_level(config.enable_log_debug ? LogLevel::DEBUG : LogLevel::INFO);
        Logger::info("Starting " + config.app_name + " stage-5 server...");

        const std::size_t worker_count =
            std::max<std::size_t>(4, std::thread::hardware_concurrency());

        Logger::info("Shard count => " + std::to_string(config.shard_count));
        Logger::info("LRU capacity => " + std::to_string(config.lru_capacity));
        Logger::info("Worker threads => " + std::to_string(worker_count));

        KVStore kv(config.shard_count, config.lru_capacity);
        Server server(kv, config, worker_count);

        server.run();
    } catch (const std::exception& ex) {
        lightkv::Logger::error(std::string("Fatal error: ") + ex.what());
        return 1;
    } catch (...) {
        lightkv::Logger::error("Fatal error: unknown exception");
        return 1;
    }

    return 0;
}