#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

echo "[lightkv] project root: ${PROJECT_ROOT}"
echo "[lightkv] build dir: ${BUILD_DIR}"

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake ..
cmake --build . -j

echo "[lightkv] build success"
echo "[lightkv] run with: ./lightkv"