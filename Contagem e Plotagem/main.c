#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarySearchTree.h"
#include "comparisons.h"

BinarySearchTree* fillRandomly(BinarySearchTree* root) {
    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) {
        pool[i] = i + 1;
    }

    for (i = 0; i < 30000; i++) {
        int randomIndex = rand() % poolSize;

        int* number = malloc(sizeof(int));

        *number = pool[randomIndex];

        root = add(root, (void*) number, compareInt);

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);

    return root;
}

BinarySearchTree* fillInOrder(BinarySearchTree* root)  {
    int i;

    for (i = 0; i < 30000; i++) {
        int* number = malloc(sizeof(int));

        *number = i + 1;

        root = add(root, (void*) number, compareInt);
    }

    return root;
}

void extensiveSearch(BinarySearchTree* root) {
    FILE* file = fopen("Dados/data.csv", "w");

    fprintf(file, "Number Searched;Comparisons\n");

    int poolSize = 30000, i;

    int* pool = malloc(poolSize * sizeof(int));

    for (i = 0; i < poolSize; i++) {
        pool[i] = i + 1;
    }

    for (i = 0; i < 10000; i++) {
        int randomIndex = rand() % poolSize;

        int numberOfComparisons = 0;

        search(root, (void*) &pool[randomIndex], compareInt, &numberOfComparisons);

        fprintf(file, "%d;%d\n", pool[randomIndex], numberOfComparisons);

        pool[randomIndex] = pool[--poolSize];
    }

    free(pool);
}

int main() {
    srand(time(NULL));

    BinarySearchTree* binarySearchTree = NULL;

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

    extensiveSearch(binarySearchTree);

    return 0;
}
