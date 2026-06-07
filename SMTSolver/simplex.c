#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "simplex.h"

Tableau* createTableau(int variables, int equations) {
    Tableau* tableau = (Tableau*) malloc(sizeof(Tableau));

    tableau->variables = variables;
    tableau->equations = equations;
    tableau->rows = equations + 1;
    tableau->columns = variables + equations + 1;
    tableau->matrix = (double**) malloc(tableau->rows * sizeof(double*));

    int i;

    for (i = 0; i < tableau->rows; i++) tableau->matrix[i] = (double*) calloc(tableau->columns, sizeof(double));

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

    for (j = 0; j < tableau->columns; j++) tableau->matrix[pivotRow][j] /= pivotValue;

    for (i = 0; i < tableau->rows; i++) {
        if (i != pivotRow) {
            double factor = tableau->matrix[i][pivotColumn];

            for (j = 0; j < tableau->columns; j++) tableau->matrix[i][j] -= factor * tableau->matrix[pivotRow][j];
        }
    }
}

bool simplex(Tableau* tableau) {
    while (true) {
        int i, j;
        int pivotColumn = -1;
        int pivotRow = -1;
        double lowest = 0.0;

        for (i = 0; i < tableau->rows - 1; i++) {
            if (tableau->matrix[i][tableau->columns - 1] < 0) {
                bool canBeFixed = false;

                for (j = 0; j < tableau->columns - 1; j++) {
                    if (tableau->matrix[i][j] < 0) {
                        canBeFixed = true;

                        break;
                    }
                }

                if (!canBeFixed) return false;
            }
        }

        for (j = 0; j < tableau->columns - 1; j++) {
            if (tableau->matrix[tableau->rows - 1][j] < lowest) {
                lowest = tableau->matrix[tableau->rows - 1][j];

                pivotColumn = j;
            }
        }

        if (lowest >= 0.0) return true;

        double minimunRatio = -1.0;
        
        for (i = 0; i < tableau->rows - 1; i++) {
            double columnValue = tableau->matrix[i][pivotColumn];
            double constantValue = tableau->matrix[i][tableau->columns - 1];

            if (columnValue > 0) {
                double ratio = constantValue / columnValue;

                if (minimunRatio == -1.0 || ratio < minimunRatio) {
                    minimunRatio = ratio;

                    pivotRow = i;
                }
            }
        }

        if (pivotRow == -1) return true;

        pivot(tableau, pivotRow, pivotColumn);
    }
}

void printSolution(Tableau* tableau) {
    printf("Variables:\n");

    int i, j;

    for (j = 0; j < tableau->columns - 1; j++) {
        int zeroCount = 0, oneCount = 0, rowWithTheOne = -1;

        for (i = 0; i < tableau->rows; i++) {
            if (tableau->matrix[i][j] == 0.0) zeroCount++;
            else if (tableau->matrix[i][j] == 1.0) {
                oneCount++;

                rowWithTheOne = i;
            }
        }

        double value = 0.0;

        if (oneCount == 1 && zeroCount == tableau->rows - 1) {
            value = tableau->matrix[rowWithTheOne][tableau->columns - 1];
        }

        if (j < tableau->variables) printf("x%d = %.2lf\n", j + 1, value);
        else printf("s%d = %.2lf\n", (j - tableau->variables) + 1, value);
    }
}
