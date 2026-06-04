#ifndef huffmanH
#define huffmanH

#include <stdbool.h>

#include "priorityQueue.h"

typedef struct huffman {
    unsigned char character;
    int frequency;
    struct huffman* left;
    struct huffman* right;
} HuffmanNode;

typedef struct {
    char bits[256];
    int length;
} HuffmanCode;

HuffmanNode* createHuffmanNode(unsigned char character, int frequency);

HuffmanNode* createHuffmanTree(PriorityQueue* priorityQueue);

bool isLeaf(HuffmanNode* node);

int getTreeSize(HuffmanNode* root);

void generateDictionary(HuffmanNode* node, char* path, int depth, HuffmanCode* dictionary);

void freeHuffmanTree(HuffmanNode* root);

#endif
