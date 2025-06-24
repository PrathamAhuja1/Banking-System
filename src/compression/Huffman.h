// Huffman.h
#pragma once
#include <string>

namespace Huffman {

bool compressFile(const std::string &inputPath, const std::string &outputPath);
bool decompressFile(const std::string &inputPath, const std::string &outputPath);

}
