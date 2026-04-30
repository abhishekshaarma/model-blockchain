#include <iostream>
#include <cstdlib>
#include <ctime>
#include "includes/FieldElement.h"
#include "includes/ECPoint.h"
#include "includes/ScalarMul.h"
#include "includes/Wallet.h"
#include "includes/MerkleTree.h"
#include "includes/BlockChain.h"

int main()
{
    srand(time(nullptr));

    std::cout << "==============================================\n";
    std::cout << " CISS358 -- ECC Blockchain Demo\n";
    std::cout << "==============================================\n";

    // ── setup curve ────────────────────────────────
    ull prime = 97;
    ull a     = 2;
    ull b     = 3;

    FieldElement::setPrime(prime);
    ECPoint::setCurve(a, b);

    // generator point G
    ECPoint G(3, 6);
    Wallet::setGenerator(G);
    ECDSA::setOrder(prime);

    // ── generate wallets ───────────────────────────
    std::cout << "\n--- Generating Wallets ---\n";

    Wallet alice = Wallet::generate();
    Wallet bob   = Wallet::generate();
    Wallet carol = Wallet::generate();

    std::cout << "Alice  "; alice.print();
    std::cout << "Bob    "; bob.print();
    std::cout << "Carol  "; carol.print();

    // ── create blockchain ──────────────────────────
    std::cout << "\n--- Creating Blockchain (difficulty=2) ---\n";
    BlockChain chain(2);

    // ── block 1 ────────────────────────────────────
    std::cout << "\n--- Building Block 1 ---\n";
    Block block1(1, chain.getLatestBlock()->blockHash_);

    // alice sends bob 10 coins
    TransactionData tx1;
    tx1.senderKey   = alice.address_;
    tx1.receiverKey = bob.address_;
    tx1.amount      = 10.0;
    tx1.timestamp   = time(nullptr);

    std::string tx1Str = tx1.senderKey + "->" +
                         tx1.receiverKey + ":" +
                         std::to_string(tx1.amount);

    Signature sig1 = ECDSA::sign(tx1Str, alice.privateKey_);
    std::cout << "Alice signs tx1: "; sig1.print();

    bool ok1 = block1.addTransaction(tx1, sig1, alice.publicKey_);
    std::cout << "tx1 accepted: " << (ok1 ? "YES" : "NO") << "\n";

    // bob sends carol 5 coins
    TransactionData tx2;
    tx2.senderKey   = bob.address_;
    tx2.receiverKey = carol.address_;
    tx2.amount      = 5.0;
    tx2.timestamp   = time(nullptr);

    std::string tx2Str = tx2.senderKey + "->" +
                         tx2.receiverKey + ":" +
                         std::to_string(tx2.amount);

    Signature sig2 = ECDSA::sign(tx2Str, bob.privateKey_);
    std::cout << "Bob signs tx2  : "; sig2.print();

    bool ok2 = block1.addTransaction(tx2, sig2, bob.publicKey_);
    std::cout << "tx2 accepted: " << (ok2 ? "YES" : "NO") << "\n";

    chain.addBlock(block1);

    // ── block 2 ────────────────────────────────────
    std::cout << "\n--- Building Block 2 ---\n";
    Block block2(2, chain.getLatestBlock()->blockHash_);

    // carol sends alice 3 coins
    TransactionData tx3;
    tx3.senderKey   = carol.address_;
    tx3.receiverKey = alice.address_;
    tx3.amount      = 3.0;
    tx3.timestamp   = time(nullptr);

    std::string tx3Str = tx3.senderKey + "->" +
                         tx3.receiverKey + ":" +
                         std::to_string(tx3.amount);

    Signature sig3 = ECDSA::sign(tx3Str, carol.privateKey_);
    std::cout << "Carol signs tx3: "; sig3.print();

    bool ok3 = block2.addTransaction(tx3, sig3, carol.publicKey_);
    std::cout << "tx3 accepted: " << (ok3 ? "YES" : "NO") << "\n";

    chain.addBlock(block2);

    // ── print full chain ───────────────────────────
    chain.print();

    // ── tamper demo ────────────────────────────────
    std::cout << "\n--- Tamper Detection Demo ---\n";
    std::cout << "Modifying block 1 transaction amount from 10 to 9999...\n";
    chain.chain_[1].transactions_[0].amount = 9999;

    std::cout << "Chain valid after tamper: "
              << (chain.isChainValid() ? "YES" : "NO") << "\n";

    // ── bad signature demo ─────────────────────────
    std::cout << "\n--- Invalid Signature Demo ---\n";
    Block block3(3, chain.getLatestBlock()->blockHash_);

    TransactionData tx4;
    tx4.senderKey   = alice.address_;
    tx4.receiverKey = bob.address_;
    tx4.amount      = 999.0;
    tx4.timestamp   = time(nullptr);

    std::string tx4Str = tx4.senderKey + "->" +
                         tx4.receiverKey + ":" +
                         std::to_string(tx4.amount);

    // bob tries to sign as alice -- should be rejected
    Signature badSig = ECDSA::sign(tx4Str, bob.privateKey_);
    std::cout << "Bob tries to sign as Alice: ";
    bool ok4 = block3.addTransaction(tx4, badSig, alice.publicKey_);
    std::cout << "tx4 accepted: " << (ok4 ? "YES" : "NO") << "\n";

    // ── merkle tree demo ───────────────────────────
    std::cout << "\n--- Merkle Tree Visualization ---\n";
    MerkleTree tree;
    tree.addTransaction(tx1Str);
    tree.addTransaction(tx2Str);
    tree.addTransaction(tx3Str);
    tree.buildRoot();
    tree.print();

    return 0;
}
