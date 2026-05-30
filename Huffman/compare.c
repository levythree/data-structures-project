#include "compare.h"

#include "huffman.h"

int compareHuffmanNodes(void* data1, void* data2) {
    HuffmanNode* huffman1 = (HuffmanNode*) data1;
    HuffmanNode* huffman2 = (HuffmanNode*) data2;

    return huffman1->frequency - huffman2->frequency;
}
