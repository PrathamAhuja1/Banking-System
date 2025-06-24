#pragma once
#include <string>

namespace CryptoUtils {

bool encryptFile(const std::string &inPath, const std::string &outPath, const std::string &password);

bool decryptFile(const std::string &inPath, const std::string &outPath, const std::string &password);

}
