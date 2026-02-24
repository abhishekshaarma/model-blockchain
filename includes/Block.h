#pragma once
#include "TransactionData.h"
#include <ctime>
#include <functional>

class Block
{
private:
    int index_;
    size_t blockHash_;
    size_t prevHash_;

public:
    TransactionData data_;
    Block(int inx , TransactionData d, size_t prevHash);
    size_t generateHash(); const
    size_t getHash(); const
    
    size_t getPrevHash(); const
    bool isHashValid(); const
    
    Block *getLatestBlock();
};
