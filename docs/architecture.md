# LightKV Architecture

## 1. Overview

LightKV is a lightweight in-memory key-value storage system implemented in C++17.

It is designed as a progressive engineering project for campus recruitment, focusing on:

- clear modular design
- thread-safe concurrent access
- TTL expiration support
- LRU hot cache
- TCP service capability
- benchmark and testability

---

## 2. High-Level Architecture

```text
Client
  |
  v
TCP Server
  |
  v
Text Protocol Parser
  |
  v
KVStore
  |----------------------|
  |                      |
  v                      v
LRU Cache            Sharded Storage
                         |
                         v
                    unordered_map<Entry>