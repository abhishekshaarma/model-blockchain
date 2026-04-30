#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

// Merkle Tree -- a binary tree of SHA-256 hashes
// Each leaf is the hash of one transaction
// Each internal node is the hash of its two children concatenated
// The root summarizes all transactions in one 64-char hex string
//
// Property: if ANY transaction changes, the root hash changes
// This is what gives blockchain its tamper-evidence

class MerkleTree
{
public:
    std::vector<std::string> leaves_;   // hashes of individual transactions
    std::string              root_;     // final root hash

    // hash a single string using SHA-256, returns 64-char hex string
    static std::string sha256(const std::string& data)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(data.c_str()),
               data.size(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

        return ss.str();
    }

    // add a transaction string as a new leaf
    void addTransaction(const std::string& transaction)
    {
        leaves_.push_back(sha256(transaction));
    }

    // build the tree bottom-up and return the root hash
    std::string buildRoot()
    {
        if (leaves_.empty())
            return "";

        std::vector<std::string> level = leaves_;

        // keep hashing pairs until one node remains
        while (level.size() > 1)
        {
            std::vector<std::string> nextLevel;

            for (size_t i = 0; i < level.size(); i += 2)
            {
                if (i + 1 < level.size())
                {
                    // hash left and right children together
                    nextLevel.push_back(sha256(level[i] + level[i + 1]));
                }
                else
                {
                    // odd number of nodes -- duplicate the last one
                    nextLevel.push_back(sha256(level[i] + level[i]));
                }
            }

            level = nextLevel;
        }

        root_ = level[0];
        return root_;
    }

    // print the tree level by level for visualization
    void print() const
    {
        if (leaves_.empty())
        {
            std::cout << "Empty tree\n";
            return;
        }

        std::vector<std::string> level = leaves_;
        int depth = 0;

        std::cout << "\n--- Merkle Tree ---\n";

        while (level.size() > 1)
        {
            std::cout << "Level " << depth << " (" << level.size() << " nodes):\n";

            for (const auto& h : level)
                std::cout << "  " << h.substr(0, 16) << "...\n";

            std::vector<std::string> nextLevel;
            for (size_t i = 0; i < level.size(); i += 2)
            {
                if (i + 1 < level.size())
                    nextLevel.push_back(sha256(level[i] + level[i + 1]));
                else
                    nextLevel.push_back(sha256(level[i] + level[i]));
            }

            level = nextLevel;
            depth++;
        }

        std::cout << "Root:\n";
        std::cout << "  " << level[0] << "\n";
        std::cout << "-------------------\n";
    }
};
