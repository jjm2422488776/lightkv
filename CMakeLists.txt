cmake_minimum_required(VERSION 3.16)

project(lightkv VERSION 0.6.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIGHTKV_BUILD_TESTS "Build tests" ON)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(LIGHTKV_CORE_SOURCES
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

set(LIGHTKV_APP_SOURCES
    src/main.cpp
)

add_executable(lightkv
    ${LIGHTKV_APP_SOURCES}
    ${LIGHTKV_CORE_SOURCES}
)

if (MSVC)
    target_compile_options(lightkv PRIVATE /W4)
else()
    target_compile_options(lightkv PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)
target_link_libraries(lightkv PRIVATE Threads::Threads)

if (LIGHTKV_BUILD_TESTS)
    enable_testing()

    add_executable(test_lru
        tests/unit/test_lru.cpp
        src/core/lru_cache.cpp
    )
    target_include_directories(test_lru PRIVATE ${PROJECT_SOURCE_DIR}/src)

    add_executable(test_expire
        tests/unit/test_expire.cpp
        src/common/config.cpp
        src/common/logger.cpp
        src/core/expire_manager.cpp
        src/core/shard.cpp
        src/core/lru_cache.cpp
        src/core/kv_store.cpp
    )
    target_include_directories(test_expire PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(test_expire PRIVATE Threads::Threads)

    add_executable(test_kv_store
        tests/unit/test_kv_store.cpp
        src/common/config.cpp
        src/common/logger.cpp
        src/core/expire_manager.cpp
        src/core/shard.cpp
        src/core/lru_cache.cpp
        src/core/kv_store.cpp
    )
    target_include_directories(test_kv_store PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(test_kv_store PRIVATE Threads::Threads)

    add_test(NAME test_lru COMMAND test_lru)
    add_test(NAME test_expire COMMAND test_expire)
    add_test(NAME test_kv_store COMMAND test_kv_store)
endif()

add_executable(bench_kv
    tests/benchmark/bench_kv.cpp
    src/common/config.cpp
    src/common/logger.cpp
    src/core/expire_manager.cpp
    src/core/shard.cpp
    src/core/lru_cache.cpp
    src/core/kv_store.cpp
)
target_include_directories(bench_kv PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(bench_kv PRIVATE Threads::Threads)