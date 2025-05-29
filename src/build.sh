#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi

pushd ../build > /dev/null

warning_flags="-Wall -Wno-unused-variable -Wno-write-strings -Wno-unused-function -Wno-changes-meaning -Wno-unused-but-set-variable"
compile_flags="-O0 -g"
macro_flags="-D BUILD_DEBUG"

g++ $macro_flags $warning_flags $compile_flags -o ./sol ../src/main.cpp
popd > /dev/null

echo "t1=============================="
../build/sol ../data/t1.sol
echo
echo "t2=============================="
../build/sol ../data/t2.sol
echo
# echo "t3=============================="
# ../build/sol ../data/t3.sol
