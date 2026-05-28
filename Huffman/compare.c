#include "compare.h"

#include "huffman.h"

int compareHuffmanNodes(void* data1, void* data2) {
    Huffman* huffman1 = (Huffman*) data1;
    Huffman* huffman2 = (Huffman*) data2;

    return huffman1->frequency - huffman2->frequency;
}
