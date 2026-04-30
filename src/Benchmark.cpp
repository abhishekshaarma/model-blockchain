#include "Benchmark.h"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std::chrono;

// times one algorithm and returns average nanoseconds over repetitions
long long Benchmark::timeAlgorithm(const std::string& name,
                                   const ECPoint& P,
                                   ull k,
                                   int repetitions)
{
    long long total = 0;

    for (int i = 0; i < repetitions; i++)
    {
        auto start = high_resolution_clock::now();

        if      (name == "Naive")          ScalarMul::naive(P, k);
        else if (name == "DoubleAndAdd")   ScalarMul::doubleAndAdd(P, k);
        else if (name == "Montgomery")     ScalarMul::montgomeryLadder(P, k);
        else if (name == "WindowedNAF")    ScalarMul::windowedNAF(P, k);

        auto end = high_resolution_clock::now();
        total += duration_cast<nanoseconds>(end - start).count();
    }

    return total / repetitions;   // average
}

// runs all 4 algorithms and prints a formatted comparison table
void Benchmark::runAll(const ECPoint& P, ull k)
{
    std::vector<std::string> algorithms = {
        "Naive", "DoubleAndAdd", "Montgomery", "WindowedNAF"
    };

    std::cout << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "  Benchmark  k = " << k << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::left
              << std::setw(20) << "Algorithm"
              << std::setw(20) << "Avg Time (ns)"
              << std::setw(20) << "Result Point"
              << "\n";
    std::cout << std::string(60, '-') << "\n";

    // skip naive for large k -- it would run forever
    bool skipNaive = (k > 100000);

    for (const auto& name : algorithms)
    {
        if (skipNaive && name == "Naive")
        {
            std::cout << std::left
                      << std::setw(20) << "Naive"
                      << std::setw(20) << "skipped (k too large)"
                      << "\n";
            continue;
        }

        long long ns = timeAlgorithm(name, P, k);

        ECPoint result = ECPoint();
        if      (name == "Naive")        result = ScalarMul::naive(P, k);
        else if (name == "DoubleAndAdd") result = ScalarMul::doubleAndAdd(P, k);
        else if (name == "Montgomery")   result = ScalarMul::montgomeryLadder(P, k);
        else if (name == "WindowedNAF")  result = ScalarMul::windowedNAF(P, k);

        std::string point = result.isInfinity() ? "infinity" :
                            "(" + std::to_string(result.x_.value_) +
                            "," + std::to_string(result.y_.value_) + ")";

        std::cout << std::left
                  << std::setw(20) << name
                  << std::setw(20) << ns
                  << std::setw(20) << point
                  << "\n";
    }

    std::cout << std::string(60, '=') << "\n";
}

// runs benchmark across multiple k values to show how each algo scales
void Benchmark::runScalingTest(const ECPoint& P, const std::vector<ull>& kValues)
{
    std::vector<BenchmarkResult> allResults;

    std::vector<std::string> algorithms = {
        "DoubleAndAdd", "Montgomery", "WindowedNAF"
    };

    for (ull k : kValues)
    {
        std::cout << "\nTesting k = " << k << " ...\n";

        for (const auto& name : algorithms)
        {
            long long ns = timeAlgorithm(name, P, k);

            BenchmarkResult r;
            r.algorithmName = name;
            r.scalarK       = k;
            r.nanoseconds   = ns;

            allResults.push_back(r);
        }
    }

    printCSV(allResults);
}

// prints CSV output -- copy paste into Excel or Python for charts
void Benchmark::printCSV(const std::vector<BenchmarkResult>& results)
{
    std::cout << "\n\n--- CSV OUTPUT (copy into Excel) ---\n";
    std::cout << "Algorithm,k,Nanoseconds\n";

    for (const auto& r : results)
    {
        std::cout << r.algorithmName << ","
                  << r.scalarK      << ","
                  << r.nanoseconds  << "\n";
    }

    std::cout << "--- END CSV ---\n";
}
