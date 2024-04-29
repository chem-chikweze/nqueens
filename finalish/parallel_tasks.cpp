#include <pthread.h>
#include <iostream>
#include <chrono>
#include <vector>
#define N 8 // Board size
#define NUM_THREADS 3  // Number of threads

using namespace std;
using namespace chrono;

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int totalSolutions = 0;

bool isSafe(vector<int>& board, int row, int col) {
    for (int i = 0; i < col; i++) {
        if (board[i] == row || abs(board[i] - row) == col - i)
            return false;
    }
    return true;
}

void solveNQueensThread(vector<int>& board, int start_row, int end_row, int current_column) {
    if (current_column == N) {
        pthread_mutex_lock(&myMutex);
        totalSolutions++;
        pthread_mutex_unlock(&myMutex);
        return;
    }

    if (current_column == 0) {
        for (int i = start_row; i < end_row; i++) {
            if (isSafe(board, i, current_column)) {
                board[current_column] = i;
                solveNQueensThread(board, start_row, end_row, current_column + 1);
                board[current_column] = -1; // Reset the row for this column
            }
        }
    } else {
        for (int i = 0; i < N; i++) {
            if (isSafe(board, i, current_column)) {
                board[current_column] = i;
                solveNQueensThread(board, 0, N, current_column + 1); // Start from row 0 for subsequent columns
                board[current_column] = -1; // Reset the row for this column
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

    // Create board and initialize
    vector<int> board(N, -1);

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
    auto duration = duration_cast<microseconds>(stop - start);

    std::cout << "Total solutions: " << totalSolutions << std::endl;
    cout << "Execution time: " << duration.count() << " microseconds" << endl;
    std::cout << "Parallel " << std::endl;
    return 0;
}
