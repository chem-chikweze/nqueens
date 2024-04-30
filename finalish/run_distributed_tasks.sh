
#!/bin/bash


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
