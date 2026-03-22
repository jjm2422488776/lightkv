# lightkv

A lightweight in-memory key-value storage system written in C++17.

## Stage 5 Features

- In-memory key-value store
- Basic CRUD operations
- TTL expiration support
- Lazy expiration deletion
- Sharded storage
- Thread-safe concurrent access
- Global LRU hot cache
- Thread pool
- TCP server
- Simple text protocol
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
│   ├── concurrency/
│   │   ├── thread_pool.h
│   │   └── thread_pool.cpp
│   └── net/
│       ├── protocol.h
│       ├── protocol.cpp
│       ├── server.h
│       └── server.cpp
└── scripts/
    └── build.sh