#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarySearchTree.h"
#include "avl.h"

BinarySearchTree* fillBinarySearchTree() {
    BinarySearchTree* binarySearchTree = NULL;

    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) {
        pool[i] = i + 1;
    }

    for (i = 0; i < 30000; i++) {
        int randomIndex = rand() % poolSize;

        binarySearchTree = add(binarySearchTree, pool[randomIndex]);

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);

    return binarySearchTree;
}

AVL* fillAVL() {
    AVL* avl = NULL;

    int i;

    for (i = 0; i < 30000; i++) avl = addAVL(avl, i + 1);

    return avl;
}

void extensiveSearch(BinarySearchTree* binarySearchTree, AVL* avl) {
    FILE* file = fopen("Dados/data.csv", "w");

    fprintf(file, "Number Searched;Binary Search Tree;AVL\n");

    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) pool[i] = i + 1;

    for (i = 0; i < 30000; i++) {
        int randomIndex = rand() % poolSize;

        fprintf(file, "%d;%d;%d\n", pool[randomIndex], search(binarySearchTree, pool[randomIndex]), searchAVL(avl, pool[randomIndex]));

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);
}

int main() {
    srand(time(NULL));

    int i;
    
    for (i = 0; i < 100; i++) {
        rand();
    }

    BinarySearchTree* binarySearchTree = fillBinarySearchTree();
    AVL* avl = fillAVL();

    extensiveSearch(binarySearchTree, avl);

    return 0;
}
