#pragma once

#include <string>
#include <vector>

#include "core/kv_store.h"

namespace lightkv {

struct CommandResult {
    std::string response;
    bool should_close{false};
};

class Protocol {
public:
    static CommandResult handle_command(KVStore& kv, const std::string& line);

private:
    static std::vector<std::string> split_tokens(const std::string& line);
    static std::string to_upper(std::string s);
};

}  // namespace lightkv