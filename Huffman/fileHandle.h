#ifndef fileHandleH
#define fileHandleH

#include "huffman.h"
#include "priorityQueue.h"

#include <stdlib.h>
#include <stdio.h>

int getNextBit(FILE* file);

void writeBit(FILE* file, int bit);

void writeTree(FILE* file, HuffmanNode* root);

HuffmanNode* rebuildTree(FILE* file);

void getFrequencies(const char* fileName, int* frequencies);

void fillPriorityQueue(PriorityQueue* priorityQueue, int* frequencies);

void compressFile(const char* inputFileName, const char* outputFileName);

void decompressFile(const char* inputFileName, const char* outputFileName);

#endif
