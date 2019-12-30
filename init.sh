#!/usr/bin/env bash

mkdir -p ./build
rm -rf ./build/*
cd ./build
conan install .. --build=missing -s compiler.version=9.2
# conan install .. --build=missing
cmake ..
make -j8
cd ./bin
ln -s ../../tilesets ../../imgui.ini ../../game.bin ../../init.js .
cd ..
