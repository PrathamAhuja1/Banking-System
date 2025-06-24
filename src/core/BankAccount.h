// BankAccount.h
#pragma once
#include <string>
#include <vector>
#include "Transaction.h"

class BankAccount {
public:
    BankAccount() = default;
    BankAccount(int accNo, const std::string &holder, double initBalance = 0.0);

    int getAccountNumber() const;
    const std::string& getHolderName() const;
    double getBalance() const;
    const std::vector<Transaction>& getTransactions() const;

    bool deposit(double amount);
    bool withdraw(double amount);
    void addTransaction(const Transaction &tr);

    // Serialize: "accountNumber|holderName|balance"
    std::string serialize() const;
    static BankAccount deserialize(const std::string &line);

private:
    int accountNumber{0};
    std::string holderName;
    double balance{0.0};
    std::vector<Transaction> transactions;
};
