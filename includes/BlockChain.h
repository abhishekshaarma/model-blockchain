#pragma once
#include <vector>
#include "Block.h"

class BlockChain
{
private:
    std::vector<Block> chain;

public:
    BlockChain();

    Block createGenesisBlock();
    Block* getLatestBlock();
    void addBlock(TransactionData d);
    bool isChainValid();
};
