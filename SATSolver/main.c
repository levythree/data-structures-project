#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "sat.h"

int main(int numberOfArguments, char** arguments) {
    Formula* formula = readCnf(arguments[1]);

    int* interpretation = (int*) calloc((formula->numberOfVariables + 1), sizeof(int));

    SATNode* satNode = NULL;

    if (sat(formula, interpretation, 1, &satNode)) {
        printf("SAT!\n");

        printSATTree(satNode);
    } else printf("UNSAT!\n");

    return 0;
}
