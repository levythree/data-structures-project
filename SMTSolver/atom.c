#include "types/atom.h"

#include <stdlib.h>

LinearAtom createLinearAtom(int numberOfVariables, double* parsedCoefficients, double parsedRHS) {
    LinearAtom linearAtom;

    linearAtom.numberOfVariables = numberOfVariables;
    linearAtom.rhs = parsedRHS;

    linearAtom.coefficients = (double*)calloc(numberOfVariables, sizeof(double));

    for (int i = 0; i < numberOfVariables; i++) {
        linearAtom.coefficients[i] = parsedCoefficients[i];
    }

    return linearAtom;
}

void freeLinearAtom(LinearAtom* linearAtom) {
    if (linearAtom->coefficients != NULL) {
        free(linearAtom->coefficients);

        linearAtom->coefficients = NULL;
    }
}
