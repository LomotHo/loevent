#!/bin/bash
cd build
cmake ..
make
# ./bench_cli <ip> <port> <connection_num> <blockSize> <threadNum>
./bench_cli 127.0.0.1 3005 1024 1024 2
