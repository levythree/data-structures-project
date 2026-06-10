#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "lia.h"
#include "simplex.h"

#define EPSILON 1e-6

double getVariableValue(Tableau* tableau, int variableColumn) {
    int zeroCount = 0, oneCount = 0, rowWithTheOne = -1, i;

    for (i = 0; i < tableau->rows; i++) {
        if (tableau->matrix[i][variableColumn] == 0.0) zeroCount++;
        else if (tableau->matrix[i][variableColumn] == 1.0) {
            oneCount++;

            rowWithTheOne = i;
        }
    }

    if (zeroCount == tableau->rows - 1 && oneCount == 1) {
        return tableau->matrix[rowWithTheOne][tableau->columns - 1];
    } else return 0.0;
}

Tableau* cloneTableau(Tableau* oldTableau) {
    Tableau* newTableau = createTableau(oldTableau->variables, oldTableau->equations);

    int i, j;

    for (i = 0; i < oldTableau->rows; i++) {
        for (j = 0; j < oldTableau->columns; j++) {
            newTableau->matrix[i][j] = oldTableau->matrix[i][j];
        }
    }

    return newTableau;
}

Tableau* cloneAndExpand(Tableau* oldTableau) {
    Tableau* newTableau = createTableau(oldTableau->variables, oldTableau->equations + 1);

    int i, j;
    
    for (i = 0; i < oldTableau->rows - 1; i++) {
        for (j = 0; j < oldTableau->columns; j++) {
            if (j == oldTableau->columns - 1) {
                newTableau->matrix[i][newTableau->columns - 1] = oldTableau->matrix[i][j];
            } else {
                newTableau->matrix[i][j] = oldTableau->matrix[i][j];
            }
        }
    }
    
    int oldObjectiveRow = oldTableau->rows - 1;
    int newObjectiveRow = newTableau->rows - 1;
    
    for (j = 0; j < oldTableau->columns; j++) {
        if (j == oldTableau->columns - 1) {
            newTableau->matrix[newObjectiveRow][newTableau->columns - 1] = oldTableau->matrix[oldObjectiveRow][j];
        } else {
            newTableau->matrix[newObjectiveRow][j] = oldTableau->matrix[oldObjectiveRow][j];
        }
    }

    return newTableau;
}

bool solveLIA(Tableau* tableau, int** solution) {
    Tableau* workingTableau = cloneTableau(tableau);

    if (!simplex(workingTableau)) {
        freeTableau(workingTableau);

        return false;
    }

    int fractionalVariableIndex = -1, j;
    double fractionalValue = 0.0;

    for (j = 0; j < workingTableau->variables; j++) {
        double value = getVariableValue(workingTableau, j);

        if (fabs(value - round(value)) > EPSILON) {
            fractionalVariableIndex = j;
            fractionalValue = value;
            
            break;
        }
    }

    if (fractionalVariableIndex == -1) {
        for (j = 0; j < workingTableau->variables; j++) {
            (*solution)[j] = (int) round(getVariableValue(workingTableau, j));
        }

        freeTableau(workingTableau);

        return true;
    }

    double floorValue = floor(fractionalValue);
    double ceilValue = ceil(fractionalValue);

    freeTableau(workingTableau);

    Tableau* leftTableau = cloneAndExpand(tableau);

    int newRowLeft = leftTableau->rows - 2;

    leftTableau->matrix[newRowLeft][fractionalVariableIndex] = 1.0;
    leftTableau->matrix[newRowLeft][leftTableau->columns - 2] = 1.0;
    leftTableau->matrix[newRowLeft][leftTableau->columns - 1] = floorValue;

    if (solveLIA(leftTableau, solution)) {
        freeTableau(leftTableau);

        return true;
    }

    freeTableau(leftTableau);

    Tableau* rightTableau = cloneAndExpand(tableau);
    int newRowRight = rightTableau->rows - 2; 
    
    rightTableau->matrix[newRowRight][fractionalVariableIndex] = -1.0;
    rightTableau->matrix[newRowRight][rightTableau->columns - 2] = 1.0;
    rightTableau->matrix[newRowRight][rightTableau->columns - 1] = -ceilValue;

    if (solveLIA(rightTableau, solution)) {
        freeTableau(rightTableau);

        return true;
    }

    freeTableau(rightTableau);

    return false;
}

void printSolution(Tableau* tableau, int* solution) {
    int i;

    printf("Variable values:\n");

    for (i = 0; i < tableau->variables; i++) printf("x%d = %d\n", i + 1, solution[i]);
}
