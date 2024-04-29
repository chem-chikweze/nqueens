#include <pthread.h>
#include <iostream>
#include <chrono>
#define N 15          // Board size
#define NUM_THREADS 10 // Number of threads

using namespace std;
using namespace chrono;

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int totalSolutions = 0;

typedef unsigned long long bitboard;

bool isSafe(bitboard row, bitboard ld, bitboard rd, int col) {
    // Check if there's a queen in the same column
    if (row & (1ULL << col)) {
        return false;
    }

    // Check the left diagonal (up and to the left)
    if (ld & (1ULL << col)) {
        return false;
    }

    // Check the right diagonal (up and to the right)
    if (rd & (1ULL << col)) {
        return false;
    }

    // If no conflicts, it's safe to place a queen
    return true;
}


// bool isSafe(bitboard row, bitboard ld, bitboard rd, int col)
// {
//     bitboard pos = 1ULL << col;
//     return !(row & pos) && !(ld & (pos << 1)) && !(rd & (pos >> 1));
// }

void solveNQueens(bitboard row, bitboard ld, bitboard rd, bitboard rowMask)
{
    if (rowMask == 0)
    {
        pthread_mutex_lock(&myMutex);
        totalSolutions++;
        pthread_mutex_unlock(&myMutex);
        return;
    }

    bitboard pos = rowMask & -rowMask;
    rowMask -= pos;

    int col = __builtin_ctzll(pos);
    if (isSafe(row, ld, rd, col))
    {
        solveNQueens(row | pos, (ld | pos) << 1, (rd | pos) >> 1, rowMask);
    }
    solveNQueens(row, ld, rd, rowMask);
}

void *threadEntryPoint(void *args)
{
    int *threadIdPtr = (int *)args;
    int threadId = *threadIdPtr;

    // Calculate rows for this thread
    int rowsPerThread = N / NUM_THREADS;
    int startRow = threadId * rowsPerThread;
    int endRow = (threadId == NUM_THREADS - 1) ? N : startRow + rowsPerThread;

    // Create bitboards for tracking occupied rows, left and right diagonals
    bitboard row = 0, ld = 0, rd = 0;
    bitboard rowMask = (1ULL << N) - 1;

    // Solve N-Queens for this thread's rows
    solveNQueens(row, ld, rd, rowMask & (rowMask << startRow));

    pthread_exit(NULL);
}

// void solveNQueens(int row, bitboard ld, bitboard rd, bitboard rowMask) {
//     if (rowMask == 0) {
//         pthread_mutex_lock(&myMutex);
//         totalSolutions++;
//         pthread_mutex_unlock(&myMutex);
//         return;
//     }

//     bitboard pos = rowMask & -rowMask;
//     rowMask -= pos;

//     int col = __builtin_ctzll(pos);
//     if (isSafe(row, ld, rd, col)) {
//         solveNQueens(row | pos, (ld | pos) << 1, (rd | pos) >> 1, rowMask);
//     }
//     solveNQueens(row, ld, rd, rowMask);
// }

// void* threadEntryPoint(void* args) {
//     int* threadIdPtr = (int*)args;
//     int threadId = *threadIdPtr;

//     // Calculate rows for this thread
//     int rowsPerThread = N / NUM_THREADS;
//     int startRow = threadId * rowsPerThread;
//     int endRow = (threadId == NUM_THREADS - 1) ? N : startRow + rowsPerThread;

//     // Create bitboards for tracking occupied rows, left and right diagonals
//     bitboard row = 0, ld = 0, rd = 0;
//     bitboard rowMask = (1ULL << N) - 1;

//     // Solve N-Queens for this thread's rows
//     solveNQueens(row, ld, rd, rowMask & (rowMask << startRow));

//     pthread_exit(NULL);
// }

int main()
{
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    auto start = high_resolution_clock::now();

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, threadEntryPoint, &threadIds[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);

    std::cout << "Total solutions: " << totalSolutions << std::endl;
    cout << "Execution time: " << duration.count() << " nanoseconds" << endl;
    std::cout << "Parallel " << std::endl;
    return 0;
}
