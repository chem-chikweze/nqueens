#include <iostream>
#include <chrono>
#include <cstdlib>  // For atoi
using namespace std;
using namespace chrono;

bool isSafe(int board[], int row, int col, int N)
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

void printBoard(int board[], int N)
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

bool solveNQueensUtil(int board[], int col, int &count, int N)
{
    if (col == N)
    {
        // printBoard(board, N);  // Uncomment to print the board
        count++;
        return true;
    }

    bool res = false;
    for (int i = 0; i < N; i++)
    {
        if (isSafe(board, i, col, N))
        {
            board[col] = i;
            res = solveNQueensUtil(board, col + 1, count, N) || res;
        }
    }
    return res;
}

int solveNQueens(int N)
{
    int count = 0;
    int *board = new int[N];
    if (solveNQueensUtil(board, 0, count, N))
    {
        delete[] board;
        return count;
    }
    delete[] board;
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <N>" << endl;
        return 1;
    }

    int N = atoi(argv[1]);  // Convert argument to integer

    if (N <= 0)
    {
        cerr << "N should be a positive integer." << endl;
        return 1;
    }

    cout << "Number of queens: " << N << endl;

    auto start = high_resolution_clock::now();
    int count = solveNQueens(N);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    if (count != -1)
    {
        cout << "Number of solutions: " << count << endl;
    }
    else
    {
        cout << "No solution exists." << endl;
    }
    cout << "Execution time: " << duration.count() << " microseconds" << endl;
    cout << "Sequential" << endl;
    return 0;
}
