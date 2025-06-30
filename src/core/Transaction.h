#pragma once
#include <string>
#include <chrono>
using namespace std;

struct Transaction {
    string timestamp;
    string type; // "Deposit", "Withdraw", "Transfer"
    double amount;
    int relatedAccount;

    Transaction(){
        timestamp="";
        type="";
        amount=0.0;
        relatedAccount=-1;
    }
    Transaction(const string &ts, const string &t, double amt, int rel = -1){
            timestamp=ts;
            type=t;
            amount=amt;
            relatedAccount=rel;
        }

    string serialize() const;
    static Transaction deserialize(const string &line);
};
