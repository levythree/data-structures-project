#ifndef huffmanH
#define huffmanH

#include "priorityQueue.h"

typedef struct huffman {
    unsigned char character;
    int frequency;
    struct huffman* left;
    struct huffman* right;
} Huffman;

Huffman* createHuffmanNode(unsigned char character, int frequency);

Huffman* createHuffmanTree(PriorityQueue* priorityQueue);

void printPreOrderFrequencies(Huffman* huffman);

void printPreOrderCharacters(Huffman* huffman);

#endif
