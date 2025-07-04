#include "Huffman.h"
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include<functional>
using namespace std;

// Node for Huffman tree
struct Node {
    uint8_t byte;
    size_t freq;
    Node *left, *right;
    Node(uint8_t b, size_t f): byte(b), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r): byte(0), freq(l->freq + r->freq), left(l), right(r) {}
};

struct NodeCmp {
    bool operator()(Node* a, Node* b) const {
        return a->freq > b->freq;
    }
};

static void buildCodes(Node* root, vector<string> &codes, string &prefix) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->byte] = prefix;
    } else {
        prefix.push_back('0');
        buildCodes(root->left, codes, prefix);
        prefix.pop_back();
        prefix.push_back('1');
        buildCodes(root->right, codes, prefix);
        prefix.pop_back();
    }
}

static void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// Write bits to output stream, buffering into bytes
class BitWriter {
    ofstream &out;
    uint8_t buffer;
    int bitCount;
public:
    BitWriter(ofstream &o): out(o), buffer(0), bitCount(0) {}
    void writeBit(int b) {
        buffer = (buffer << 1) | (b & 1);
        bitCount++;
        if (bitCount == 8) {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
    void writeCode(const string &code) {
        for (char c: code) writeBit(c == '1' ? 1 : 0);
    }
    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
};

// Read bits from input stream
class BitReader {
    ifstream &in;
    uint8_t buffer;
    int bitCount;
public:
    BitReader(ifstream &i): in(i), buffer(0), bitCount(0) {}
    // Return -1 on EOF
    int readBit() {
        if (bitCount == 0) {
            int c = in.get();
            if (c == EOF) return -1;
            buffer = static_cast<uint8_t>(c);
            bitCount = 8;
        }
        int b = (buffer >> 7) & 1;
        buffer <<= 1;
        bitCount--;
        return b;
    }
};

bool Huffman::compressFile(const string &inputPath, const string &outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) return false;
    // Frequency map
    vector<size_t> freq(256, 0);
    char c;
    while (in.get(c)) {
        uint8_t b = static_cast<uint8_t>(c);
        freq[b]++;
    }
    in.clear();
    in.seekg(0);

    // Build min-heap
    priority_queue<Node*, vector<Node*>, NodeCmp> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), freq[i]));
        }
    }
    if (pq.empty()) return false;
    // Edge case: only one unique byte
    if (pq.size() == 1) {
        Node* only = pq.top(); pq.pop();
        Node* root = new Node(only, new Node(only->byte, 0));
        pq.push(root);
    }
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        Node* parent = new Node(a, b);
        pq.push(parent);
    }
    Node* root = pq.top();

    // Build codes
    vector<string> codes(256);
    string prefix;
    buildCodes(root, codes, prefix);

    ofstream out(outputPath, ios::binary);
    if (!out) { deleteTree(root); return false; }
    // Serialize tree: preorder. Use '1' + byte for leaf, '0' for internal.
    function<void(Node*)> writeTree = [&](Node* node) {
        if (!node) return;
        if (!node->left && !node->right) {
            out.put(char(1));
            out.put(static_cast<char>(node->byte));
        } else {
            out.put(char(0));
            writeTree(node->left);
            writeTree(node->right);
        }
    };
    writeTree(root);

    // Write a marker to separate tree and data, e.g., one zero byte
    out.put(char(2));

    // Write compressed data
    BitWriter writer(out);
    while (in.get(c)) {
        uint8_t b = static_cast<uint8_t>(c);
        writer.writeCode(codes[b]);
    }
    writer.flush();
    deleteTree(root);
    return true;
}

bool Huffman::decompressFile(const string &inputPath, const string &outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) return false;

    // Rebuild tree
    function<Node*()> readTree = [&]() -> Node* {
        int flag = in.get();
        if (flag == EOF) return nullptr;
        if (flag == 1) {
            int byte = in.get();
            if (byte == EOF) return nullptr;
            return new Node(static_cast<uint8_t>(byte), 0);
        } else if (flag == 0) {
            Node* left = readTree();
            Node* right = readTree();
            return new Node(left, right);
        } else {
            // Unexpected
            return nullptr;
        }
    };
    Node* root = readTree();
    // Read separator
    in.get(); // assume the marker

    ofstream out(outputPath, ios::binary);
    if (!out) { deleteTree(root); return false; }

    BitReader reader(in);
    Node* node = root;
    while (true) {
        int bit = reader.readBit();
        if (bit < 0) break;
        if (bit == 0) {
            node = node->left;
        } else {
            node = node->right;
        }
        if (!node->left && !node->right) {
            out.put(static_cast<char>(node->byte));
            node = root;
        }
    }
    deleteTree(root);
    return true;
}
