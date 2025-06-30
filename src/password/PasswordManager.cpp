#include "PasswordManager.h"
#include "../crypto/CryptoUtils.h"
#include <fstream>
#include <filesystem>
#include <sstream>
using namespace std;

string VaultEntry::serialize() const {
    return service + "|" + username + "|" + password;
}
VaultEntry VaultEntry::deserialize(const string &line) {
    istringstream iss(line);
    string s, u, p;
    if (!getline(iss, s, '|')) return {};
    if (!getline(iss, u, '|')) return {};
    if (!getline(iss, p, '|')) return {};
    return VaultEntry{s,u,p};
}
PasswordManager::PasswordManager(const string &vaultFile, const string &masterPassword){

    vaultFilePath=vaultFile;
    tempPlainVault="temp_vault.txt";
    masterPwd=masterPassword;
}

bool PasswordManager::load() {
    lock_guard<mutex> lk(mtx);
    entries.clear();
    if (filesystem::exists(vaultFilePath)) {
        if (!CryptoUtils::decryptFile(vaultFilePath, tempPlainVault, masterPwd)) return false;
        ifstream in(tempPlainVault);
        if (!in) return false;
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            entries.push_back(VaultEntry::deserialize(line));
        }
        in.close();
        filesystem::remove(tempPlainVault);
    }
    return true;
}

bool PasswordManager::save() {
    lock_guard<mutex> lk(mtx);
    ofstream out(tempPlainVault, ios::trunc);
    if (!out) return false;
    for (auto &e : entries) {
        out << e.serialize() << "\n";
    }
    out.close();
    if (!CryptoUtils::encryptFile(tempPlainVault, vaultFilePath, masterPwd)) return false;
    filesystem::remove(tempPlainVault);
    return true;
}

vector<VaultEntry> PasswordManager::listEntries() {
    lock_guard<mutex> lk(mtx);
    return entries;
}

bool PasswordManager::addEntry(const string &service, const string &username, const string &password) {
    lock_guard<mutex> lk(mtx);
    // If duplicate service, reject or overwrite? Here we reject duplicates.
    for (auto &e: entries) {
        if (e.service == service) return false;
    }
    entries.push_back(VaultEntry{service, username, password});
    return save();
}

bool PasswordManager::deleteEntry(const string &service) {
    lock_guard<mutex> lk(mtx);
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->service == service) {
            entries.erase(it);
            return save();
        }
    }
    return false;
}
