#ifndef binarySearchTreeH
#define binarySearchTreeH

typedef struct binarySearchTree {
    int value;
    struct binarySearchTree *left;
    struct binarySearchTree *right;
} BinarySearchTree;

BinarySearchTree* createBinarySearchTree(int value);

BinarySearchTree* add(BinarySearchTree* root, int value);

int search(BinarySearchTree* root, int value);

#endif
