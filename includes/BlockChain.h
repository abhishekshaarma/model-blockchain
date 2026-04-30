#pragma once
#include "Block.h"
#include <vector>
#include <iostream>

class BlockChain
{
public:
    std::vector<Block> chain_;
    int                difficulty_;   // proof of work difficulty

    BlockChain(int difficulty = 2)
    {
        difficulty_ = difficulty;
        chain_.push_back(createGenesisBlock());
    }

    Block createGenesisBlock()
    {
        Block genesis(0, "0000000000000000");
        genesis.merkleRoot_ = "0000000000000000";
        genesis.mine(difficulty_);
        return genesis;
    }

    Block* getLatestBlock()
    {
        return &chain_.back();
    }

    // add a new block with transactions already added
    void addBlock(Block& block)
    {
        block.prevHash_  = getLatestBlock()->blockHash_;
        block.timestamp_ = time(nullptr);
        block.mine(difficulty_);
        chain_.push_back(block);
    }

    // validate entire chain
    bool isChainValid()
    {
        for (size_t i = 1; i < chain_.size(); i++)
        {
            Block& curr = chain_[i];
            Block& prev = chain_[i - 1];

            // check block hash is still correct
            if (!curr.isHashValid())
            {
                std::cout << "  [INVALID] block " << i << " hash mismatch\n";
                return false;
            }

            // check chain linkage
            if (curr.prevHash_ != prev.blockHash_)
            {
                std::cout << "  [INVALID] block " << i << " broken chain link\n";
                return false;
            }
        }

        return true;
    }

    void print()
    {
        std::cout << "\n========== Blockchain ==========\n";
        for (auto& b : chain_)
            b.print();
        std::cout << "================================\n";
        std::cout << "Chain valid: "
