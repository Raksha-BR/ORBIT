#!/usr/bin/env bash

set -e

echo "========================================"
echo "        ORBIT Build & Test"
echo "========================================"

echo
echo "[1/3] Configuring..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

echo
echo "[2/3] Building..."
cmake --build build

echo
echo "[3/3] Running tests..."
ctest --test-dir build --output-on-failure

echo
echo "========================================"
echo "        ORBIT BUILD SUCCESS"
echo "========================================"
