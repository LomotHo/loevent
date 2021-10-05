#!/bin/bash
cd build
cmake ..
make
./pp_server 3005
