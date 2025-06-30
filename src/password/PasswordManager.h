#pragma once
#include <string>
#include <vector>
#include <mutex>
using namespace std;

struct VaultEntry {
    string service;
    string username;
    string password;
    string serialize() const;
    static VaultEntry deserialize(const string &line);
};

class PasswordManager {
public:
    PasswordManager(const string &vaultFile, const string &masterPassword);

    bool load();   // decrypt vault to memory
    bool save();   // encrypt vault to disk

    vector<VaultEntry> listEntries();
    bool addEntry(const string &service, const string &username, const string &password);
    bool deleteEntry(const string &service);

private:
    vector<VaultEntry> entries;
    string vaultFilePath;
    string tempPlainVault;
    string masterPwd;
    mutex mtx;
};
