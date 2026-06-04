#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "huffman.h"

HuffmanNode* createHuffmanNode(unsigned char character, int frequency) {
    HuffmanNode* huffman = (HuffmanNode*) malloc(sizeof(HuffmanNode));

    huffman->character = character;
    huffman->frequency = frequency;
    huffman->left = NULL;
    huffman->right = NULL;

    return huffman;
}

HuffmanNode* createHuffmanTree(PriorityQueue* priorityQueue) {
    while (priorityQueue->head != NULL && priorityQueue->head->next != NULL) {
        HuffmanNode* leftChild = (HuffmanNode*) dequeue(priorityQueue);
        HuffmanNode* rightChild = (HuffmanNode*) dequeue(priorityQueue);

        HuffmanNode* parent = createHuffmanNode('*', leftChild->frequency + rightChild->frequency);

        parent->left = leftChild;
        parent->right = rightChild;

        enqueue(priorityQueue, parent);
    }

    return (HuffmanNode*) dequeue(priorityQueue);
}

bool isLeaf(HuffmanNode* node) {
    return node->left == NULL && node->right == NULL;
}

int getTreeSize(HuffmanNode* root) {
    if (root == NULL) return 0;

    if (isLeaf(root) && (root->character == '*' || root->character == '\\')) return 2;

    return 1 + getTreeSize(root->left) + getTreeSize(root->right);
}

void generateDictionary(HuffmanNode* node, char* path, int depth, HuffmanCode* dictionary) {
    if (node == NULL) return;

    if (isLeaf(node)) {
        int i;

        for (i = 0; i < depth; i++) dictionary[node->character].bits[i] = path[i];

        dictionary[node->character].bits[depth] = '\0';
        
        dictionary[node->character].length = depth;

        return;
    }

    path[depth] = '0';
    generateDictionary(node->left, path, depth + 1, dictionary);

    path[depth] = '1';
    generateDictionary(node->right, path, depth + 1, dictionary);
}

void freeHuffmanTree(HuffmanNode* root) {
    if (root != NULL) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        free(root);
    }
}
