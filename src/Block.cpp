#include "Block.h"

Block::Block(int idx, TransactionData d, size_t prevHash)
{
    index_  = idx;
    data_ = d;
    prevHash_ = prevHash;
    blockHash_ = generateHash();
}

size_t Block::generateHash() const
{
    std::hash<std::string> hash1;
    std::hash<size_t> hash2;
    std::hash<size_t> finalHash;
    std::string toHash = std::to_string(data_.amount) + data_.receiverKey +
        data_.senderKey + std::to_string(data_.timestamp);
    return finalHash(hash1(toHash) + hash2(prevHash_));
};

size_t Block::getHash() const
{
    return blockHash_;
}
size_t Block::getPrevHash() const
{
    return prevHash_;
}
bool Block::isHashValid() const
{
    return (generateHash() == blockHash_);
}
