#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

void search(BinarySearchTree* root, void* data, int (*compare) (void*, void*), int* numberOfComparisons) {
    if (root == NULL) return;

    (*numberOfComparisons)++;

    int comparison = compare(data, root->data);

    if (comparison == 0) return;
    else if (comparison > 0) return search(root->right, data, compare, numberOfComparisons);
    else return search(root->left, data, compare, numberOfComparisons);
}
