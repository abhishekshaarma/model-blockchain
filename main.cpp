#include <iostream>
#include "Block.h"
#include "BlockChain.h"
int main()
{
    BlockChain coin;
    
    TransactionData data1;
    TransactionData data2;
    
    time_t  data_time;
    data1.amount = 1;
    data1.receiverKey = "Alice";
    data1.senderKey = "Bob";
    data1.timestamp = time(&data_time);

    coin.addBlock(data1);

    std::cout << "Is chain Valid" << std::endl
              << coin.isChainValid();
//    time_t  data_time;
    data2.amount = 0.12312312;
    data2.receiverKey = "John";
    data2.senderKey = "Fred";
    data2.timestamp = time(&data_time);
    
    coin.addBlock(data2);
    std::cout << "Is chain Valid2" << std::endl
              << coin.isChainValid();
    std::cout << std::endl;
    Block * jack = coin.getLatestBlock();
    jack->data_.amount = 1000;
    //jack->data1.amount = 1000;


    
    std::cout << "Is chain Valid" << std::endl
              << coin.isChainValid();

}
