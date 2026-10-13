#!/usr/bin/env bash

rm -rf build

clear

cmake -S . -B build -DENUMLITE_BUILD_TESTS=ON

cmake --build build

ctest --test-dir build --output-on-failure -V
