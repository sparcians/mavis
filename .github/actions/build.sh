#!/bin/bash

cd ${GITHUB_WORKSPACE}
mkdir $MAVIS_BUILD_TYPE
cd $MAVIS_BUILD_TYPE

CXX_COMPILER=${COMPILER/clang/clang++}
CXX_COMPILER=${CXX_COMPILER/gcc/g++}

cmake .. -DCMAKE_C_COMPILER=$COMPILER -DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_BUILD_TYPE=$MAVIS_BUILD_TYPE

if [ $? -ne 0 ]; then
    echo "ERROR: CMake for mavis failed"
    exit 1
fi

make -j$(nproc --all)

if [ $? -ne 0 ]; then
    echo "ERROR: build of mavis failed"
    exit 1
fi
