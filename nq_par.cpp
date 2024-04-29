#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#define N 15 // Board size
#define NUM_THREADS 4 // Number of threads

using namespace std;
using namespace chrono;

int totalSolutions = 0;
mutex mtx;

// Function to check if it's safe to place a queen at a given position
bool isSafe(int row, int col, int* board) {
    for (int i = 0; i < col; i++) {
        // Check if there's a queen in the same row or diagonal
        if (board[i] == row || abs(board[i] - row) == abs(i - col)) {
            return false;
        }
    }
    return true;
}

// Function to solve N-Queens problem recursively
void solveNQueens(int col, int* board) {
    if (col == N) {
        mtx.lock();
        totalSolutions++;
        mtx.unlock();
        return;
    }

    // Iterate over each row in the column
    for (int row = 0; row < N; row++) {
        if (isSafe(row, col, board)) {
            board[col] = row; // Place the queen
            solveNQueens(col + 1, board); // Recur for the next column
            // No need to reset the board[col] to 0, as it will be overwritten in the next iteration
        }
    }
}

// Function for each thread to solve a subset of the N-Queens problem
void solveSubset(int startCol, int* board) {
    int localCount = 0; // Local count of solutions for this thread

    for (int col = startCol; col < N; col += NUM_THREADS) {
        int tempBoard[N] = {0}; // Create a temporary board for this thread
        solveNQueens(col, tempBoard); // Solve N-Queens for the subset of columns
        localCount++; // Increment local count for each solution found
    }

    mtx.lock(); // Lock before updating totalSolutions
    totalSolutions += localCount; // Add local count to total count
    mtx.unlock(); // Unlock after updating totalSolutions
}

int main() {
    auto start = high_resolution_clock::now();
    
    // Create threads
    thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        int board[N] = {0}; // Initialize the board for each thread
        threads[i] = thread(solveSubset, i, board);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Total solutions: " << totalSolutions << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}
