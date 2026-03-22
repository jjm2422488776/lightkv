#include "net/protocol.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace lightkv {

std::vector<std::string> Protocol::split_tokens(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string Protocol::to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return s;
}

CommandResult Protocol::handle_command(KVStore& kv, const std::string& line) {
    const auto tokens = split_tokens(line);
    if (tokens.empty()) {
        return {"ERROR empty command\n", false};
    }

    const std::string cmd = to_upper(tokens[0]);

    if (cmd == "SET") {
        if (tokens.size() != 3) {
            return {"ERROR usage: SET <key> <value>\n", false};
        }
        kv.set(tokens[1], tokens[2]);
        return {"OK\n", false};
    }

    if (cmd == "SETEX") {
        if (tokens.size() != 4) {
            return {"ERROR usage: SETEX <key> <ttl_ms> <value>\n", false};
        }

        try {
            const auto ttl_ms = std::stoll(tokens[2]);
            kv.set(tokens[1], tokens[3], ttl_ms);
            return {"OK\n", false};
        } catch (...) {
            return {"ERROR invalid ttl_ms\n", false};
        }
    }

    if (cmd == "GET") {
        if (tokens.size() != 2) {
            return {"ERROR usage: GET <key>\n", false};
        }

        auto value = kv.get(tokens[1]);
        if (!value.has_value()) {
            return {"NOT_FOUND\n", false};
        }

        return {"VALUE " + value.value() + "\n", false};
    }

    if (cmd == "DEL") {
        if (tokens.size() != 2) {
            return {"ERROR usage: DEL <key>\n", false};
        }

        const bool ok = kv.del(tokens[1]);
        return {"(integer) " + std::to_string(ok ? 1 : 0) + "\n", false};
    }

    if (cmd == "EXISTS") {
        if (tokens.size() != 2) {
            return {"ERROR usage: EXISTS <key>\n", false};
        }

        const bool ok = kv.exists(tokens[1]);
        return {"(integer) " + std::to_string(ok ? 1 : 0) + "\n", false};
    }

    if (cmd == "EXPIRE") {
        if (tokens.size() != 3) {
            return {"ERROR usage: EXPIRE <key> <ttl_ms>\n", false};
        }

        try {
            const auto ttl_ms = std::stoll(tokens[2]);
            const bool ok = kv.expire(tokens[1], ttl_ms);
            return {"(integer) " + std::to_string(ok ? 1 : 0) + "\n", false};
        } catch (...) {
            return {"ERROR invalid ttl_ms\n", false};
        }
    }

    if (cmd == "TTL") {
        if (tokens.size() != 2) {
            return {"ERROR usage: TTL <key>\n", false};
        }

        const auto ttl_ms = kv.ttl(tokens[1]);
        return {"(integer) " + std::to_string(ttl_ms) + "\n", false};
    }

    if (cmd == "SIZE") {
        if (tokens.size() != 1) {
            return {"ERROR usage: SIZE\n", false};
        }

        return {"(integer) " + std::to_string(kv.size()) + "\n", false};
    }

    if (cmd == "CLEAR") {
        if (tokens.size() != 1) {
            return {"ERROR usage: CLEAR\n", false};
        }

        kv.clear();
        return {"OK\n", false};
    }

    if (cmd == "PING") {
        return {"PONG\n", false};
    }

    if (cmd == "QUIT") {
        return {"BYE\n", true};
    }

    return {"ERROR unknown command\n", false};
}

}  // namespace lightkv