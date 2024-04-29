#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

const int N = 8; // Size of the board and number of queens
std::mutex queueMutex;
std::condition_variable cv_not_full;
std::condition_variable cv_not_empty;
std::queue<std::vector<int>> taskQueue; // Queue for tasks
std::atomic<bool> finished(false);
std::atomic<int> totalSolutions(0);

bool isSafe(int row, int col, const std::vector<int>& positions) {
    for (int i = 0; i < col; ++i) {
        if (positions[i] == row || abs(positions[i] - row) == col - i) {
            return false;
        }
    }
    return true;
}

void solveNQueens(std::vector<int> positions, int col) {
    if (col == N) {
        totalSolutions++;
        return;
    }
    for (int row = 0; row < N; ++row) {
        if (isSafe(row, col, positions)) {
            positions[col] = row;
            solveNQueens(positions, col + 1);
        }
    }
}

void producer(int startRow) {
    std::vector<int> positions(N, -1);
    positions[0] = startRow; // Initial placement
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv_not_full.wait(lock, [] { return taskQueue.size() < 10; }); // Wait until there's space
        taskQueue.push(positions);
        cv_not_empty.notify_one();
    }
}

void consumer() {
    while (true) {
        std::vector<int> positions;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv_not_empty.wait(lock, [] { return !taskQueue.empty() || finished; });
            if (taskQueue.empty() && finished) {
                break;
            }
            positions = taskQueue.front();
            taskQueue.pop();
        }
        solveNQueens(positions, 1); // Start solving from the second column
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < N; ++i) {
        producers.emplace_back(producer, i);
    }

    for (int i = 0; i < 4; ++i) {
        consumers.emplace_back(consumer);
    }

    for (auto& producer : producers) {
        producer.join();
    }

    finished = true;
    cv_not_empty.notify_all();

    for (auto& consumer : consumers) {
        consumer.join();
    }

    std::cout << "Total solutions found: " << totalSolutions << std::endl;
    return 0;
}
