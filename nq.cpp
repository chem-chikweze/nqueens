#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
std::queue<std::vector<int>> work_queue;
std::atomic<int> solutions_found(0);
std::atomic<bool> finished(false);

int board_size;

bool is_safe(const std::vector<int> &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = i + 1; j < board.size(); j++)
        {
            if (std::abs(board[i] - board[j]) == j - i || board[i] == board[j])
            {
                return false;
            }
        }
    }
    return true;
}

void worker_thread()
{
    while (!finished)
    {
        std::vector<int> partial_solution;
        {
            std::unique_lock<std::mutex> lck(mtx);
            cv.wait(lck, [&]
                    { return !work_queue.empty() || finished; });
            if (finished && work_queue.empty())
            {
                return;
            }
            partial_solution = work_queue.front();
            work_queue.pop();
        }

        int start_col = partial_solution.size();
        std::vector<int> solution = partial_solution;
        std::vector<int> remaining(board_size - start_col);
        std::iota(remaining.begin(), remaining.end(), 1);

        do
        {
            for (int i = start_col; i < board_size; i++)
            {
                solution[i] = remaining[i - start_col];
            }
            if (is_safe(solution))
            {
                solutions_found++;
            }
        } while (std::next_permutation(remaining.begin(), remaining.end()));
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        board_size = std::stoi(argv[1]);
    }
    else
    {
        board_size = 8;
    }

    int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(worker_thread);
    }

    std::vector<int> initial_config(board_size - 4);
    std::iota(initial_config.begin(), initial_config.end(), 1);

    do
    {
        work_queue.push(initial_config);
    } while (std::next_permutation(initial_config.begin(), initial_config.end()));

    {
        std::unique_lock<std::mutex> lck(mtx);
        finished = true;
    }
    cv.notify_all();

    for (auto &t : threads)
    {
        t.join();
    }

    std::cout << "Total solutions found: " << solutions_found << std::endl;

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
