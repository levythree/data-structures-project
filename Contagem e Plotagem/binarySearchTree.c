#include <stdio.h>
#include <stdlib.h>

#include "binarySearchTree.h"

BinarySearchTree* createBinarySearchTree(void* data) {
    BinarySearchTree* node = (BinarySearchTree*) malloc(sizeof(BinarySearchTree));

    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

BinarySearchTree* add(BinarySearchTree* root, void* data, int (*compare) (void*, void*)) {
    if (root == NULL) return createBinarySearchTree(data);

    int comparison = compare(data, root->data);

    if (comparison > 0) root->right = add(root->right, data, compare);
    if (comparison < 0) root->left = add(root->left, data, compare);

    return root;
}
