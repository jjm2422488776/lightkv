#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "core/kv_store.h"

using namespace lightkv;

int main() {
    {
        KVStore kv(8, 32);

        kv.set("name", "lightkv");
        assert(kv.exists("name") == true);

        auto value = kv.get("name");
        assert(value.has_value());
        assert(value.value() == "lightkv");

        assert(kv.size() == 1);
        assert(kv.del("name") == true);
        assert(kv.del("name") == false);
        assert(kv.exists("name") == false);
    }

    {
        KVStore kv(8, 32);

        kv.set("a", "1");
        kv.set("b", "2");
        kv.set("c", "3");

        assert(kv.size() == 3);

        kv.clear();
        assert(kv.size() == 0);
        assert(kv.cache_size() == 0);
        assert(kv.empty());
    }

    {
        KVStore kv(8, 32);

        kv.set("normal", "v");
        auto v1 = kv.get("normal");
        assert(v1.has_value());
        assert(v1.value() == "v");
        assert(kv.cache_size() >= 1);
    }

    {
        KVStore kv(8, 32);

        const auto before = kv.cache_size();

        kv.set("ttl_key", "abc", 1000);
        auto v2 = kv.get("ttl_key");
        assert(v2.has_value());
        assert(v2.value() == "abc");

        // 修正版阶段四：带 TTL 的 key 不进入 cache
        assert(kv.cache_size() == before);
    }

    {
        KVStore kv(8, 32);

        kv.set("x", "1");
        assert(kv.get("x").has_value());

        assert(kv.expire("x", 100) == true);
        assert(kv.ttl("x") >= 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        assert(!kv.get("x").has_value());
        assert(kv.ttl("x") == -2);
    }

    std::cout << "test_kv_store passed\n";
    return 0;
}