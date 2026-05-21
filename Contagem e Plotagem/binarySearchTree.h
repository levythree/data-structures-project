#ifndef binarySearchTreeH
#define binarySearchTreeH

typedef struct binarySearchTree {
    void* data;
    struct binarySearchTree *left;
    struct binarySearchTree *right;
} BinarySearchTree;

BinarySearchTree* createBinarySearchTree(void* data);

BinarySearchTree* add(BinarySearchTree* root, void* data, int (*compare) (void*, void*));

#endif
