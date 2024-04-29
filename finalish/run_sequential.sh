#!/bin/bash

# Compile the C++ code
g++ -std=c++11 -o nq_seq sequential.cpp

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful"
    # Run the compiled program and measure execution time
    time ./nq_seq 4
else
    echo "Compilation failed"
fi
