#!/bin/bash
cd build
cmake ..
make
./echo_server 3005
