#include <iostream>
#include <chrono>
#include <cstdlib> // For atoi
using namespace std;
using namespace chrono;

#define N 14 // Board size

bool isSafe(int board[], int row, int col)
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

void printBoard(int board[])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (board[j] == i)
            {
                cout << "Q ";
            }
            else
            {
                cout << ". ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

bool solveNQueensUtil(int board[], int col, int &count)
{
    if (col == N)
    {
        // printBoard(board);
        count++;
        return true;
    }

    bool res = false;
    for (int i = 0; i < N; i++)
    {
        if (isSafe(board, i, col))
        {
            board[col] = i;
            res = solveNQueensUtil(board, col + 1, count) || res;
        }
    }
    return res;
}

int solveNQueens()
{
    int count = 0;
    int *board = new int[N];
    if (solveNQueensUtil(board, 0, count))
    {
        delete[] board;
        return count;
    }
    delete[] board;
    return -1;
}

int main(int argc, char *argv[])
{
    // if (argc != 2) {
    //     cerr << "Usage: " << argv[0] << " <N>" << endl;
    //     return 1;
    // }

    if (N <= 0)
    {
        cerr << "N should be a positive integer." << endl;
        return 1;
    }

    cout << "Number of queens: " << N << endl;

    auto start = high_resolution_clock::now();
    int count = solveNQueens();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    if (count != -1)
    {
        cout << "Number of solutions: " << count << endl;
    }
    else
    {
        cout << "No solution exists." << endl;
    }
    cout << "Execution time: " << duration.count() << "  milliseconds" << endl;
    std::cout << "Sequential " << std::endl;
    return 0;
}
