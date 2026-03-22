# lightkv

A lightweight in-memory key-value storage system written in C++17.

## Stage 4 Features

- In-memory key-value store
- Basic CRUD operations
- TTL expiration support
- Lazy expiration deletion
- Sharded storage
- Thread-safe concurrent access
- Global LRU hot cache
- Simple logger
- Basic config structure
- CMake-based build

## Project Structure

```txt
lightkv/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── common/
│   │   ├── logger.h
│   │   ├── logger.cpp
│   │   ├── config.h
│   │   └── config.cpp
│   ├── core/
│   │   ├── entry.h
│   │   ├── expire_manager.h
│   │   ├── expire_manager.cpp
│   │   ├── shard.h
│   │   ├── shard.cpp
│   │   ├── lru_cache.h
│   │   ├── lru_cache.cpp
│   │   ├── kv_store.h
│   │   └── kv_store.cpp
└── scripts/
    └── build.sh
## Stage 4 Design

- Main storage uses sharded `unordered_map`.
- Hot data is accelerated by a global LRU cache.
- Read path:
  - check cache first
  - fallback to main store on miss
  - refill cache only for keys without TTL
- TTL is validated in the main store.
- Keys with TTL are never cached, which avoids stale cache hits after expiration.
- On TTL updates or deletes, cache entries are invalidated.