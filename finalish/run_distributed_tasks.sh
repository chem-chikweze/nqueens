
#!/bin/bash

# Compiling the MPI program with C++11 standard
mpicxx -std=c++11 -o nqueens_mpi distributed_tasks.cpp

# Running the MPI program with 4 processes and board size of 15
mpirun -np 4 ./nqueens_mpi 15
