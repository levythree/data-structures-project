#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "fileHandle.h"
#include "huffman.h"
#include "priorityQueue.h"
#include "compare.h"

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

void writeBit(FILE* file, int bit) {
    static unsigned char buffer = 0;
    static int bitsFilled = 0;

    if (bit == -1) {
        if (bitsFilled > 0) fputc(buffer, file);

        return;
    }

    if (bit == 1) buffer = buffer | (1 << (7 - bitsFilled));

    bitsFilled++;

    if (bitsFilled == 8) {
        fputc(buffer, file);

        buffer = 0;
        bitsFilled = 0; 
    }
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

void compressFile(const char* inputFileName, const char* outputFileName) {
    int* frequencies = (int*) calloc(256, sizeof(int));
    getFrequencies(inputFileName, frequencies);

    PriorityQueue* queue = createPriorityQueue(compareHuffmanNodes);
    fillPriorityQueue(queue, frequencies);

    HuffmanNode* root = createHuffmanTree(queue);

    HuffmanCode* dictionary = (HuffmanCode*) malloc(256 * sizeof(HuffmanCode));

    int i;

    for (i = 0; i < 256; i++) dictionary[i].length = 0;

    char* pathBuffer = (char*) malloc(256 * sizeof(char));

    generateDictionary(root, pathBuffer, 0, dictionary);

    int treeSize = getTreeSize(root);

    long long totalBits = 0;

    for (i = 0; i < 256; i++) if (frequencies[i] > 0) totalBits += frequencies[i] * dictionary[i].length;

    int trashSize = (8 - (totalBits % 8)) % 8;

    uint16_t header = (trashSize << 13) | treeSize;
    unsigned char firstByte = header >> 8;
    unsigned char secondByte = header & 0xFF; // 11111111

    FILE* outputFile = fopen(outputFileName, "wb");

    if (outputFile == NULL) return;

    fputc(firstByte, outputFile);
    fputc(secondByte, outputFile);

    writeTree(outputFile, root);

    FILE* inputFile = fopen(inputFileName, "rb");

    if (inputFile == NULL) return;

    int currentByte;

    while ((currentByte = fgetc(inputFile)) != EOF) {
        for (i = 0; i < dictionary[currentByte].length; i++) {
            int bit = dictionary[currentByte].bits[i] - '0';

            writeBit(outputFile, bit);
        }
    }

    writeBit(outputFile, -1);

    freeHuffmanTree(root);
    fclose(inputFile);
    fclose(outputFile);
}

void decompressFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "rb");

    if (inputFile == NULL) return;

    fseek(inputFile, 0, SEEK_END);

    long fileSize = ftell(inputFile);

    rewind(inputFile);

    int firstByte = fgetc(inputFile);
    int secondByte = fgetc(inputFile);

    if (firstByte == EOF || secondByte == EOF) return;

    uint16_t header = (firstByte << 8) | secondByte;

    int trashSize = header >> 13;
    int treeSize = header & 0x1FFF; // 0001111111111111

    HuffmanNode* root = rebuildTree(inputFile);
    long long compressedDataBytes = fileSize - (2 + treeSize);
    long long bitsToRead = (compressedDataBytes * 8LL) - trashSize;

    FILE* outputFile = fopen(outputFileName, "wb");

    if (outputFile == NULL) return;

    long long i;

    HuffmanNode* currentNode = root;

    for (i = 0; i < bitsToRead; i++) {
        int bit = getNextBit(inputFile);

        if (bit == 0) currentNode = currentNode->left;
        else if (bit == 1) currentNode = currentNode->right;

        if (isLeaf(currentNode)) {
            fputc(currentNode->character, outputFile);

            currentNode = root;
        }
    }

    freeHuffmanTree(root);
    fclose(inputFile);
    fclose(outputFile);
}
