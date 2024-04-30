#!/bin/bash

# Compile the work-stealing program
g++ -std=c++11 -pthread -o work_stealing4 queue_work_stealing.cpp
echo "PARALLEL RUN WORK STEALING"
# Run the work-stealing program for N from 1 to 20 with 1 to 8 threads
for N in {1..15}; do
  for threads in {1..8}; do
    echo "Running N = $N with $threads threads"
    ./work_stealing4 $N $threads
  done
done
