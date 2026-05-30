#ifndef fileHandleH
#define fileHandleH

#include "huffman.h"
#include "priorityQueue.h"

#include <stdlib.h>
#include <stdio.h>

int getNextBit(FILE* file);

void writeTree(FILE* file, HuffmanNode* root);

HuffmanNode* rebuildTree(FILE* file);

void getFrequencies(const char* fileName, int* frequencies);

void fillPriorityQueue(PriorityQueue* priorityQueue, int* frequencies);

#endif
