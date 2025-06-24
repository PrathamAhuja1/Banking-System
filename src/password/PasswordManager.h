// PasswordManager.h
#pragma once
#include <string>
#include <vector>
#include <mutex>

struct VaultEntry {
    std::string service;
    std::string username;
    std::string password;
    std::string serialize() const;
    static VaultEntry deserialize(const std::string &line);
};

class PasswordManager {
public:
    PasswordManager(const std::string &vaultFile, const std::string &masterPassword);

    bool load();   // decrypt vault to memory
    bool save();   // encrypt vault to disk

    std::vector<VaultEntry> listEntries();
    bool addEntry(const std::string &service, const std::string &username, const std::string &password);
    bool deleteEntry(const std::string &service);

private:
    std::vector<VaultEntry> entries;
    std::string vaultFilePath;
    std::string tempPlainVault;
    std::string masterPwd;
    std::mutex mtx;
};
