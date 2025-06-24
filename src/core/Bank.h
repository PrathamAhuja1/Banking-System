// Bank.h
#pragma once
#include <vector>
#include <string>
#include "BankAccount.h"
#include "Transaction.h"
#include <mutex>

class Bank {
public:
    Bank(const std::string &dataFile, const std::string &logFile, const std::string &masterPassword);

    bool load();   // decrypt & load accounts and transactions
    bool save();   // serialize & encrypt accounts and transactions

    BankAccount* createAccount(const std::string &holderName, double initDeposit);
    BankAccount* findAccount(int accountNumber);
    bool deleteAccount(int accountNumber);

    bool deposit(int accountNumber, double amount);
    bool withdraw(int accountNumber, double amount);

    // Log transaction: append to log file (encrypted on disk)
    bool logTransaction(const Transaction &tr);

    const std::vector<BankAccount>& getAllAccounts() const;

private:
    std::vector<BankAccount> accounts;
    std::string dataFilePath; // encrypted file path
    std::string logFilePath;  // encrypted transaction log
    std::string tempPlainData; // temp plaintext filename
    std::string tempPlainLog;  // temp plaintext log filename
    std::string masterPwd;

    std::mutex mtx; // protect save/load and log

    int nextAccountNumber();
    bool loadPlainData(const std::string &plainPath);
    bool savePlainData(const std::string &plainPath);
    bool loadPlainLog(const std::string &plainPath);
    bool savePlainLog(const std::string &plainPath);
};
