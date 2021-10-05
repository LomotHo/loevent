#!/bin/bash
cd build
cmake ..
make
./pp_client 127.0.0.1 3005