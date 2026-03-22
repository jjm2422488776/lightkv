cmake_minimum_required(VERSION 3.16)

project(lightkv VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIGHTKV_BUILD_TESTS "Build tests" OFF)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(LIGHTKV_SOURCES
    src/main.cpp
    src/common/logger.cpp
    src/common/config.cpp
    src/core/expire_manager.cpp
    src/core/shard.cpp
    src/core/lru_cache.cpp
    src/core/kv_store.cpp
    src/concurrency/thread_pool.cpp
    src/net/protocol.cpp
    src/net/server.cpp
)

add_executable(lightkv ${LIGHTKV_SOURCES})

if (MSVC)
    target_compile_options(lightkv PRIVATE /W4)
else()
    target_compile_options(lightkv PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)
target_link_libraries(lightkv PRIVATE Threads::Threads)