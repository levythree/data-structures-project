#include "./types/sat.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals) {
    clause->literals = (int*)malloc(numberOfLiterals * sizeof(int));
    clause->numberOfLiterals = numberOfLiterals;

    int i;

    for (i = 0; i < numberOfLiterals; i++) {
        clause->literals[i] = temporaryBuffer[i];
    }
}

Formula* createFormula(int numberOfVariables, int numberOfClauses) {
    Formula* formula = (Formula*)malloc(sizeof(Formula));

    formula->clauses = (Clause*)malloc(numberOfClauses * sizeof(Clause));
    formula->numberOfVariables = numberOfVariables;
    formula->numberOfClauses = numberOfClauses;

    return formula;
}

SATNode* createSATNode(int variableId, int valueAssigned) {
    SATNode* satNode = (SATNode*)malloc(sizeof(SATNode));

    satNode->variableId = variableId;
    satNode->valueAssigned = valueAssigned;
    satNode->left = NULL;
    satNode->right = NULL;

    return satNode;
}
