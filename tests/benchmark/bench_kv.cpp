#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "core/kv_store.h"

using namespace lightkv;

namespace {

void bench_single_thread_write(std::size_t n) {
    KVStore kv(16, 1024);

    auto start = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < n; ++i) {
        kv.set("key_" + std::to_string(i), "value_" + std::to_string(i));
    }
    auto end = std::chrono::steady_clock::now();

    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    const double qps = ms == 0 ? 0.0 : (static_cast<double>(n) * 1000.0 / ms);

    std::cout << "[bench] single_thread_write\n";
    std::cout << "  ops=" << n << " time_ms=" << ms
              << " qps=" << std::fixed << std::setprecision(2) << qps << "\n";
}

void bench_single_thread_read(std::size_t n) {
    KVStore kv(16, 1024);

    for (std::size_t i = 0; i < n; ++i) {
        kv.set("key_" + std::to_string(i), "value_" + std::to_string(i));
    }

    auto start = std::chrono::steady_clock::now();
    std::size_t hits = 0;
    for (std::size_t i = 0; i < n; ++i) {
        auto v = kv.get("key_" + std::to_string(i));
        if (v.has_value()) {
            ++hits;
        }
    }
    auto end = std::chrono::steady_clock::now();

    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    const double qps = ms == 0 ? 0.0 : (static_cast<double>(n) * 1000.0 / ms);

    std::cout << "[bench] single_thread_read\n";
    std::cout << "  ops=" << n << " hits=" << hits << " time_ms=" << ms
              << " qps=" << std::fixed << std::setprecision(2) << qps << "\n";
}

void bench_multi_thread_write(std::size_t threads, std::size_t per_thread_ops) {
    KVStore kv(16, 1024);

    auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;
    workers.reserve(threads);

    for (std::size_t t = 0; t < threads; ++t) {
        workers.emplace_back([&kv, t, per_thread_ops]() {
            for (std::size_t i = 0; i < per_thread_ops; ++i) {
                kv.set("t" + std::to_string(t) + "_key_" + std::to_string(i),
                       "value_" + std::to_string(i));
            }
        });
    }

    for (auto& th : workers) {
        th.join();
    }

    auto end = std::chrono::steady_clock::now();

    const std::size_t total_ops = threads * per_thread_ops;
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    const double qps = ms == 0 ? 0.0 : (static_cast<double>(total_ops) * 1000.0 / ms);

    std::cout << "[bench] multi_thread_write\n";
    std::cout << "  threads=" << threads
              << " total_ops=" << total_ops
              << " time_ms=" << ms
              << " qps=" << std::fixed << std::setprecision(2) << qps << "\n";
}

void bench_multi_thread_read(std::size_t threads, std::size_t per_thread_ops) {
    KVStore kv(16, 1024);

    const std::size_t total_ops = threads * per_thread_ops;
    for (std::size_t i = 0; i < total_ops; ++i) {
        kv.set("key_" + std::to_string(i), "value_" + std::to_string(i));
    }

    auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;
    workers.reserve(threads);

    for (std::size_t t = 0; t < threads; ++t) {
        workers.emplace_back([&kv, t, per_thread_ops]() {
            for (std::size_t i = 0; i < per_thread_ops; ++i) {
                const std::size_t idx = t * per_thread_ops + i;
                auto v = kv.get("key_" + std::to_string(idx));
                (void)v;
            }
        });
    }

    for (auto& th : workers) {
        th.join();
    }

    auto end = std::chrono::steady_clock::now();

    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    const double qps = ms == 0 ? 0.0 : (static_cast<double>(total_ops) * 1000.0 / ms);

    std::cout << "[bench] multi_thread_read\n";
    std::cout << "  threads=" << threads
              << " total_ops=" << total_ops
              << " time_ms=" << ms
              << " qps=" << std::fixed << std::setprecision(2) << qps << "\n";
}

}  // namespace

int main() {
    constexpr std::size_t n = 100000;
    constexpr std::size_t threads = 4;
    constexpr std::size_t per_thread_ops = 50000;

    std::cout << "lightkv benchmark start\n";

    bench_single_thread_write(n);
    bench_single_thread_read(n);
    bench_multi_thread_write(threads, per_thread_ops);
    bench_multi_thread_read(threads, per_thread_ops);

    std::cout << "lightkv benchmark done\n";
    return 0;
}