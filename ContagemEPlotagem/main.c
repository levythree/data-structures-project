#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarySearchTree.h"
#include "avl.h"
#include "sort.h"

#define MAX 32767

BinarySearchTree* fillBinarySearchTree() {
    BinarySearchTree* binarySearchTree = NULL;

    int poolSize = MAX + 1, i;

    int* pool = (int*) malloc(poolSize * sizeof(int));

    for (i = 0; i <= poolSize; i++) {
        pool[i] = i;
    }

    for (i = 0; i <= MAX; i++) {
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

    for (i = 0; i <= MAX; i++) avl = addAVL(avl, i);

    return avl;
}

void extensiveSearch(BinarySearchTree* binarySearchTree, AVL* avl) {
    FILE* file = fopen("Dados/data.csv", "w");

    fprintf(file, "Number Searched;Binary Search Tree;AVL\n");

    int* sortedBinarySearchTree = (int*) malloc((MAX + 1) * sizeof(int));
    int* sortedAVL = (int*) malloc((MAX + 1) * sizeof(int));

    int i;

    for (i = 0; i <= MAX; i++) {
        sortedBinarySearchTree[i] = search(binarySearchTree, i);
        sortedAVL[i] = searchAVL(avl, i);
    }

    quickSort(sortedBinarySearchTree, MAX + 1);
    quickSort(sortedAVL, MAX + 1);

    for (i = 0; i <= MAX; i++) fprintf(file, "%d;%d;%d\n", i, sortedBinarySearchTree[i], sortedAVL[i]);
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
