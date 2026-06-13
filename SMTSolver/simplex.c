#include "./types/simplex.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 1e-6

Tableau* createTableau(int variables, int equations) {
    Tableau* tableau = (Tableau*)malloc(sizeof(Tableau));

    tableau->variables = variables;
    tableau->equations = equations;
    tableau->rows = equations + 1;
    tableau->columns = variables + equations + 1;
    tableau->matrix = (double**)malloc(tableau->rows * sizeof(double*));

    int i;

    for (i = 0; i < tableau->rows; i++)
        tableau->matrix[i] = (double*)calloc(tableau->columns, sizeof(double));

    for (i = 0; i < equations; i++) tableau->matrix[i][variables + i] = 1.0;

    return tableau;
}

void freeTableau(Tableau* tableau) {
    int i;

    for (i = 0; i < tableau->rows; i++) free(tableau->matrix[i]);

    free(tableau->matrix);
    free(tableau);
}

void pivot(Tableau* tableau, int pivotRow, int pivotColumn) {
    double pivotValue = tableau->matrix[pivotRow][pivotColumn];

    int i, j;

    for (j = 0; j < tableau->columns; j++)
        tableau->matrix[pivotRow][j] /= pivotValue;

    for (i = 0; i < tableau->rows; i++) {
        if (i != pivotRow) {
            double factor = tableau->matrix[i][pivotColumn];

            for (j = 0; j < tableau->columns; j++)
                tableau->matrix[i][j] -= factor * tableau->matrix[pivotRow][j];
        }
    }
}

bool simplex(Tableau* tableau) {
    while (true) {
        int pivotColumn = -1;
        int pivotRow = -1;
        double mostNegative = -EPSILON;

        for (int i = 0; i < tableau->rows - 1; i++) {
            if (tableau->matrix[i][tableau->columns - 1] < mostNegative) {
                mostNegative = tableau->matrix[i][tableau->columns - 1];

                pivotRow = i;
            }
        }

        if (pivotRow == -1) return true;

        for (int j = 0; j < tableau->columns - 1; j++) {
            if (tableau->matrix[pivotRow][j] < -EPSILON) {
                pivotColumn = j;

                break;
            }
        }

        if (pivotColumn == -1) return false;

        pivot(tableau, pivotRow, pivotColumn);
    }
}
