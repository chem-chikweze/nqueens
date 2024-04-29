#!/bin/bash

# Define the source code file and the output executable name
SOURCE="nq.cpp"
OUTPUT="n_queen"

# Compile the C++ code into an executable
g++ -std=c++14 -pthread -o $OUTPUT $SOURCE

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    ./$OUTPUT
else
    echo "Compilation failed."
fi
