#include <iostream>
#include <queue>
#include <pthread.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono> 


#define N 5
#define NUM_THREADS 5

using namespace std;
using namespace chrono;

struct Task {
    int board[N][N];
    int column;
};

mutex queueMutex;
condition_variable cv;
condition_variable cv_queue_not_full;

queue<Task> tasks;
int totalSolutions = 0;
bool finished = false;

bool isSafe(int board[N][N], int row, int col) {
    for (int i = 0; i < col; i++)
        if (board[row][i]) return false;
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
        if (board[i][j]) return false;
    for (int i = row, j = col; j >= 0 && i < N; i++, j--)
        if (board[i][j]) return false;
    return true;
}


void* solveNQueens(void* arg) {
    while (true) {
        // Lock scope reduced to necessary operations
        Task task;
        {
            unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [] { return !tasks.empty() || finished; });

            if (finished && tasks.empty()) {
                return NULL;  // Directly return from the thread function
            }

            task = tasks.front();  // Copy the task to work on it without holding the lock
            tasks.pop();
        }  // Lock is automatically released here

        if (task.column == N) {
            {
                unique_lock<std::mutex> lock(queueMutex);  // Lock only when updating shared resources
                totalSolutions++;
            }
            cv_queue_not_full.notify_all();  // Notify outside of the lock
            continue;
        }

        for (int i = 0; i < N; i++) {
            if (isSafe(task.board, i, task.column)) {
                Task newTask = task;
                newTask.board[i][task.column] = 1;  // Place a queen
                newTask.column++;  // Move to the next column

                {
                    unique_lock<std::mutex> lock(queueMutex);
                    cv_queue_not_full.wait(lock, [] { return tasks.size() < NUM_THREADS * 1000; });
                    tasks.push(newTask);
                }  // Lock is automatically released here

                cv.notify_one();  // Notify outside of the lock
                cv_queue_not_full.notify_all();
            }
        }
    }
}
 
 
// void* solveNQueens(void* arg) {
//     while (true) {
//         unique_lock<std::mutex> lock(queueMutex);
//         cv.wait(lock, [] { return !tasks.empty() || finished; });

//         if (finished && tasks.empty()) {
//             lock.unlock();
//             pthread_exit(NULL);
//         }

//         Task task = tasks.front();
//         tasks.pop();
//         lock.unlock();

//         if (task.column == N) {
//             lock.lock();
//             totalSolutions++;
//             lock.unlock();
//             cv_queue_not_full.notify_all();  // Notify in case adding was blocked
//             continue;
//         }

//         for (int i = 0; i < N; i++) {
//             if (isSafe(task.board, i, task.column)) {
//                 Task newTask = task;
//                 newTask.board[i][task.column] = 1; // Place a queen
//                 newTask.column++; // Move to the next column

//                 lock.lock();
//                 // Wait until there's room to add a new task
//                 cv_queue_not_full.wait(lock, [] { return tasks.size() < NUM_THREADS * 10; });
//                 tasks.push(newTask);
//                 lock.unlock();

//                 cv.notify_one(); // Notify one thread that a new task is available
//                 cv_queue_not_full.notify_all(); // Allow waiting threads to add tasks if space available
//             }
//         }
//     }
// }



int main() {
    pthread_t threads[NUM_THREADS];
    int board[N][N] = {0};

    // Start timing
    auto start = high_resolution_clock::now(); // Get the starting time point

    // Initialize the task queue with the first column
    for (int i = 0; i < N; i++) {
        Task initTask;
        memcpy(initTask.board, board, sizeof(board));
        initTask.board[i][0] = 1;
        initTask.column = 1;
        {
            lock_guard<std::mutex> lock(queueMutex);
            tasks.push(initTask);
        }
    }

    // Start threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, solveNQueens, NULL);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Stop timing
    auto stop = high_resolution_clock::now(); // Get the ending time point

    {
        lock_guard<std::mutex> lock(queueMutex);
        finished = true;
    }
    cv.notify_all(); // Notify all threads to exit

    // Calculate the duration
    auto duration = duration_cast<milliseconds>(stop - start);

    // Output the results
    cout << "Total solutions: " << totalSolutions << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}