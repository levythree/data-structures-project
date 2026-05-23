#include <stdio.h>
#include <stdlib.h>

#include "binarySearchTree.h"

BinarySearchTree* createBinarySearchTree(int value) {
    BinarySearchTree* node = (BinarySearchTree*) malloc(sizeof(BinarySearchTree));

    node->value = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

BinarySearchTree* add(BinarySearchTree* root, int value) {
    if (root == NULL) return createBinarySearchTree(value);

    if (value > root->value) root->right = add(root->right, value);
    if (value < root->value) root->left = add(root->left, value);

    return root;
}

void search(BinarySearchTree* root, int value, int* numberOfComparisons) {
    if (root == NULL) return;

    (*numberOfComparisons)++;

    if (value == root->value) return;
    else if (value > root->value) search(root->right, value, numberOfComparisons);
    else search(root->left, value, numberOfComparisons);
}
