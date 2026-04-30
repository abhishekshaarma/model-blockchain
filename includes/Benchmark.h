#pragma once
#include "ScalarMul.h"
#include <string>
#include <vector>

// Benchmarking results for one algorithm at one key size
struct BenchmarkResult
{
    std::string algorithmName;
    ull         scalarK;
    long long   nanoseconds;
    ECPoint     result;
};

class Benchmark
{
public:
    // runs all 4 algorithms on the same P and k and prints a comparison table
    static void runAll(const ECPoint& P, ull k);

    // runs one algorithm multiple times and returns average nanoseconds
    static long long timeAlgorithm(const std::string& name,
                                   const ECPoint& P,
                                   ull k,
                                   int repetitions = 100);

    // runs runAll across a range of k values to show scaling behavior
    static void runScalingTest(const ECPoint& P,
                               const std::vector<ull>& kValues);

    // prints results as a CSV -- paste directly into Excel for your paper charts
    static void printCSV(const std::vector<BenchmarkResult>& results);
};
