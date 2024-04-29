#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <memory>

const int BOARD_SIZE = 4;
std::vector<std::thread> workers;
std::vector<std::queue<std::function<void()>>> messages;
std::vector<std::unique_ptr<std::mutex>> queue_mutexes;
std::atomic<int> solutions_count{0};
std::atomic<bool> stop_signal{false};

bool isSafe(int row, int col, std::vector<int> &positions)
{
    for (int i = 0; i < col; i++)
    {
        if (positions[i] == row || abs(positions[i] - row) == abs(i - col))
            return false;
    }
    return true;
}

void solveNQueens(int col, std::vector<int> &positions, int n)
{
    if (col == n)
    {
        solutions_count++;
        return;
    }

    for (int row = 0; row < n; ++row)
    {
        if (isSafe(row, col, positions))
        {
            positions[col] = row;
            solveNQueens(col + 1, positions, n);
            positions[col] = -1;
        }
    }
}

void worker(int id)
{
    while (!stop_signal)
    {
        std::function<void()> job;
        {
            std::lock_guard<std::mutex> lock(*queue_mutexes[id]);
            if (!messages[id].empty())
            {
                job = std::move(messages[id].front());
                messages[id].pop();
            }
        }
        if (job)
            job();
    }
}

void distributeJobs(int n)
{
    for (int i = 0; i < n; ++i)
    {
        std::vector<int> positions(n, -1);
        int worker_id = i % workers.size();
        std::lock_guard<std::mutex> lock(*queue_mutexes[worker_id]);
        messages[worker_id].push([positions, n]() mutable
                                 { solveNQueens(0, positions, n); });
    }
}

int main()
{
    int max_workers = std::thread::hardware_concurrency();
    messages.resize(max_workers);
    queue_mutexes.resize(max_workers);

    for (int i = 0; i < max_workers; ++i)
    {
        queue_mutexes[i] = std::make_unique<std::mutex>();
        workers.emplace_back(worker, i);
    }

    distributeJobs(BOARD_SIZE);

    // Ensure all jobs are finished
    for (auto &queue : messages)
    {
        std::lock_guard<std::mutex> lock(*queue_mutexes[&queue - &messages[0]]);
        while (!queue.empty())
            ;
    }

    stop_signal = true;
    for (auto &worker : workers)
    {
        worker.join();
    }

    std::cout << "Total solutions: " << solutions_count << std::endl;
    return 0;
}

// #include <iostream>
// #include <thread>
// #include <vector>
// #include <mutex>
// #include <condition_variable>
// #include <queue>
// #include <functional>
// #include <atomic>

// const int BOARD_SIZE = 8; // Assuming board size is a constant, similar to `size(8).`

// std::vector<std::thread> workers;
// std::vector<std::queue<std::function<void()>>> messages;
// std::mutex cout_mutex;
// std::atomic<int> solutions_count{0};

// bool isSafe(int row, int col, std::vector<int> &positions)
// {
//     for (int i = 0; i < col; i++)
//     {
//         if (positions[i] == row || abs(positions[i] - row) == abs(i - col))
//             return false;
//     }
//     return true;
// }

// void solveNQueens(int col, std::vector<int> &positions, int n)
// {
//     if (col == n)
//     {
//         std::lock_guard<std::mutex> lock(cout_mutex);
//         solutions_count++;
//         return;
//     }

//     for (int row = 0; row < n; ++row)
//     {
//         if (isSafe(row, col, positions))
//         {
//             positions[col] = row;
//             solveNQueens(col + 1, positions, n);
//             positions[col] = -1;
//         }
//     }
// }

// void worker(int id)
// {
//     while (true)
//     {
//         std::function<void()> job;
//         if (!messages[id].empty())
//         {
//             job = messages[id].front();
//             messages[id].pop();
//             job(); // Execute the job
//         }
//     }
// }

// void distributeJobs(int n)
// {
//     for (int i = 0; i < n; ++i)
//     {
//         // Create a separate copy of `positions` for each job
//         std::vector<int> positions(n, -1);                           // This is now inside the loop
//         messages[i % workers.size()].push([positions, n]() mutable { // capture by value but mutable
//             solveNQueens(0, positions, n);
//         });
//     }
// }

// // void distributeJobs(int n) {
// //     std::vector<int> positions(n, -1);
// //     for (int i = 0; i < n; ++i) {
// //         messages[i % workers.size()].push([=]{
// //             solveNQueens(0, positions, n);
// //         });
// //     }
// // }

// int main()
// {
//     int max_workers = std::thread::hardware_concurrency();
//     messages.resize(max_workers);

//     for (int i = 0; i < max_workers; ++i)
//     {
//         workers.emplace_back(worker, i);
//     }

//     distributeJobs(BOARD_SIZE);

//     for (auto &worker : workers)
//     {
//         worker.join();
//     }

//     std::cout << "Total solutions: " << solutions_count << std::endl;
//     return 0;
// }
