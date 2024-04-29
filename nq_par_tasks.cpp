#include <iostream>
#include <pthread.h>
#include <chrono>
using namespace std;
using namespace chrono;

const int N = 6; // Change N to the desired board size
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
bool solutionFound = false;
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




void solveNQueensThread(int board[N][N], int col) {
    if (col >= N) {
        pthread_mutex_lock(&myMutex);
        totalSolutions++;
        pthread_mutex_unlock(&myMutex);
        return;
    }

    for (int i = 0; i < N; i++) {
        if (isSafe(board, i, col)) {
            board[i][col] = 1;
            solveNQueensThread(board, col + 1);
            board[i][col] = 0; // Backtrack
        }
    }
}

void* threadFunction(void* arg) {
    int col = *(int*)arg;
    int board[N][N] = {0};
    solveNQueensThread(board, col);
    return nullptr;
}

int main() {
    pthread_t threads[N];
    int threadArgs[N];

    auto start = high_resolution_clock::now();

    // Create N threads
    for (int i = 0; i < N; ++i) {
        threadArgs[i] = i;
        pthread_create(&threads[i], nullptr, threadFunction, &threadArgs[i]);
    }

    // Join N threads
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], nullptr);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);

    cout << "Number of solutions: " << totalSolutions << endl;
    cout << "Execution time: " << duration.count() << " nanoseconds" << endl;

    return 0;
}
