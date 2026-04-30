#include <iostream>
#include "includes/FieldElement.h"
#include "includes/ECPoint.h"
#include "includes/ScalarMul.h"
#include "includes/Benchmark.h"

int main()
{
    std::cout << "==============================================\n";
    std::cout << " CISS358 -- ECC Scalar Multiplication Benchmark\n";
    std::cout << "==============================================\n";

    // ── setup ──────────────────────────────────────
    // small demo curve: y^2 = x^3 + 2x + 3  (mod 97)
    // this curve has known points we can verify against
    ull prime = 97;
    ull a     = 2;
    ull b     = 3;

    FieldElement::setPrime(prime);
    ECPoint::setCurve(a, b);

    // generator point G = (3, 6) -- verified point on y^2 = x^3 + 2x + 3 mod 97
    ECPoint G(3, 6);

    std::cout << "\nCurve    : y^2 = x^3 + " << a << "x + " << b
              << "  (mod " << prime << ")\n";
    std::cout << "Generator: ";
    G.print();

    // ── correctness check ──────────────────────────
    // all four algorithms must agree on the same result
    std::cout << "\n--- Correctness Check (k=13) ---\n";
    ull k = 13;

    ECPoint r1 = ScalarMul::naive(G, k);
    ECPoint r2 = ScalarMul::doubleAndAdd(G, k);
    ECPoint r3 = ScalarMul::montgomeryLadder(G, k);
    ECPoint r4 = ScalarMul::windowedNAF(G, k);

    std::cout << "Naive        : "; r1.print();
    std::cout << "DoubleAndAdd : "; r2.print();
    std::cout << "Montgomery   : "; r3.print();
    std::cout << "WindowedNAF  : "; r4.print();

    bool allMatch = (r1 == r2) && (r2 == r3) && (r3 == r4);
    std::cout << "All match    : " << (allMatch ? "YES" : "NO") << "\n";

    // ── single benchmark ───────────────────────────
    std::cout << "\n--- Single Benchmark ---\n";
    Benchmark::runAll(G, 1000);
    Benchmark::runAll(G, 50000);

    // ── scaling test ───────────────────────────────
    // shows how each algorithm scales as k grows
    // naive excluded -- too slow for large k
    std::cout << "\n--- Scaling Test ---\n";

    std::vector<ull> kValues = {
        100,
        1000,
        10000,
        100000,
        500000,
        1000000
    };

    Benchmark::runScalingTest(G, kValues);

    return 0;
}
