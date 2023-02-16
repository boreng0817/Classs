#!/bin/bash
# cmake -S . -B debug
# cd debug
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cd ..
# cmake --build debug 
cmake -S . -B build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
cmake --build build
