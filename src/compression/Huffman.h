// Huffman.h
#pragma once
#include <string>
using namespace std;

class Huffman {

    public:
     static bool compressFile(const string &inputPath, const string &outputPath);
        static bool decompressFile(const string &inputPath, const string &outputPath);

};
