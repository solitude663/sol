#!/bin/bash

if [ ! -d "./build" ]; then
    mkdir ./build
fi

pushd ./build > /dev/null

warning_flags="-Wall -Wno-unused-variable -Wno-write-strings -Wno-unused-function"

g++ $warning_flags -g -o ./sol ../src/main.cpp
popd > /dev/null

# ./build/sol
