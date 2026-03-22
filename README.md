# lightkv

A lightweight in-memory key-value storage system written in C++17.

## Stage 3 Features

- In-memory key-value store
- Basic CRUD operations
- TTL expiration support
- Lazy expiration deletion
- Sharded storage
- Thread-safe concurrent access
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
│   │   ├── kv_store.h
│   │   └── kv_store.cpp
└── scripts/
    └── build.sh