#include "Bank.h"
#include "BankAccount.cpp"
#include "../crypto/CryptoUtils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

using namespace std;

Bank::Bank(const string &dataFile, const string &logFile, const string &masterPassword){

    dataFilePath=dataFile;
    logFilePath=logFile;
    tempPlainData="temp_accounts.txt";
    tempPlainLog="temp_transactions.txt";
    masterPwd=masterPassword;

}

int Bank::nextAccountNumber() {
    int maxNo = 1000;
    for (auto &acc : accounts) {
        if (acc.getAccountNumber() >= maxNo) maxNo = acc.getAccountNumber() + 1;
    }
    return maxNo;
}

bool Bank::load() {
    lock_guard<mutex> lk(mtx);
    accounts.clear();
    // Decrypt dataFilePath to tempPlainData if exists
    if (filesystem::exists(dataFilePath)) {
        if (!CryptoUtils::decryptFile(dataFilePath, tempPlainData, masterPwd)) return false;
        if (!loadPlainData(tempPlainData)) return false;
        filesystem::remove(tempPlainData);
    }
    // Load transactions if needed: we keep log encrypted on disk; for appending, decrypt to temp each time
    return true;
}

bool Bank::save() {
    lock_guard<mutex> lk(mtx);
    // Serialize accounts to tempPlainData
    if (!savePlainData(tempPlainData)) return false;
    // Encrypt to dataFilePath
    if (!CryptoUtils::encryptFile(tempPlainData, dataFilePath, masterPwd)) return false;
    filesystem::remove(tempPlainData);
    // For log: we assume log file is appended separately in logTransaction
    return true;
}

bool Bank::loadPlainData(const string &plainPath) {
    ifstream in(plainPath);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        BankAccount acc = BankAccount::deserialize(line);
        // Optionally load transactions per account from the log file
        accounts.push_back(acc);
    }
    return true;
}

bool Bank::savePlainData(const string &plainPath) {
    ofstream out(plainPath, ios::trunc);
    if (!out) return false;
    for (auto &acc : accounts) {
        out << acc.serialize() << "\n";
    }
    return true;
}

bool Bank::loadPlainLog(const string &plainPath) {
    // Not used for in-memory; logs appended directly
    return true;
}

bool Bank::savePlainLog(const string &plainPath) {
    // Not used
    return true;
}

BankAccount* Bank::createAccount(const string &holderName, double initDeposit) {
    int accNo = nextAccountNumber();
    BankAccount acc(accNo, holderName, initDeposit);
    accounts.push_back(acc);
    if (initDeposit > 0) {
        string now_time = getCurrentIsoTimestamp();
        Transaction tr{ Transaction().timestamp=now_time, "Deposit", initDeposit, -1 };
        logTransaction(tr);
    }
    return &accounts.back();
}

BankAccount* Bank::findAccount(int accountNumber) {
    for (auto &acc : accounts) {
        if (acc.getAccountNumber() == accountNumber) return &acc;
    }
    return nullptr;
}

bool Bank::deleteAccount(int accountNumber) {
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->getAccountNumber() == accountNumber) {
            accounts.erase(it);
            return true;
        }
    }
    return false;
}

bool Bank::deposit(int accountNumber, double amount) {
    lock_guard<mutex> lk(mtx);
    BankAccount* acc = findAccount(accountNumber);
    if (!acc) return false;
    if (!acc->deposit(amount)) return false;
    Transaction tr{ Transaction().timestamp, "Deposit", amount, accountNumber };
    return logTransaction(tr);
}

bool Bank::withdraw(int accountNumber, double amount) {
    lock_guard<mutex> lk(mtx);
    BankAccount* acc = findAccount(accountNumber);
    if (!acc) return false;
    if (!acc->withdraw(amount)) return false;
    Transaction tr{ Transaction().timestamp, "Withdraw", amount, accountNumber };
    return logTransaction(tr);
}

bool Bank::logTransaction(const Transaction &tr) {
    // Decrypt existing log to temp, append, then encrypt back
    lock_guard<mutex> lk(mtx);
    if (filesystem::exists(logFilePath)) {
        if (!CryptoUtils::decryptFile(logFilePath, tempPlainLog, masterPwd)) return false;
    } else {
        // create empty
        ofstream tmp(tempPlainLog);
    }
    ofstream out(tempPlainLog, ios::app);
    if (!out) return false;
    out << tr.serialize() << "\n";
    out.close();
    if (!CryptoUtils::encryptFile(tempPlainLog, logFilePath, masterPwd)) return false;
    filesystem::remove(tempPlainLog);
    return true;
}

const vector<BankAccount>& Bank::getAllAccounts() const {
    return accounts;
}
