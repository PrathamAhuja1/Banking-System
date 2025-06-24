// PasswordManager.cpp
#include "PasswordManager.h"
#include "../crypto/CryptoUtils.h"
#include <fstream>
#include <filesystem>
#include <sstream>

std::string VaultEntry::serialize() const {
    return service + "|" + username + "|" + password;
}
VaultEntry VaultEntry::deserialize(const std::string &line) {
    std::istringstream iss(line);
    std::string s, u, p;
    if (!std::getline(iss, s, '|')) return {};
    if (!std::getline(iss, u, '|')) return {};
    if (!std::getline(iss, p, '|')) return {};
    return VaultEntry{s,u,p};
}

PasswordManager::PasswordManager(const std::string &vaultFile, const std::string &masterPassword)
    : vaultFilePath(vaultFile), tempPlainVault("temp_vault.txt"), masterPwd(masterPassword) {}

bool PasswordManager::load() {
    std::lock_guard<std::mutex> lk(mtx);
    entries.clear();
    if (std::filesystem::exists(vaultFilePath)) {
        if (!CryptoUtils::decryptFile(vaultFilePath, tempPlainVault, masterPwd)) return false;
        std::ifstream in(tempPlainVault);
        if (!in) return false;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            entries.push_back(VaultEntry::deserialize(line));
        }
        in.close();
        std::filesystem::remove(tempPlainVault);
    }
    return true;
}

bool PasswordManager::save() {
    std::lock_guard<std::mutex> lk(mtx);
    std::ofstream out(tempPlainVault, std::ios::trunc);
    if (!out) return false;
    for (auto &e : entries) {
        out << e.serialize() << "\n";
    }
    out.close();
    if (!CryptoUtils::encryptFile(tempPlainVault, vaultFilePath, masterPwd)) return false;
    std::filesystem::remove(tempPlainVault);
    return true;
}

std::vector<VaultEntry> PasswordManager::listEntries() {
    std::lock_guard<std::mutex> lk(mtx);
    return entries;
}

bool PasswordManager::addEntry(const std::string &service, const std::string &username, const std::string &password) {
    std::lock_guard<std::mutex> lk(mtx);
    // If duplicate service, reject or overwrite? Here we reject duplicates.
    for (auto &e: entries) {
        if (e.service == service) return false;
    }
    entries.push_back(VaultEntry{service, username, password});
    return save();
}

bool PasswordManager::deleteEntry(const std::string &service) {
    std::lock_guard<std::mutex> lk(mtx);
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->service == service) {
            entries.erase(it);
            return save();
        }
    }
    return false;
}
