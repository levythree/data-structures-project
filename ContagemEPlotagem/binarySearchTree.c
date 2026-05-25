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

int search(BinarySearchTree* root, int value) {
    if (root == NULL) return 1;
    
    if (value == root->value) return 1;
    else if (value > root->value) return search(root->right, value) + 1;
    else return search(root->left, value) + 1;
}
