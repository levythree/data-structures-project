#include <stdio.h>
#include <stdlib.h>

#include "huffman.h"

Huffman* createHuffmanNode(unsigned char character, int frequency) {
    Huffman* huffman = (Huffman*) malloc(sizeof(Huffman));

    huffman->character = character;
    huffman->frequency = frequency;
    huffman->left = NULL;
    huffman->right = NULL;

    return huffman;
}

Huffman* createHuffmanTree(PriorityQueue* priorityQueue) {
    while (priorityQueue->head != NULL && priorityQueue->head->next != NULL) {
        Huffman* leftChild = (Huffman*) dequeue(priorityQueue);
        Huffman* rightChild = (Huffman*) dequeue(priorityQueue);

        Huffman* parent = createHuffmanNode('*', leftChild->frequency + rightChild->frequency);

        parent->left = leftChild;
        parent->right = rightChild;

        enqueue(priorityQueue, parent);
    }

    return (Huffman*) dequeue(priorityQueue);
}

void printPreOrderFrequencies(Huffman* huffman) {
    if (huffman != NULL) {
        printf("%d ", huffman->frequency);
        printPreOrderFrequencies(huffman->left);
        printPreOrderFrequencies(huffman->right);
    }
}

void printPreOrderCharacters(Huffman* huffman) {
    if (huffman != NULL) {
        printf("%c ", huffman->character);
        printPreOrderCharacters(huffman->left);
        printPreOrderCharacters(huffman->right);
    }
}
