#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "sat.h"

int main(int numberOfArguments, char** arguments) {
    if (numberOfArguments != 2) {
        printf("Usage: ./main <fileName>\n");

        return 1;
    }

    Formula* formula = readCnf(arguments[1]);

    int* interpretation = (int*) calloc((formula->numberOfVariables + 1), sizeof(int));

    SATNode* satNode = NULL;

    if (sat(formula, interpretation, 1, &satNode)) {
        printf("SAT!\n\n");

        printSATTree(satNode);
    } else printf("UNSAT!\n");

    return 0;
}
