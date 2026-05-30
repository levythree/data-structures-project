#include <stdlib.h>
#include <stdio.h>

#include "fileHandle.h"
#include "huffman.h"
#include "priorityQueue.h"

int getNextBit(FILE* file) {
    static unsigned char buffer = 0;
    static int bitsLeft = 0;

    if (bitsLeft == 0) {
        int byte = fgetc(file);

        if (byte == EOF) return -1;

        buffer = (unsigned char) byte;

        bitsLeft = 8;
    }

    bitsLeft--;

    unsigned char mask = 1 << bitsLeft;

    return (buffer & mask) != 0;
}

void writeTree(FILE* file, HuffmanNode* node) {
    if (node != NULL) {
        if (isLeaf(node) && (node->character == '*' || node->character == '\\')) {
            fputc('\\', file);
            fputc(node->character, file);
        } else fputc(node->character, file);

        writeTree(file, node->left);
        writeTree(file, node->right);
    }
}

HuffmanNode* rebuildTree(FILE* file) {
    int byte = fgetc(file);

    if (byte == EOF) return NULL;

    if (byte == '*') {
        HuffmanNode* node = createHuffmanNode('*', 0);

        node->left = rebuildTree(file);
        node->right = rebuildTree(file);    

        return node;
    } else {
        if (byte == '\\') byte = fgetc(file);

        HuffmanNode* node = createHuffmanNode((unsigned char) byte, 0);

        return node;
    }
}

void getFrequencies(const char* fileName, int* frequencies) {
    FILE* file = fopen(fileName, "rb");

    if (file == NULL) return;

    int byte;

    while ((byte = fgetc(file)) != EOF) frequencies[byte]++;

    fclose(file);
}

void fillPriorityQueue(PriorityQueue* priorityQueue, int* frequencies) {
    int i;

    for (i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode* node = createHuffmanNode(i, frequencies[i]);

            enqueue(priorityQueue, node);
        }
    }
}
