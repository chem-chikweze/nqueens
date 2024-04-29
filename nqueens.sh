#!/bin/bash

# Compile the C++ code
g++ -std=c++11 -pthread -o nqueens nq.cpp

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

# Set the default board size if no argument is provided
board_size=4

# Check if an argument is provided
if [ $# -eq 1 ]; then
    board_size=$1
fi

# Run the program with the provided board size
./nqueens $board_size

# Clean up
rm nqueens