# lightkv

A lightweight in-memory key-value storage system written in C++17.

## Stage 2 Features

- In-memory key-value store
- Basic CRUD operations
- TTL expiration support
- Lazy expiration deletion
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
│   │   ├── kv_store.h
│   │   ├── kv_store.cpp
│   │   ├── expire_manager.h
│   │   └── expire_manager.cpp
└── scripts/
    └── build.sh