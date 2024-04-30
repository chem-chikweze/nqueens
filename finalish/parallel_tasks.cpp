#include <pthread.h>
#include <iostream>
#include <chrono>
#include <cstdlib> // For atoi
#include <vector>

using namespace std;
using namespace chrono;

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int totalSolutions = 0;

bool isSafe(vector<vector<int>> &board, int row, int col, int N)
{
    int i, j;
    // Check this row on the left side
    for (i = 0; i < col; i++)
        if (board[row][i] == 1)
            return false;

    // Check upper diagonal on left side
    for (i = row, j = col; i >= 0 && j >= 0; i--, j--)
        if (board[i][j] == 1)
            return false;

    // Check lower diagonal on left side
    for (i = row, j = col; j >= 0 && i < N; i++, j--)
        if (board[i][j] == 1)
            return false;

    return true;
}

void solveNQueensThread(vector<vector<int>> &board, int start_row, int end_row, int current_column, int N)
{
    if (current_column == N)
    {
        pthread_mutex_lock(&myMutex);
        totalSolutions++;
        pthread_mutex_unlock(&myMutex);
        return;
    }

    for (int i = start_row; i < end_row; i++)
    {
        if (isSafe(board, i, current_column, N))
        {
            board[i][current_column] = 1;
            solveNQueensThread(board, 0, N, current_column + 1, N); // Start from row 0 for subsequent columns
            board[i][current_column] = 0;
        }
    }
}

void *threadEntryPoint(void *args)
{
    int *params = (int *)args;
    int threadId = params[0];
    int N = params[1];
    int NUM_THREADS = params[2];

    // Calculate rows for this thread
    int rowsPerThread = N / NUM_THREADS;
    int startRow = threadId * rowsPerThread;
    int endRow = (threadId == NUM_THREADS - 1) ? N : startRow + rowsPerThread;

    // Create board and initialize
    vector<vector<int>> board(N, vector<int>(N, 0));

    // Solve N-Queens for this thread's rows
    solveNQueensThread(board, startRow, endRow, 0, N);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <N> <NUM_THREADS>" << endl;
        return 1;
    }

    int N = atoi(argv[1]);
    int NUM_THREADS = atoi(argv[2]);

    pthread_t threads[NUM_THREADS];
    int params[NUM_THREADS][3]; // Store thread ID, N, and NUM_THREADS for each thread

    auto start = high_resolution_clock::now();

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        params[i][0] = i;
        params[i][1] = N;
        params[i][2] = NUM_THREADS;
        pthread_create(&threads[i], NULL, threadEntryPoint, &params[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Total solutions: " << totalSolutions << endl;
    cout << "Execution time: " << duration.count() << " microseconds" << endl;
    cout << "Parallel using " << NUM_THREADS << " threads." << endl;
    return 0;
}