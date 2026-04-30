#include "ScalarMul.h"
#include <stdexcept>

// ─────────────────────────────────────────────
// 1. Naive repeated addition  O(k)
// Simply adds P to itself k times
// Only usable for tiny k -- here to show WHY we need better algorithms
// ─────────────────────────────────────────────
ECPoint ScalarMul::naive(const ECPoint& P, ull k)
{
    if (k == 0) return ECPoint();   // 0*P = infinity

    ECPoint result = P;
    for (ull i = 1; i < k; i++)
        result = result + P;

    return result;
}

// ─────────────────────────────────────────────
// 2. Double-and-Add  O(log k)
// Scan bits of k from MSB to LSB
// At each step: double the accumulator
// If current bit is 1: also add P
//
// Example: k=13 = 1101 in binary
//   start:  R = infinity
//   bit 1:  R = 2*infinity + P  = P
//   bit 1:  R = 2*P + P         = 3P
//   bit 0:  R = 2*3P            = 6P
//   bit 1:  R = 2*6P + P        = 13P
// ─────────────────────────────────────────────
ECPoint ScalarMul::doubleAndAdd(const ECPoint& P, ull k)
{
    if (k == 0) return ECPoint();   // 0*P = infinity

    ECPoint result = ECPoint();     // identity (point at infinity)
    ECPoint addend = P;             // current power of 2 multiple of P

    while (k > 0)
    {
        if (k & 1)                  // if current bit is 1
            result = result + addend;

        addend = addend.doubled();  // double for next bit
        k >>= 1;                    // shift to next bit
    }

    return result;
}

// ─────────────────────────────────────────────
// 3. Montgomery Ladder  O(log k)
// Always does exactly one doubling and one addition per bit
// No data-dependent branches -- constant time regardless of k
// Resistant to timing side-channel attacks
//
// Maintains two points R0 and R1 where R1 = R0 + P at all times
// bit=0: R1 = R0+R1,  R0 = 2*R0
// bit=1: R0 = R0+R1,  R1 = 2*R1
// ─────────────────────────────────────────────
ECPoint ScalarMul::montgomeryLadder(const ECPoint& P, ull k)
{
    if (k == 0) return ECPoint();

    ECPoint R0 = ECPoint();         // identity
    ECPoint R1 = P;

    // find the position of the most significant bit
    int bits = 0;
    ull temp = k;
    while (temp > 0) { bits++; temp >>= 1; }

    // scan from MSB down to LSB
    for (int i = bits - 1; i >= 0; i--)
    {
        if ((k >> i) & 1)           // bit is 1
        {
            R0 = R0 + R1;
            R1 = R1.doubled();
        }
        else                        // bit is 0
        {
            R1 = R0 + R1;
            R0 = R0.doubled();
        }
    }

    return R0;
}

// ─────────────────────────────────────────────
// Helper: compute NAF (Non-Adjacent Form) of k
// NAF is a signed binary representation {-1, 0, 1}
// Key property: no two consecutive nonzero digits
// This guarantees at most ceil(log2(k)/2) + 1 additions
// vs roughly log2(k)/2 for standard binary (similar but
// NAF is provably optimal in terms of nonzero digits)
// ─────────────────────────────────────────────
std::vector<int> ScalarMul::computeNAF(ull k)
{
    std::vector<int> naf;

    while (k > 0)
    {
        if (k & 1)                  // k is odd
        {
            // digit is 2 - (k mod 4) giving values in {-1, 1}
            int digit = 2 - (int)(k % 4);
            naf.push_back(digit);
            k = (ull)((long long)k - digit); // subtract digit
        }
        else
        {
            naf.push_back(0);
        }
        k >>= 1;
    }

    return naf;   // LSB first
}

// ─────────────────────────────────────────────
// Helper: compute windowed NAF digits of k
// Window size w means we process w bits at a time
// Precompute table of odd multiples: P, 3P, 5P ... (2^w - 1)P
// Reduces number of additions further than plain NAF
// ─────────────────────────────────────────────
std::vector<int> ScalarMul::computeWindowedNAF(ull k, int w)
{
    std::vector<int> naf;
    ull windowMod = (1ULL << w);     // 2^w
    ull halfWindow = windowMod >> 1; // 2^(w-1)

    while (k > 0)
    {
        if (k & 1)                   // k is odd
        {
            // take k mod 2^w, centered around 0
            int digit = (int)(k % windowMod);
            if (digit >= (int)halfWindow)
                digit -= (int)windowMod;  // make it negative if > half window

            naf.push_back(digit);
            k = (ull)((long long)k - digit);
        }
        else
        {
            naf.push_back(0);
        }
        k >>= 1;
    }

    return naf;   // LSB first
}

// ─────────────────────────────────────────────
// 4. Windowed NAF  O(log k) with fewer additions
// Precomputes a table of odd multiples of P
// Then scans windowed NAF digits to accumulate result
// windowSize w=4 means table holds 1P, 3P, 5P, 7P (2^(w-1) entries)
// ─────────────────────────────────────────────
ECPoint ScalarMul::windowedNAF(const ECPoint& P, ull k, int windowSize)
{
    if (k == 0) return ECPoint();

    // precompute table: table[i] = (2i+1)*P
    // indices 0..2^(w-1)-1  -->  multiples 1P, 3P, 5P, 7P ...
    int tableSize = 1 << (windowSize - 1);
    std::vector<ECPoint> table(tableSize);

    table[0] = P;
    ECPoint P2 = P.doubled();        // 2P (used to step between odd multiples)

    for (int i = 1; i < tableSize; i++)
        table[i] = table[i - 1] + P2;

    // compute windowed NAF digits
    std::vector<int> naf = computeWindowedNAF(k, windowSize);

    // scan NAF from MSB to LSB
    ECPoint result = ECPoint();      // identity

    for (int i = (int)naf.size() - 1; i >= 0; i--)
    {
        result = result.doubled();

        int digit = naf[i];

        if (digit > 0)
        {
            // table index for digit d is (d-1)/2
            result = result + table[(digit - 1) / 2];
        }
        else if (digit < 0)
        {
            // for negative digit use the negated point
            // negation on elliptic curve: (x, y) -> (x, -y)
            ECPoint neg = table[(-digit - 1) / 2];
            neg.y_      = -neg.y_;
            result      = result + neg;
        }
    }

    return result;
}
