#include <cassert>
#include <iostream>
#include <string>

#include "core/lru_cache.h"

using namespace lightkv;

int main() {
    {
        LRUCache cache(2);

        assert(cache.empty());
        assert(cache.capacity() == 2);

        cache.put("a", "1");
        cache.put("b", "2");

        assert(cache.size() == 2);
        assert(cache.get("a").has_value());
        assert(cache.get("a").value() == "1");

        cache.put("c", "3");
        assert(cache.size() == 2);

        // 因为访问过 a，所以 b 应该被淘汰
        assert(!cache.get("b").has_value());
        assert(cache.get("a").has_value());
        assert(cache.get("c").has_value());
    }

    {
        LRUCache cache(2);

        cache.put("x", "1");
        cache.put("y", "2");
        cache.put("x", "100");

        assert(cache.get("x").has_value());
        assert(cache.get("x").value() == "100");

        cache.put("z", "3");

        // x 最近更新过，因此 y 淘汰
        assert(!cache.get("y").has_value());
        assert(cache.get("x").has_value());
        assert(cache.get("z").has_value());
    }

    {
        LRUCache cache(2);

        cache.put("k1", "v1");
        cache.put("k2", "v2");

        assert(cache.erase("k1") == true);
        assert(cache.erase("k1") == false);
        assert(!cache.get("k1").has_value());

        cache.clear();
        assert(cache.empty());
        assert(cache.size() == 0);
    }

    std::cout << "test_lru passed\n";
    return 0;
}