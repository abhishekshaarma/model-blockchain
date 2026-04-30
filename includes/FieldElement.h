#pragma once
#include <string>

// For demo purposes we use a small 64-bit prime field
// Real secp256k1 uses a 256-bit prime but the algorithm logic is identical
// p = 2^31 - 1 (Mersenne prime, good for demos)

typedef unsigned long long ull;

class FieldElement
{
public:
    ull value_;
    static ull prime_;

    static void setPrime(ull p);

    FieldElement();
    FieldElement(ull val);

    FieldElement operator+(const FieldElement& other) const;
    FieldElement operator-(const FieldElement& other) const;
    FieldElement operator*(const FieldElement& other) const;
    FieldElement operator-()                          const; // negation

    bool operator==(const FieldElement& other) const;
    bool operator!=(const FieldElement& other) const;

    FieldElement inv() const;   // a^(p-2) mod p  (Fermat's little theorem)
    bool isZero()      const;

    std::string toHex() const;
};
