#pragma once

#include <cstddef>
#include <string>

namespace lightkv {

struct Config {
    std::string app_name = "lightkv";
    std::string host = "0.0.0.0";
    int port = 7777;
    std::size_t shard_count = 16;
    bool enable_log_debug = false;
};

class ConfigManager {
public:
    static Config default_config();
};

}  // namespace lightkv