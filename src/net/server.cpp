#include "net/server.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

#include "common/logger.h"
#include "net/protocol.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace lightkv {

namespace {
constexpr int kBacklog = 128;
constexpr std::size_t kReadBufferSize = 1024;
}  // namespace

Server::Server(KVStore& kv, const Config& config, std::size_t worker_count)
    : kv_(kv),
      config_(config),
      thread_pool_(worker_count) {}

Server::~Server() {
    stop();
}

bool Server::start() {
    if (running_) {
        return true;
    }

    if (!setup_listen_socket()) {
        return false;
    }

    running_ = true;
    Logger::info("Server started on " + config_.host + ":" + std::to_string(config_.port));
    return true;
}

void Server::stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        if (listen_fd_ >= 0) {
            ::close(listen_fd_);
            listen_fd_ = -1;
        }
        Logger::info("Server stopped.");
    }
}

void Server::run() {
    if (!start()) {
        Logger::error("Failed to start server.");
        return;
    }
    accept_loop();
}

bool Server::setup_listen_socket() {
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        Logger::error("socket() failed: " + std::string(std::strerror(errno)));
        return false;
    }

    int opt = 1;
    if (::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::warn("setsockopt(SO_REUSEADDR) failed: " + std::string(std::strerror(errno)));
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<std::uint16_t>(config_.port));

    if (::inet_pton(AF_INET, config_.host.c_str(), &addr.sin_addr) <= 0) {
        Logger::error("inet_pton() failed for host: " + config_.host);
        ::close(listen_fd_);
        listen_fd_ = -1;
        return false;
    }

    if (::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        Logger::error("bind() failed: " + std::string(std::strerror(errno)));
        ::close(listen_fd_);
        listen_fd_ = -1;
        return false;
    }

    if (::listen(listen_fd_, kBacklog) < 0) {
        Logger::error("listen() failed: " + std::string(std::strerror(errno)));
        ::close(listen_fd_);
        listen_fd_ = -1;
        return false;
    }

    return true;
}

void Server::accept_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        const int client_fd = ::accept(
            listen_fd_,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len
        );

        if (client_fd < 0) {
            if (!running_) {
                break;
            }
            Logger::warn("accept() failed: " + std::string(std::strerror(errno)));
            continue;
        }

        char ip_buf[INET_ADDRSTRLEN] = {0};
        ::inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, sizeof(ip_buf));
        Logger::info("Accepted connection from " + std::string(ip_buf) + ":" +
                     std::to_string(ntohs(client_addr.sin_port)));

        thread_pool_.submit([this, client_fd]() {
            handle_client(client_fd);
        });
    }
}

void Server::handle_client(int client_fd) {
    const std::string welcome =
        "WELCOME TO LIGHTKV\n"
        "Commands: PING, SET, SETEX, GET, DEL, EXISTS, EXPIRE, TTL, SIZE, CLEAR, QUIT\n";

    if (!send_all(client_fd, welcome)) {
        ::close(client_fd);
        return;
    }

    while (running_) {
        std::string line;
        if (!recv_line(client_fd, line)) {
            break;
        }

        Logger::debug("Received command: " + line);
        const auto result = Protocol::handle_command(kv_, line);

        if (!send_all(client_fd, result.response)) {
            break;
        }

        if (result.should_close) {
            break;
        }
    }

    ::close(client_fd);
}

bool Server::send_all(int fd, const std::string& data) {
    std::size_t total_sent = 0;

    while (total_sent < data.size()) {
        const ssize_t n = ::send(
            fd,
            data.data() + total_sent,
            data.size() - total_sent,
            0
        );

        if (n <= 0) {
            return false;
        }

        total_sent += static_cast<std::size_t>(n);
    }

    return true;
}

bool Server::recv_line(int fd, std::string& out_line) {
    out_line.clear();
    char ch = '\0';

    while (true) {
        const ssize_t n = ::recv(fd, &ch, 1, 0);
        if (n <= 0) {
            return false;
        }

        if (ch == '\r') {
            continue;
        }

        if (ch == '\n') {
            return true;
        }

        out_line.push_back(ch);

        if (out_line.size() > 4096) {
            out_line = "QUIT";
            return true;
        }
    }
}

}  // namespace lightkv