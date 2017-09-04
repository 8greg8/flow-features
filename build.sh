#!/bin/bash

installDir="/home/gregork/local"

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$installDir .. &&

cd .. &&

make -j $nproc &&
make install
