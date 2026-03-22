#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "core/kv_store.h"

using namespace lightkv;

int main() {
    {
        KVStore kv(4, 16);

        kv.set("a", "1", 100);
        assert(kv.exists("a") == true);

        auto ttl1 = kv.ttl("a");
        assert(ttl1 >= 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        assert(!kv.get("a").has_value());
        assert(kv.exists("a") == false);
        assert(kv.ttl("a") == -2);
    }

    {
        KVStore kv(4, 16);

        kv.set("b", "2");
        assert(kv.ttl("b") == -1);

        assert(kv.expire("b", 200) == true);
        auto ttl2 = kv.ttl("b");
        assert(ttl2 >= 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        assert(!kv.get("b").has_value());
        assert(kv.ttl("b") == -2);
    }

    {
        KVStore kv(4, 16);

        kv.set("c", "3", 1000);
        assert(kv.ttl("c") >= 0);

        kv.set("c", "33");
        assert(kv.get("c").has_value());
        assert(kv.get("c").value() == "33");
        assert(kv.ttl("c") == -1);
    }

    {
        KVStore kv(4, 16);

        kv.set("d", "4");
        assert(kv.expire("d", 0) == true);
        assert(kv.exists("d") == false);
        assert(kv.ttl("d") == -2);
    }

    std::cout << "test_expire passed\n";
    return 0;
}