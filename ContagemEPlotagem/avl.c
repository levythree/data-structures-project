#include <stdio.h>
#include <stdlib.h>

#include "avl.h"

AVL* createAVL(int value) {
    AVL* node = (AVL*) malloc(sizeof(AVL));

    node->value = value;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int getHeight(AVL* node) {
    if (node == NULL) return 0;

    return node->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getBalanceFactor(AVL* node) {
    if (node == NULL) return 0;
    
    return getHeight(node->left) - getHeight(node->right);
}

int getUpdatedHeight(AVL* node) {
    if (node == NULL) return 0;
    
    return 1 + max(getHeight(node->left), getHeight(node->right));
}

AVL* rotateLeft(AVL* node) {
    AVL* subtreeRight = node->right;

    node->right = subtreeRight->left;
    subtreeRight->left = node;

    node->height = getUpdatedHeight(node);
    subtreeRight->height = getUpdatedHeight(subtreeRight);

    return subtreeRight;
}

AVL* rotateRight(AVL* node) {
    AVL* subtreeLeft = node->left;

    node->left = subtreeLeft->right;
    subtreeLeft->right = node;

    node->height = getUpdatedHeight(node);
    subtreeLeft->height = getUpdatedHeight(subtreeLeft);

    return subtreeLeft;
}

AVL* addAVL(AVL* root, int value) {
    if (root == NULL) return createAVL(value);

    if (value < root->value) root->left = addAVL(root->left, value);
    else if (value > root->value) root->right = addAVL(root->right, value);
    else return root;

    root->height = getUpdatedHeight(root);

    int balanceFactor = getBalanceFactor(root);

    if (balanceFactor > 1 && value < root->left->value) return rotateRight(root);
    if (balanceFactor < -1 && value > root->right->value) return rotateLeft(root);

    if (balanceFactor > 1 && value > root->left->value) {
        root->left = rotateLeft(root->left);
        
        return rotateRight(root);
    }

    if (balanceFactor < -1 && value < root->right->value) {
        root->right = rotateRight(root->right);

        return rotateLeft(root);
    }

    return root;
}

int searchAVL(AVL* root, int value) {
    if (root == NULL) return 1;

    if (value == root->value) return 1;
    else if (value < root->value) return searchAVL(root->left, value) + 1;
    else return searchAVL(root->right, value) + 1;
}
