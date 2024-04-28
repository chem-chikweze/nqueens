#!/bin/bash

# Define the source code file and the output binary name
SOURCE="nq_parallel.cpp"
OUTPUT="nqueens2"

# Compile the C++ program
g++ -std=c++11 -o $OUTPUT $SOURCE -lpthread

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful."
    # Run the compiled program
    ./$OUTPUT
else
    echo "Compilation failed."
fi
