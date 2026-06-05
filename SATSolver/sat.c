#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sat.h"

void initClause(Clause* clause, int* temporaryBuffer, int numberOfLiterals) {
    clause->literals = (int*) malloc(numberOfLiterals * sizeof(int));
    clause->numberOfLiterals = numberOfLiterals;

    int i;

    for (i = 0; i < numberOfLiterals; i++) {
        clause->literals[i] = temporaryBuffer[i];
    }
}

Formula* createFormula(int numberOfVariables, int numberOfClauses) {
    Formula* formula = (Formula*) malloc(sizeof(Formula));

    formula->clauses = (Clause*) malloc(numberOfClauses * sizeof(Clause));
    formula->numberOfVariables = numberOfVariables;
    formula->numberOfClauses = numberOfClauses;

    return formula;
}

SATNode* createSATNode(int variableId, int valueAssigned) {
    SATNode* satNode = (SATNode*) malloc(sizeof(SATNode));

    satNode->variableId = variableId;
    satNode->valueAssigned = valueAssigned;
    satNode->left = NULL;
    satNode->right = NULL;

    return satNode;
}

Formula* readCnf(const char* fileName) {
    FILE* cnf = fopen(fileName, "r");

    int numberOfVariables, numberOfClauses;

    char line[256];

    while (fgets(line, sizeof(line), cnf)) {
        if (line[0] == 'c') continue;
        else if (line[0] == 'p') {
            sscanf(line, "p cnf %d %d", &numberOfVariables, &numberOfClauses);

            break;
        }
    }

    Formula* formula = createFormula(numberOfVariables, numberOfClauses);

    int* temporaryBuffer = (int*) malloc(numberOfVariables * sizeof(int));

    int literal, literalCount = 0, literalIndex = 0;

    while (literalIndex < numberOfClauses && fscanf(cnf, "%d", &literal) == 1) {
        if (literal != 0) {
            temporaryBuffer[literalCount] = literal;

            literalCount++;
        } else {
            initClause(&formula->clauses[literalIndex], temporaryBuffer, literalCount);

            literalCount = 0;
            literalIndex++;
        }
    }

    free(temporaryBuffer);

    fclose(cnf);

    return formula;
}

int evaluateFormula(Formula* formula, int* interpretation) {
    bool allClausesTrue = true;
    
    int i, j;

    for (i = 0; i < formula->numberOfClauses; i++) {
        bool clauseIsTrue = false, clauseHasUnassigned = false;

        for (j = 0; j < formula->clauses[i].numberOfLiterals; j++) {
            int literal = formula->clauses[i].literals[j];
            int variableId = abs(literal);
            int variableValue = interpretation[variableId];

            int literalTruthValue = 0;
            
            if (variableValue != 0) literalTruthValue = (literal > 0) ? variableValue : -variableValue;

            if (literalTruthValue == 1) {
                clauseIsTrue = true;

                break;
            } else if (literalTruthValue == 0) clauseHasUnassigned = true;
        }

        if (!clauseIsTrue) {
            if (!clauseHasUnassigned) return -1;
            else allClausesTrue = false;
        }
    }

    if (allClausesTrue) return 1;

    return 0;
}

bool sat(Formula* formula, int* interpretation, int currentVariable, SATNode** satNode) {
    int status = evaluateFormula(formula, interpretation);

    if (status == 1) return true;
    if (status == -1) return false;

    if (currentVariable > formula->numberOfVariables) return false;

    *satNode = createSATNode(currentVariable, 0);

    interpretation[currentVariable] = 1;
    (*satNode)->valueAssigned = 1;
    if (sat(formula, interpretation, currentVariable + 1, &((*satNode)->left))) return true;

    interpretation[currentVariable] = -1;
    (*satNode)->valueAssigned = -1;
    if (sat(formula, interpretation, currentVariable + 1, &((*satNode)->right))) return true;

    interpretation[currentVariable] = 0;
    free(*satNode);
    (*satNode) = NULL;
    
    return false;
}

void printSATTree(SATNode* satNode) {
    if (satNode == NULL) return;

    if (satNode->valueAssigned == 1) printf("x%d: True\n", satNode->variableId);
    else if (satNode->valueAssigned == -1) printf("x%d: False\n", satNode->variableId);

    printSATTree(satNode->left);
    printSATTree(satNode->right);
}
