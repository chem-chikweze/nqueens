#!/bin/bash

# Compile the sequential program
g++ -std=c++11 -o sequential sequential.cpp
echo "SEQUENTIAL RUN"
# Run the sequential program for N from 1 to 20
for N in {1..15}; do
  echo "Running N = $N"
  ./sequential $N
done
