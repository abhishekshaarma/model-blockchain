#pragma once
#include "ScalarMul.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

// A Wallet holds an ECC keypair
//   privateKey -- a random scalar k
//   publicKey  -- the curve point Q = k*G
//
// The generator point G is a fixed well-known point on the curve
// Anyone who knows G and Q cannot easily find k -- this is the
// Elliptic Curve Discrete Logarithm Problem (ECDLP)

class Wallet
{
public:
    ull     privateKey_;
    ECPoint publicKey_;
    std::string address_;   // simplified address = last 8 hex digits of public key x

    // the shared generator point G -- set once at startup
    static ECPoint G_;
    static void setGenerator(const ECPoint& G)
    {
        G_ = G;
    }

    // generate a new random wallet
    static Wallet generate()
    {
        Wallet w;

        // private key is a random scalar in range [1, prime-1]
        // in real ECC this would be a cryptographically secure random number
        w.privateKey_ = (ull)(rand() % (FieldElement::prime_ - 1)) + 1;

        // public key is Q = privateKey * G  (scalar multiplication)
        w.publicKey_  = ScalarMul::montgomeryLadder(G_, w.privateKey_);

        // address = simple hash of public key x coordinate
        std::stringstream ss;
        ss << std::hex << w.publicKey_.x_.value_;
        std::string hex = ss.str();
        w.address_ = hex.length() >= 8 ?
                     hex.substr(hex.length() - 8) :
                     hex;

        return w;
    }

    void print() const
    {
        std::cout << "Wallet:\n";
        std::cout << "  Private key : " << privateKey_          << "\n";
        std::cout << "  Public key  : ";
        publicKey_.print();
        std::cout << "  Address     : " << address_             << "\n";
    }
};

// ─────────────────────────────────────────────────────────────
// ECDSA -- Elliptic Curve Digital Signature Algorithm
// Sign a message with a private key
// Verify a signature with the corresponding public key
//
// Signing:
//   1. hash the message to get e
//   2. pick a random nonce r
//   3. compute R = r*G, take x coordinate as sig.r
//   4. compute sig.s = r^-1 * (e + privateKey * sig.r) mod order
//
// Verifying:
//   1. hash the message to get e
//   2. compute u1 = e * s^-1 mod order
//   3. compute u2 = sig.r * s^-1 mod order
//   4. compute point X = u1*G + u2*publicKey
//   5. valid if X.x == sig.r
// ─────────────────────────────────────────────────────────────

struct Signature
{
    ull r;
    ull s;

    void print() const
    {
        std::cout << "Signature: (r=" << r << ", s=" << s << ")\n";
    }
};

class ECDSA
{
public:
    // curve order -- for demo we use the same prime as field
    // in real secp256k1 the order is a different large prime
    static ull order_;

    static void setOrder(ull order)
    {
        order_ = order;
    }

    // modular inverse using Fermat's little theorem mod order
    static ull modInv(ull a, ull mod)
    {
        ull result = 1;
        ull base   = a % mod;
        ull exp    = mod - 2;

        while (exp > 0)
        {
            if (exp & 1)
                result = (result * base) % mod;
            base = (base * base) % mod;
            exp >>= 1;
        }

        return result;
    }

    // sign a message string with a private key
    static Signature sign(const std::string& message, ull privateKey)
    {
        // step 1: hash message to a number
        ull e = simpleHash(message) % order_;

        Signature sig;
        sig.r = 0;
        sig.s = 0;

        // keep trying until we get a valid signature
        while (sig.r == 0 || sig.s == 0)
        {
            // step 2: random nonce
            ull nonce = (ull)(rand() % (order_ - 1)) + 1;

            // step 3: R = nonce * G
            ECPoint R = ScalarMul::montgomeryLadder(Wallet::G_, nonce);
            if (R.isInfinity()) continue;

            sig.r = R.x_.value_ % order_;
            if (sig.r == 0) continue;

            // step 4: s = nonce^-1 * (e + privateKey * r) mod order
            ull nonceInv = modInv(nonce, order_);
            sig.s = (nonceInv * ((e + privateKey * sig.r) % order_)) % order_;
        }

        return sig;
    }

    // verify a signature against a message and public key
    static bool verify(const std::string& message,
                       const Signature&   sig,
                       const ECPoint&     publicKey)
    {
        if (sig.r == 0 || sig.s == 0)     return false;
        if (sig.r >= order_)              return false;
        if (sig.s >= order_)              return false;

        // step 1: hash message
        ull e = simpleHash(message) % order_;

        // step 2 & 3: u1 and u2
        ull sInv = modInv(sig.s, order_);
        ull u1   = (e      * sInv) % order_;
        ull u2   = (sig.r  * sInv) % order_;

        // step 4: X = u1*G + u2*publicKey
        ECPoint X = ScalarMul::montgomeryLadder(Wallet::G_,  u1) +
                    ScalarMul::montgomeryLadder(publicKey,   u2);

        if (X.isInfinity()) return false;

        // step 5: valid if X.x == sig.r
        return (X.x_.value_ % order_) == sig.r;
    }

    // simple hash: djb2 algorithm, maps string to ull
    static ull simpleHash(const std::string& s)
    {
        ull hash = 5381;
        for (char c : s)
            hash = ((hash << 5) + hash) + (ull)c;
        return hash;
    }
};

// define static members
inline ECPoint Wallet::G_  = ECPoint();
inline ull     ECDSA::order_ = 2147483647ULL;
