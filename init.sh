#!/usr/bin/env bash

mkdir -p ./build
rm -rf ./build/*
cd ./build
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    conan install .. --build=missing -s compiler.version=9.2
elif [[ "$OSTYPE" == "darwin"* ]]; then
    conan install .. --build=missing -s compiler.version=11.0
fi
cmake ..
make -j8
cd ./bin
ln -s ../../data .
cd ..
