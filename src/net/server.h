#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include "common/config.h"
#include "concurrency/thread_pool.h"
#include "core/kv_store.h"

namespace lightkv {

class Server {
public:
    Server(KVStore& kv, const Config& config, std::size_t worker_count);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    bool start();
    void stop();
    void run();

private:
    bool setup_listen_socket();
    void accept_loop();
    void handle_client(int client_fd);
    bool send_all(int fd, const std::string& data);
    bool recv_line(int fd, std::string& out_line);

private:
    KVStore& kv_;
    Config config_;
    ThreadPool thread_pool_;

    int listen_fd_{-1};
    std::atomic<bool> running_{false};
};

}  // namespace lightkv