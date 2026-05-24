#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "sat.h"

int main() {
    Formula* formula = readCnf("formula.cnf");

    int* interpretation = (int*) calloc((formula->numberOfVariables + 1), sizeof(int));

    if (sat(formula, interpretation, 1)) {
        printf("SAT!\n");

        printInterpretation(formula, interpretation);
    } else printf("UNSAT!\n");

    return 0;
}
