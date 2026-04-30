#pragma once
#include "FieldElement.h"

// Elliptic curve in Weierstrass form: y^2 = x^3 + ax + b  (mod p)
// For our demo we use a small curve with known generator point G
// Real secp256k1 uses a=0, b=7 -- we keep the same form

class ECPoint
{
public:
    FieldElement x_;
    FieldElement y_;
    bool infinity_; // true if this is the "point at infinity" (identity element)

    // Curve parameters (shared across all points)
    static FieldElement a_;
    static FieldElement b_;

    // Set up the curve -- call once at startup
    static void setCurve(ull a, ull b);

    // Constructors
    ECPoint();                                      // point at infinity
    ECPoint(ull x, ull y);                          // from coordinates
    ECPoint(const FieldElement& x, const FieldElement& y);

    // Core curve operations
    ECPoint operator+(const ECPoint& other) const;  // point addition
    ECPoint doubled()                       const;  // point doubling (faster than P+P)
    bool    operator==(const ECPoint& other) const;
    bool    isInfinity()                     const;

    void print() const;
};
