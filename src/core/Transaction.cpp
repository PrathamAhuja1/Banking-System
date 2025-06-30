#include "Transaction.h"
#include <sstream>
#include <iomanip>
using namespace std;

string Transaction::serialize() const {
    ostringstream oss;
    oss << timestamp << "|" << type << "|" << amount << "|" << relatedAccount;
    return oss.str();
}

Transaction Transaction::deserialize(const string &line) {
    istringstream iss(line);
    string ts, t, amt_s, rel_s;
    
    if (!getline(iss, ts, '|')) {
        return Transaction();
    }
    if (!getline(iss, t, '|')) {
        return Transaction();
    }
    if (!getline(iss, amt_s, '|')) {
        return Transaction();
    }
    if (!getline(iss, rel_s, '|')) {
        rel_s = "-1";
    }
    
    Transaction tr;
    tr.timestamp = ts;
    tr.type = t;
    tr.amount = stod(amt_s);
    tr.relatedAccount = stoi(rel_s);
    return tr;
}