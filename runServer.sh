#!/bin/bash
cd build
cmake ..
make
./server 3005
