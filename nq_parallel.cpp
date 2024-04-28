#include <pthread.h>
#include <iostream>
#include <chrono>
#define N 4 // Board size
#define NUM_THREADS 3  // Number of threads


using namespace std;
using namespace chrono;

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int totalSolutions = 0;

bool isSafe(int board[N][N], int row, int col) {
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

void solveNQueensThread(int board[N][N], int start_row, int end_row, int current_column) {
    if (current_column == N) {
        pthread_mutex_lock(&myMutex);
        totalSolutions++;
        pthread_mutex_unlock(&myMutex);
        return;
    }

    if (current_column == 0) {
        for (int i = start_row; i < end_row; i++) {
            if (isSafe(board, i, current_column)) {
                board[i][current_column] = 1;
                solveNQueensThread(board, start_row, end_row, current_column+1);
                board[i][current_column] = 0;
            }
        }
    } else {
        for (int i = 0; i < N; i++) {
            if (isSafe(board, i, current_column)) {
                board[i][current_column] = 1;
                solveNQueensThread(board, 0, N, current_column+1); // Start from row 0 for subsequent columns
                board[i][current_column] = 0;
            }
        }
    }
}

void* threadEntryPoint(void* args) {
    int* threadIdPtr = (int*)args;
    int threadId = *threadIdPtr;

    // Calculate rows for this thread
    int rowsPerThread = N / NUM_THREADS;
    int startRow = threadId * rowsPerThread;
    int endRow = (threadId == NUM_THREADS - 1) ? N : startRow + rowsPerThread;

    // Create board
    int board[N][N] = {0};

    // Solve N-Queens for this thread's rows
    solveNQueensThread(board, startRow, endRow, 0);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];

    auto start = high_resolution_clock::now();
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, threadEntryPoint, &threadIds[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);

    std::cout << "Total solutions: " << totalSolutions << std::endl;
    cout << "Execution time: " << duration.count() << " nanoseconds" << endl;
    std::cout << "Parallel " << std::endl;
    return 0;
}