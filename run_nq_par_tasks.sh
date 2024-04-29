#!/bin/bash

# Compile the C++ code with C++11 standard
g++ -std=c++11 -pthread -o nq_par_tasks nq_par_tasks.cpp

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful"
    # Run the compiled program
    ./nq_par_tasks
else
    echo "Compilation failed"
fi
