#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarySearchTree.h"
#include "avl.h"

BinarySearchTree* fillRandomly(BinarySearchTree* root) {
    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) {
        pool[i] = i + 1;
    }

    for (i = 0; i < 30000; i++) {
        int randomIndex = rand() % poolSize;

        root = add(root, pool[randomIndex]);

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);

    return root;
}

BinarySearchTree* fillInOrder(BinarySearchTree* root)  {
    int i;

    for (i = 0; i < 30000; i++) root = add(root, i + 1);

    return root;
}

AVL* fillAVL(AVL* root) {
    int i;

    for (i = 0; i < 30000; i++) root = addAVL(root, i + 1);

    return root;
}

void extensiveSearch(BinarySearchTree* root, AVL* rootAVL) {
    FILE* file = fopen("Dados/data.csv", "w");

    fprintf(file, "Number Searched;Binary Search Tree;AVL\n");

    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) {
        pool[i] = i + 1;
    }

    for (i = 0; i < 10000; i++) {
        int randomIndex = rand() % poolSize;

        int numberOfComparisons = 0;
        int numberOfComparisonsAVL = 0;

        search(root, pool[randomIndex], &numberOfComparisons);
        searchAVL(rootAVL, pool[randomIndex], &numberOfComparisonsAVL);

        fprintf(file, "%d;%d;%d\n", pool[randomIndex], numberOfComparisons, numberOfComparisonsAVL);

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);
}

int main() {
    srand(time(NULL));

    BinarySearchTree* binarySearchTree = NULL;
    AVL* avl = NULL;

    int i, input;
    
    for (i = 0; i < 100; i++) {
        rand();
    }

    printf("[1] - Fill in order\n");
    printf("[2] - Fill randomly\n");
    printf("Choose your option: ");
    scanf("%d", &input);

    if (input == 1) binarySearchTree = fillInOrder(binarySearchTree);
    else if (input == 2) binarySearchTree = fillRandomly(binarySearchTree);

    avl = fillAVL(avl);

    extensiveSearch(binarySearchTree, avl);

    return 0;
}
