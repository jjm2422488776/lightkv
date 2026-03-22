#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

if [ ! -d "${BUILD_DIR}" ]; then
  echo "[lightkv] build directory not found, please build first"
  exit 1
fi

cd "${BUILD_DIR}"

if [ ! -f "./bench_kv" ]; then
  echo "[lightkv] bench_kv not found, please rebuild with benchmark target"
  exit 1
fi

echo "[lightkv] running benchmark..."
./bench_kv