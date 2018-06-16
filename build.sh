#!/bin/bash

installDir="/home/gregork/local"

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$installDir .. &&

make -j $nproc &&
make install
