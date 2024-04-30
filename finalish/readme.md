# N-Queens Problem Solutions

This project explores various methods to solve the N-Queens problem, ranging from a sequential approach to advanced distributed and parallel computing techniques. Below are instructions on how to compile and run each implementation.

## Prerequisites

- GCC compiler with support for C++11 (for sequential and parallel implementations)
- Bash shell for executing shell scripts
- MPI environment (e.g., MPICH, OpenMPI) for distributed tasks
- POSIX Threads library for certain parallel implementations

## File Descriptions

- `sequential.cpp`: Solves the N-Queens problem using a sequential approach.
- `run_sequential.sh`: Script to compile and run the sequential solution.
- `parallel_tasks.cpp`: Parallel solution using pthreads.
- `run_parallel_tasks.sh`: Script to compile and run the parallel pthreads solution.
- `queue_work_stealing.cpp`: Parallel solution using a work-stealing queue and multiple threads.
- `run_queue_work_stealing.sh`: Script to compile and run the work-stealing solution.
- `distributed_tasks.cpp`: Distributed solution using MPI.
- `run_distributed_tasks.sh`: Script to compile and run the distributed MPI solution.
- `run.sh`: Script to compile and run all the files at once. Used to generate the data for graphing between N from 1 to 20, cores from 1 to 4.

## Compilation and Execution

### Sequential Solution

1. **Compile and Run:**
   ```bash
   ./run_sequential.sh
   ```

### Parallel Tasks (Using pthreads)

1. **Compile and Run:**
   ```bash
   ./run_parallel_tasks.sh
   ```

### Queue Work Stealing

1. **Compile and Run:**

   ```bash
   ./run_queue_work_stealing.sh [board_size]
   ```

   - Optionally specify `board_size` to change the default board size (8).

### Distributed Tasks (Using MPI)

1. **Compile and Run:**

   ```bash
   ./run_distributed_tasks.sh
   ```

   - This script runs the MPI program with 4 processes and a board size of 15 by default. Adjust the script if different settings are needed.

### RUN ALL

1. **Compile and Run:**

   ```bash
   ./run.sh
   ```

   - This script runs all the programs one by one. 

## License

MIT
