#pragma once
#include <string>
#include <chrono>
using namespace std;

struct Transaction {
    string timestamp;
    string type; // "Deposit", "Withdraw", "Transfer"
    double amount;
    int relatedAccount; // for transfers; else -1

    Transaction() = default;
    Transaction(const string &ts, const string &t, double amt, int rel = -1)
        : timestamp(ts), type(t), amount(amt), relatedAccount(rel) {}
    // Serialize to a line: timestamp|type|amount|relatedAccount
    string serialize() const;
    static Transaction deserialize(const string &line);
};
