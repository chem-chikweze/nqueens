#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <bitset>
#include <chrono>
#include <cstdlib>
#include <future>

using namespace std;
using namespace chrono;

int board_size;
atomic<int> solutions_found(0);

bool is_safe(const bitset<64>& occupied_rows, const bitset<64>& occupied_diagonals1, const bitset<64>& occupied_diagonals2, int row, int col) {
    return !occupied_rows[row] && !occupied_diagonals1[row + col] && !occupied_diagonals2[row - col + board_size - 1];
}

void solve_nqueens(const bitset<64>& occupied_rows, const bitset<64>& occupied_diagonals1, const bitset<64>& occupied_diagonals2, int col) {
    if (col == board_size) {
        solutions_found++;
        return;
    }

    bitset<64> new_occupied_rows = occupied_rows;
    bitset<64> new_occupied_diagonals1 = occupied_diagonals1;
    bitset<64> new_occupied_diagonals2 = occupied_diagonals2;

    for (int row = 0; row < board_size; row++) {
        if (is_safe(occupied_rows, occupied_diagonals1, occupied_diagonals2, row, col)) {
            new_occupied_rows.set(row);
            new_occupied_diagonals1.set(row + col);
            new_occupied_diagonals2.set(row - col + board_size - 1);

            solve_nqueens(new_occupied_rows, new_occupied_diagonals1, new_occupied_diagonals2, col + 1);

            new_occupied_rows.reset(row);
            new_occupied_diagonals1.reset(row + col);
            new_occupied_diagonals2.reset(row - col + board_size - 1);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        board_size = stoi(argv[1]);
    } else {
        board_size = 8;
    }

    auto start = high_resolution_clock::now();

    vector<future<void>> futures;
    int num_threads = thread::hardware_concurrency();

    bitset<64> initial_occupied_rows, initial_occupied_diagonals1, initial_occupied_diagonals2;

    for (int i = 0; i < num_threads; i++) {
        futures.emplace_back(async(launch::async, solve_nqueens, initial_occupied_rows, initial_occupied_diagonals1, initial_occupied_diagonals2, i));
    }

    for (auto& fut : futures) {
        fut.get();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Number of queens: " << board_size << endl;
    cout << "Total solutions found: " << solutions_found << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

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
