#include "BankAccount.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;


string getCurrentIsoTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#endif
    std::ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}


BankAccount::BankAccount(int accNo, const string &holder, double initBalance){
        accountNumber=accNo;
        holderName=holder;
        balance=initBalance;
    // Record initial deposit if >0
    if (initBalance > 0.0) {
        string now_time = getCurrentIsoTimestamp();
        transactions.emplace_back(
            Transaction{
                Transaction().timestamp = now_time, "Deposit", initBalance, -1
        });
    }
}

int BankAccount::getAccountNumber() const { return accountNumber; }
const string& BankAccount::getHolderName() const { return holderName; }
double BankAccount::getBalance() const { return balance; }
const vector<Transaction>& BankAccount::getTransactions() const { return transactions; }

bool BankAccount::deposit(double amount) {
    if (amount <= 0) return false;
    string now_time = getCurrentIsoTimestamp();
    balance += amount;
    transactions.emplace_back(Transaction{Transaction().timestamp=now_time, "Deposit", amount, -1 });
    return true;
}

bool BankAccount::withdraw(double amount) {
    if (amount <= 0 || amount > balance) return false;
    string now_time = getCurrentIsoTimestamp();
    balance -= amount;
    transactions.emplace_back(Transaction{Transaction().timestamp=now_time, "Withdraw", amount, -1 });
    return true;
}

void BankAccount::addTransaction(const Transaction &tr) {
    transactions.push_back(tr);
    // Update balance if relevant
    if (tr.type == "Deposit") balance += tr.amount;
    else if (tr.type == "Withdraw") balance -= tr.amount;
    // Transfers handled in Bank
}

string BankAccount::serialize() const {
    ostringstream oss;
    oss << accountNumber << "|" << holderName << "|" << balance;
    return oss.str();
}

BankAccount BankAccount::deserialize(const string &line) {
    istringstream iss(line);
    string acc_s, holder, bal_s;
    if (!getline(iss, acc_s, '|')) return {};
    if (!getline(iss, holder, '|')) return {};
    if (!getline(iss, bal_s, '|')) return {};
    BankAccount acc;
    acc.accountNumber = stoi(acc_s);
    acc.holderName = holder;
    acc.balance = stod(bal_s);
    // transactions loaded separately
    return acc;
}
