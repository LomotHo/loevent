#!/bin/bash
cd build
cmake ..
make
./client 127.0.0.1 3005