#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

void randomGenerate(vector<vector<int>> &m, int n)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 32767);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            m[i][j] = dist(gen);
        }
    }
}

int calcDuration(void (*method)(const vector<vector<int>> &m1, const vector<vector<int>> &m2, vector<vector<int>> &res, int n), const vector<vector<int>> &m1, const vector<vector<int>> &m2, vector<vector<int>> &res, int n)
{
    auto start = high_resolution_clock::now();
    method(m1, m2, res, n);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    return duration.count();
}

void withOpenMP(const vector<vector<int>> &m1, const vector<vector<int>> &m2, vector<vector<int>> &res, int n)
{
#pragma omp parallel for shared(m1, m2, res)
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            res[i][j] = m1[i][j] * m2[i][j];
        }
    }
}

void clean(const vector<vector<int>> &m1, const vector<vector<int>> &m2, vector<vector<int>> &res, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            res[i][j] = m1[i][j] * m2[i][j];
        }
    }
}

int main()
{
    int n;
    size_t iter_count;

    cout << "Matrix size: ";
    cin >> n;

    cout << endl
         << "Iterations: ";
    cin >> iter_count;

    int num_threads = omp_get_max_threads();

    cout << endl
         << "Threads: " << num_threads << endl
         << endl;

    vector<vector<int>> m1(n, vector<int>(n));
    vector<vector<int>> m2(n, vector<int>(n));
    vector<vector<int>> res(n, vector<int>(n));

    double avgClean = 0;
    double avgOpenMP = 0;

    for (size_t i = 0; i < iter_count; i++)
    {
        randomGenerate(m1, n);
        randomGenerate(m2, n);

        int durationClean = calcDuration(clean, m1, m2, res, n);
        cout << "Clean: " << durationClean << " ms" << endl;

        int durationOpenMP = calcDuration(withOpenMP, m1, m2, res, n);
        cout << "OpenMP: " << durationOpenMP << " ms" << endl;

        cout << endl;

        avgClean += durationClean;
        avgOpenMP += durationOpenMP;
    }

    cout << "Avg Clean: " << avgClean / iter_count << endl;
    cout << "Avg OpenMP: " << avgOpenMP / iter_count << endl;
    cout << "Diff (Clean / OpenMP): " << (avgClean / avgOpenMP - 1) * 100 << "%" << endl;

    return 0;
}
