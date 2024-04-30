#!/bin/bash

g++ -std=c++11 -o parallel_pthreads parallel_tasks.cpp -lpthread
echo "PARALLEL RUN PARALLEL TASKS"
# Run the pthreads program for N from 1 to 20 with 1 to 8 threads
for N in {1..15}; do
  for threads in {1..8}; do
    echo "Running N = $N with $threads threads"
    ./parallel_pthreads $N $threads
  done
done
