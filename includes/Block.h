#pragma once
#include "TransactionData.h"
#include "MerkleTree.h"
#include "Wallet.h"
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

class Block
{
public:
    int         index_;
    std::string blockHash_;
    std::string prevHash_;
    std::string merkleRoot_;
    time_t      timestamp_;
    int         nonce_;          // proof of work

    // transactions in this block
    std::vector<TransactionData> transactions_;

    // signatures parallel to transactions_
    std::vector<Signature> signatures_;

    // compute SHA-256 of a string -- returns 64 char hex
    static std::string sha256(const std::string& data)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(data.c_str()),
               data.size(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            ss << std::hex << std::setw(2)
               << std::setfill('0') << (int)hash[i];

        return ss.str();
    }

    // build a string from all block fields for hashing
    std::string blockData() const
    {
        std::stringstream ss;
        ss << index_ << prevHash_ << merkleRoot_ << timestamp_ << nonce_;
        return ss.str();
    }

    // compute block hash
    std::string calculateHash() const
    {
        return sha256(blockData());
    }

    // proof of work -- mine until hash starts with difficulty zeros
    void mine(int difficulty)
    {
        std::string target(difficulty, '0');
        nonce_ = 0;

        std::cout << "Mining block " << index_ << " ";

        while (blockHash_.substr(0, difficulty) != target)
        {
            nonce_++;
            blockHash_ = calculateHash();
            if (nonce_ % 100000 == 0)
                std::cout << "." << std::flush;
        }

        std::cout << " mined! nonce=" << nonce_ << "\n";
    }

    // constructor
    Block(int index, std::string prevHash)
    {
        index_     = index;
        prevHash_  = prevHash;
        nonce_     = 0;
        timestamp_ = time(nullptr);
        blockHash_ = calculateHash();
    }

    // add a signed transaction to this block
    bool addTransaction(const TransactionData& tx,
                        const Signature&       sig,
                        const ECPoint&         senderPublicKey)
    {
        // build the transaction string for verification
        std::string txStr = tx.senderKey    + "->" +
                            tx.receiverKey  + ":" +
                            std::to_string(tx.amount);

        // verify signature before accepting
        if (!ECDSA::verify(txStr, sig, senderPublicKey))
        {
            std::cout << "  [REJECTED] invalid signature on transaction\n";
            return false;
        }

        transactions_.push_back(tx);
        signatures_.push_back(sig);

        // rebuild merkle root with new transaction
        MerkleTree tree;
        for (const auto& t : transactions_)
        {
            std::string s = t.senderKey   + "->" +
                            t.receiverKey + ":" +
                            std::to_string(t.amount);
            tree.addTransaction(s);
        }

        merkleRoot_ = tree.buildRoot();
        blockHash_  = calculateHash();

        return true;
    }

    // check hash is still valid
    bool isHashValid() const
    {
        return blockHash_ == calculateHash();
    }

    void print() const
    {
        std::cout << "\n--- Block " << index_ << " ---\n";
        std::cout << "  Hash      : " << blockHash_.substr(0, 32) << "...\n";
        std::cout << "  PrevHash  : " << prevHash_.substr(0, 32)  << "...\n";
        std::cout << "  MerkleRoot: " << merkleRoot_.substr(0, 32) << "...\n";
        std::cout << "  Nonce     : " << nonce_       << "\n";
        std::cout << "  Txns      : " << transactions_.size() << "\n";

        for (size_t i = 0; i < transactions_.size(); i++)
        {
            std::cout << "    [" << i << "] "
                      << transactions_[i].senderKey   << " -> "
                      << transactions_[i].receiverKey << " : "
                      << transactions_[i].amount      << "\n";
        }
    }
};
