// BankAccount.cpp
#include "BankAccount.h"
#include <sstream>

BankAccount::BankAccount(int accNo, const std::string &holder, double initBalance)
    : accountNumber(accNo), holderName(holder), balance(initBalance) {
    // Record initial deposit if >0
    if (initBalance > 0.0) {
        transactions.emplace_back(Transaction{
            /*timestamp*/ Transaction().timestamp = Transaction().timestamp, 
            /*type*/ "Deposit", initBalance, -1
        });
    }
}

int BankAccount::getAccountNumber() const { return accountNumber; }
const std::string& BankAccount::getHolderName() const { return holderName; }
double BankAccount::getBalance() const { return balance; }
const std::vector<Transaction>& BankAccount::getTransactions() const { return transactions; }

bool BankAccount::deposit(double amount) {
    if (amount <= 0) return false;
    balance += amount;
    transactions.emplace_back(Transaction{ /*timestamp*/ Transaction().timestamp, "Deposit", amount, -1 });
    return true;
}

bool BankAccount::withdraw(double amount) {
    if (amount <= 0 || amount > balance) return false;
    balance -= amount;
    transactions.emplace_back(Transaction{ Transaction().timestamp, "Withdraw", amount, -1 });
    return true;
}

void BankAccount::addTransaction(const Transaction &tr) {
    transactions.push_back(tr);
    // Update balance if relevant
    if (tr.type == "Deposit") balance += tr.amount;
    else if (tr.type == "Withdraw") balance -= tr.amount;
    // Transfers handled in Bank
}

std::string BankAccount::serialize() const {
    std::ostringstream oss;
    // Escape holderName if needed (not done here)
    oss << accountNumber << "|" << holderName << "|" << balance;
    return oss.str();
}

BankAccount BankAccount::deserialize(const std::string &line) {
    std::istringstream iss(line);
    std::string acc_s, holder, bal_s;
    if (!std::getline(iss, acc_s, '|')) return {};
    if (!std::getline(iss, holder, '|')) return {};
    if (!std::getline(iss, bal_s, '|')) return {};
    BankAccount acc;
    acc.accountNumber = std::stoi(acc_s);
    acc.holderName = holder;
    acc.balance = std::stod(bal_s);
    // transactions loaded separately
    return acc;
}
