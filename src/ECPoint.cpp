#include "ECPoint.h"
#include <iostream>
#include <stdexcept>

// initialize static members
FieldElement ECPoint::a_ = FieldElement(0);
FieldElement ECPoint::b_ = FieldElement(7);

void ECPoint::setCurve(ull a, ull b)
{
    a_ = FieldElement(a);
    b_ = FieldElement(b);
}

// point at infinity (identity element)
ECPoint::ECPoint()
{
    infinity_ = true;
}

ECPoint::ECPoint(ull x, ull y)
{
    x_        = FieldElement(x);
    y_        = FieldElement(y);
    infinity_ = false;
}

ECPoint::ECPoint(const FieldElement& x, const FieldElement& y)
{
    x_        = x;
    y_        = y;
    infinity_ = false;
}

bool ECPoint::isInfinity() const
{
    return infinity_;
}

bool ECPoint::operator==(const ECPoint& other) const
{
    if (infinity_ && other.infinity_) return true;
    if (infinity_ || other.infinity_) return false;
    return x_ == other.x_ && y_ == other.y_;
}

// Point doubling: P + P
// slope = (3 * x^2 + a) / (2 * y)
ECPoint ECPoint::doubled() const
{
    if (infinity_)          return ECPoint();   // 2 * infinity = infinity
    if (y_.isZero())        return ECPoint();   // tangent is vertical

    FieldElement two(2);
    FieldElement three(3);

    FieldElement slope = (three * x_ * x_ + a_) * (two * y_).inv();
    FieldElement xr    = slope * slope - x_ - x_;
    FieldElement yr    = slope * (x_ - xr) - y_;

    return ECPoint(xr, yr);
}

// Point addition: P + Q
// slope = (y2 - y1) / (x2 - x1)
ECPoint ECPoint::operator+(const ECPoint& other) const
{
    // identity cases
    if (infinity_)       return other;
    if (other.infinity_) return *this;

    // P + (-P) = infinity
    if (x_ == other.x_ && y_ != other.y_)
        return ECPoint();

    // P + P = 2P
    if (*this == other)
        return doubled();

    // general case
    FieldElement slope = (other.y_ - y_) * (other.x_ - x_).inv();
    FieldElement xr    = slope * slope - x_ - other.x_;
    FieldElement yr    = slope * (x_ - xr) - y_;

    return ECPoint(xr, yr);
}

void ECPoint::print() const
{
    if (infinity_)
        std::cout << "Point(infinity)\n";
    else
        std::cout << "Point(" << x_.value_ << ", " << y_.value_ << ")\n";
}
