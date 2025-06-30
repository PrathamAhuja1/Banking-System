#pragma once
#include <string>
#include <vector>
#include "Transaction.h"
using namespace std;

class BankAccount {
public:
    BankAccount() = default;
    BankAccount(int accNo, const string &holder, double initBalance = 0.0);

    int getAccountNumber() const;
    const string& getHolderName() const;
    double getBalance() const;
    const vector<Transaction>& getTransactions() const;

    bool deposit(double amount);
    bool withdraw(double amount);
    void addTransaction(const Transaction &tr);

    string serialize() const;
    static BankAccount deserialize(const string &line);

private:
    int accountNumber=0;
    string holderName;
    double balance=0.0;
    vector<Transaction> transactions;
};
