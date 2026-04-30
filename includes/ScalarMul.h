#pragma once
#include "ECPoint.h"
#include <vector>

// Scalar Multiplication: compute k*P for integer k and curve point P
//
// This is the core algorithmic problem of the project.
// We implement three algorithms and benchmark them against each other:
//
//   1. Double-and-Add      -- O(log k) basic version
//   2. Montgomery Ladder   -- O(log k) constant-time (no data-dependent branches)
//   3. Windowed NAF        -- O(log k) with fewer additions via precomputation
//
// All three produce the same result -- they differ in speed and security

class ScalarMul
{
public:

    // 1. Naive repeated addition -- O(k) baseline for comparison
    static ECPoint naive(const ECPoint& P, ull k);

    // 2. Double-and-Add (binary method) -- O(log k)
    static ECPoint doubleAndAdd(const ECPoint& P, ull k);

    // 3. Montgomery Ladder -- O(log k), constant time
    static ECPoint montgomeryLadder(const ECPoint& P, ull k);

    // 4. Windowed NAF -- O(log k) with reduced additions
    static ECPoint windowedNAF(const ECPoint& P, ull k, int windowSize = 4);

private:
    // Helper: compute Non-Adjacent Form (NAF) of k
    // NAF is a signed binary representation with no two consecutive nonzero digits
    // This minimizes the number of point additions needed
    static std::vector<int> computeNAF(ull k);

    // Helper: compute windowed NAF digits of k
    static std::vector<int> computeWindowedNAF(ull k, int w);
};
