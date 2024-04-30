#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace chrono;

int isSafe(int *board, int row, int col, int N)
{
    for (int i = 0; i < col; i++)
    {
        if (board[i] == row || abs(board[i] - row) == col - i)
        {
            return false;
        }
    }
    return true;
}

int solveNQueensUtil(int *board, int col, int N)
{
    if (col == N)
    {
        return 1; // Found one solution
    }

    int count = 0;
    for (int i = 0; i < N; i++)
    {
        if (isSafe(board, i, col, N))
        {
            board[col] = i;
            count += solveNQueensUtil(board, col + 1, N);
        }
    }
    return count;
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 14; // Default board size
    if (argc > 1)
    {
        N = atoi(argv[1]);
    }

    int *board = new int[N];
    int global_count = 0;

    auto start = high_resolution_clock::now();

    // Divide the first row choices among nodes
    int local_count = 0;
    for (int i = rank; i < N; i += size)
    {
        board[0] = i;
        local_count += solveNQueensUtil(board, 1, N);
    }

    // Reduce all local counts to global count
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    if (rank == 0)
    {
        cout << "Number of queens: " << N << endl;
        cout << "Total solutions found: " << global_count << endl;
        cout << "Execution time: " << duration.count() << " microseconds" << endl;
    }

    delete[] board;
    MPI_Finalize();
    return 0;
}
