#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <cstdlib>

using namespace std;
using namespace chrono;

int N = 8; // Default board size, can be overridden from command line

std::mutex mtx;
std::condition_variable cv;
std::queue<vector<int>> work_queue;
std::atomic<int> solutions_found(0);
std::atomic<bool> finished(false);

bool isSafe(const vector<int> &board, int row, int col)
{
    for (int i = 0; i < col; i++)
    {
        if (board[i] == row || abs(board[i] - row) == col - i)
        {
            return false;
        }
    }
    return true;
}

void worker_thread()
{
    while (true)
    {
        vector<int> board;
        {
            unique_lock<mutex> lock(mtx);
            if (work_queue.empty())
            {
                if (finished)
                    break;
                cv.wait(lock, []
                        { return finished || !work_queue.empty(); });
                if (finished && work_queue.empty())
                    break;
            }
            board = work_queue.front();
            work_queue.pop();
        }

        int col = board.size();
        if (col == N)
        {
            solutions_found++;
            continue;
        }

        for (int row = 0; row < N; row++)
        {
            if (isSafe(board, row, col))
            {
                board.push_back(row);
                {
                    lock_guard<mutex> lock(mtx);
                    work_queue.push(board);
                    cv.notify_one();
                }
                board.pop_back();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        N = atoi(argv[1]);

    auto start = high_resolution_clock::now();

    int num_threads = thread::hardware_concurrency();
    vector<thread> threads;

    work_queue.push({});
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(worker_thread);
    }

    {
        lock_guard<mutex> lock(mtx);
        finished = true;
    }
    cv.notify_all();

    for (auto &t : threads)
    {
        t.join();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Number of queens: " << N << endl;
    cout << "Total solutions found: " << solutions_found.load() << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}

// #include <iostream>
// #include <vector>
// #include <queue>
// #include <thread>
// #include <atomic>
// #include <algorithm>
// #include <numeric>
// #include <mutex>
// #include <condition_variable>
// #include <chrono>
// #include <cstdlib>
// #include <deque>

// using namespace std;
// using namespace chrono;

// std::mutex mtx;
// std::condition_variable cv;
// std::deque<std::vector<int>> work_queue;
// std::atomic<int> solutions_found(0);
// std::atomic<bool> finished(false);

// int board_size;

// bool is_safe(const std::vector<int> &board)
// {
//     int n = board.size();
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = i + 1; j < n; j++)
//         {
//             if (board[i] == board[j] || abs(board[i] - board[j]) == j - i)
//             {
//                 return false;
//             }
//         }
//     }
//     return true;
// }

// void worker_thread()
// {
//     while (true)
//     {
//         std::vector<int> board;
//         {
//             std::unique_lock<std::mutex> lock(mtx);
//             if (work_queue.empty())
//             {
//                 if (finished)
//                     break;
//                 cv.wait(lock, []
//                         { return finished || !work_queue.empty(); });
//                 if (finished && work_queue.empty())
//                     break;
//             }
//             board = work_queue.front();
//             work_queue.pop_front();
//         }

//         if (board.size() == board_size)
//         {
//             if (is_safe(board))
//             {
//                 solutions_found++;
//             }
//             continue;
//         }

//         for (int i = 0; i < board_size; ++i)
//         {
//             board.push_back(i);
//             if (is_safe(board))
//             {
//                 std::unique_lock<std::mutex> lock(mtx);
//                 work_queue.push_back(board);
//                 cv.notify_one();
//             }
//             board.pop_back();
//         }
//     }
// }

// int main(int argc, char *argv[])
// {
//     board_size = (argc > 1) ? std::stoi(argv[1]) : 8;
//     auto start = high_resolution_clock::now();

//     int num_threads = std::thread::hardware_concurrency();
//     std::vector<std::thread> threads;

//     work_queue.push_back({});
//     for (int i = 0; i < num_threads; i++)
//     {
//         threads.emplace_back(worker_thread);
//     }

//     std::unique_lock<std::mutex> lock(mtx);
//     finished = true;
//     cv.notify_all();
//     lock.unlock();

//     for (auto &t : threads)
//     {
//         t.join();
//     }

//     auto stop = high_resolution_clock::now();
//     auto duration = duration_cast<milliseconds>(stop - start);

//     std::cout << "Number of queens: " << board_size << std::endl;
//     std::cout << "Total solutions found: " << solutions_found << std::endl;
//     std::cout << "Execution time: " << duration.count() << " milliseconds" << endl;

//     return 0;
// }
