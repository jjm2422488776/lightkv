#include "common/config.h"

namespace lightkv {

Config ConfigManager::default_config() {
    return Config{};
}

}  // namespace lightkv