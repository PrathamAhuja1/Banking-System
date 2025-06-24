#include "Transaction.h"
#include <sstream>
#include <iomanip>
using namespace std;

static string currentTimestamp() {
    auto now = chrono::system_clock::now();
    auto t_c = chrono::system_clock::to_time_t(now);
    tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t_c);
#else
    localtime_r(&t_c, &tm);
#endif
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

string Transaction::serialize() const {
    ostringstream oss;
    oss << timestamp << "|" << type << "|" << amount << "|" << relatedAccount;
    return oss.str();
}

Transaction Transaction::deserialize(const string &line) {
    istringstream iss(line);
    string ts, t, amt_s, rel_s;
    if (!getline(iss, ts, '|')) return {};
    if (!getline(iss, t, '|')) return {};
    if (!getline(iss, amt_s, '|')) return {};
    if (!getline(iss, rel_s, '|')) rel_s = "-1";
    Transaction tr;
    tr.timestamp = ts;
    tr.type = t;
    tr.amount = stod(amt_s);
    tr.relatedAccount = stoi(rel_s);
    return tr;
}
