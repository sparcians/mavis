#!/bin/bash

cd ${GITHUB_WORKSPACE}
cd $MAVIS_BUILD_TYPE
make -j$(nproc --all) regress

if [ $? -ne 0 ]; then
    echo "ERROR: regress of mavis failed"
    exit 1
fi
