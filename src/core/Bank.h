#pragma once
#include <vector>
#include <string>
#include "BankAccount.h"
#include "Transaction.h"
#include <mutex>
using namespace std;

class Bank {
public:
    Bank(const string &dataFile, const string &logFile, const string &masterPassword);

    bool load();   // decrypt & load accounts and transactions
    bool save();   // serialize & encrypt accounts and transactions

    BankAccount* createAccount(const string &holderName, double initDeposit);
    BankAccount* findAccount(int accountNumber);
    bool deleteAccount(int accountNumber);

    bool deposit(int accountNumber, double amount);
    bool withdraw(int accountNumber, double amount);

    // Log transaction: append to log file (encrypted on disk)
    bool logTransaction(const Transaction &tr);

    const vector<BankAccount>& getAllAccounts() const;

private:
    vector<BankAccount> accounts;
    string dataFilePath; // encrypted file path
    string logFilePath;  // encrypted transaction log
    string tempPlainData; // temp plaintext filename
    string tempPlainLog;  // temp plaintext log filename
    string masterPwd;

    mutex mtx;

    int nextAccountNumber();
    bool loadPlainData(const string &plainPath);
    bool savePlainData(const string &plainPath);
    bool loadPlainLog(const string &plainPath);
    bool savePlainLog(const string &plainPath);
};
