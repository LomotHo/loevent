#!/bin/bash
rm -rf build
mkdir build
cd build
cmake ..
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..
make
