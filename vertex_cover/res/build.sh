#!/usr/bin/env bash

mkdir cmake-build
cd cmake-build
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..

cmake --build cmake-build --target install -- -j 4
cp cmake-build/vertex_cover/res/config_example.json cmake-build/vertex_cover/res/config.json