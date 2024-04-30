#!/bin/bash
# Compile the sequential program
g++ -std=c++11 -o sequential sequential.cpp
echo "SEQUENTIAL RUN"
# Run the sequential program for N from 1 to 20
for N in {1..15}; do
  echo "Running N = $N"
  ./sequential $N
done


# Compile the pthreads program
g++ -std=c++11 -o parallel_pthreads parallel_tasks.cpp -lpthread
echo "PARALLEL RUN PARALLEL TASKS"
# Run the pthreads program for N from 1 to 20 with 1 to 8 threads
for N in {1..15}; do
  for threads in {1..8}; do
    echo "Running N = $N with $threads threads"
    ./parallel_pthreads $N $threads
  done
done

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


# Compile the MPI program
mpicxx -std=c++11 -o mpi_nqueens distributed_tasks.cpp
echo "PARALLEL RUN DISTRIBUTED CORES"
# Run the MPI program for N from 1 to 20 with 1 to 4 cores
for N in {1..15}; do
  for cores in {1..4}; do
    echo "Running MPI N = $N with $cores cores"
    mpirun -np $cores ./mpi_nqueens $N
  done
done
