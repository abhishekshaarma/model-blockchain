#pragma once
#include <vector>
#include "Block.h"

class BlockChain
{
  private:
    
  public:    
    std::vector<Block> chain;
    BlockChain()
    {
        chain.push_back(createGenesisBlock());
    }
    
    /* void addBlock(TransactionData data); */
    /* void isChainValid(); */
    Block createGenesisBlock()
    {
        time_t  current;
        TransactionData d;
        d.amount = 0;
        d.receiverKey = "None";
        d.senderKey = "None";
        d.timestamp = time(&current);
        std::hash<int> hash1;
        Block genesis(0, d, hash1(0));
        return genesis; 
    }
    Block* getLatestBlock()
    {
        return &chain.back();
    }
    void addBlock(TransactionData d)
    {
        int index = chain.size();
        Block newBlock(index, d, getLatestBlock()->getHash());
        chain.push_back(newBlock);
    }

    

    bool isChainValid()
    {
        for (size_t i = 1; i < chain.size(); i++)
        {
            Block& currentBlock = chain[i];
            Block& previousBlock = chain[i - 1];

            if (!currentBlock.isHashValid())
                return false;

            if (currentBlock.getPrevHash() != previousBlock.getHash())
                return false;
        }

        return true;
    }
    
};


