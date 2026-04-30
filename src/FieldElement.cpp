#include "FieldElement.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

// initialize static members
ull FieldElement::prime_ = 2147483647ULL; // 2^31 - 1 default

void FieldElement::setPrime(ull p)
{
    prime_ = p;
}

FieldElement::FieldElement()
{
    value_ = 0;
}

FieldElement::FieldElement(ull val)
{
    value_ = val % prime_;
}

FieldElement FieldElement::operator+(const FieldElement& other) const
{
    return FieldElement((value_ + other.value_) % prime_);
}

FieldElement FieldElement::operator-(const FieldElement& other) const
{
    return FieldElement((value_ + prime_ - other.value_) % prime_);
}

FieldElement FieldElement::operator*(const FieldElement& other) const
{
    return FieldElement((value_ * other.value_) % prime_);
}

FieldElement FieldElement::operator-() const
{
    return FieldElement((prime_ - value_) % prime_);
}

bool FieldElement::operator==(const FieldElement& other) const
{
    return value_ == other.value_;
}

bool FieldElement::operator!=(const FieldElement& other) const
{
    return value_ != other.value_;
}

// Fermat's little theorem: a^(p-2) mod p
FieldElement FieldElement::inv() const
{
    if (isZero())
        throw std::runtime_error("Cannot invert zero element");

    ull result = 1;
    ull base   = value_ % prime_;
    ull exp    = prime_ - 2;

    // fast exponentiation (square and multiply)
    while (exp > 0)
    {
        if (exp % 2 == 1)
            result = (result * base) % prime_;
        base = (base * base) % prime_;
        exp /= 2;
    }

    return FieldElement(result);
}

bool FieldElement::isZero() const
{
    return value_ == 0;
}

std::string FieldElement::toHex() const
{
    std::stringstream ss;
    ss << std::hex << value_;
    return ss.str();
}
