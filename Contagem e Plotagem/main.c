#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "binarySearchTree.h"
#include "comparisons.h"

int main() {
    BinarySearchTree* binarySearchTree = NULL;

    int i;

    for (i = 1; i <= 10000; i++) {
        int* newNumber = malloc(sizeof(int));

        *newNumber = i;

        binarySearchTree = add(binarySearchTree, newNumber, compareInt);
    }

    printf("%d", *(int*) binarySearchTree->data);

    return 0;
}
