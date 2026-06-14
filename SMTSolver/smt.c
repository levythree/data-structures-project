#include "./types/smt.h"

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types/lia.h"
#include "./types/atom.h"
#include "./types/sat.h"

void linearizeExpression(ASTNode* node, Tableau* tableau, int row, double sign,
                         SymbolTable* table) {
    if (!node) return;

    if (tableau == NULL) {
        if (node->type == NODE_VAR)
            getOrCreateVariableColumn(table, node->token);

        linearizeExpression(node->left, NULL, row, sign, table);
        linearizeExpression(node->right, NULL, row, sign, table);
        return;
    }

    switch (node->type) {
        case NODE_NUM:
            tableau->matrix[row][tableau->columns - 1] -=
                (atof(node->token) * sign);
            break;

        case NODE_VAR:
            int col = getOrCreateVariableColumn(table, node->token);
            tableau->matrix[row][col] += (1.0 * sign);
            break;

        case NODE_OP_ADD:
            linearizeExpression(node->left, tableau, row, sign, table);
            linearizeExpression(node->right, tableau, row, sign, table);
            break;

        case NODE_OP_SUB:
            if (node->right == NULL) {
                linearizeExpression(node->left, tableau, row, -sign, table);
            } else {
                linearizeExpression(node->left, tableau, row, sign, table);
                linearizeExpression(node->right, tableau, row, -sign, table);
            }
            break;
        case NODE_OP_MUL:
            ASTNode* numNode = NULL;
            ASTNode* varNode = NULL;

            if (node->left->type == NODE_NUM) {
                numNode = node->left;
                varNode = node->right;
            } else if (node->left->type == NODE_VAR) {
                numNode = node->right;
                varNode = node->left;
            }

            if (numNode && varNode && varNode->type == NODE_VAR) {
                double coefficient = atof(numNode->token);
                int col = getOrCreateVariableColumn(table, varNode->token);
                tableau->matrix[row][col] += (coefficient * sign);
            }

            break;

        case NODE_OP_EQ:
        case NODE_OP_LE:
        case NODE_OP_GE:
            linearizeExpression(node->left, tableau, row, sign, table);
            linearizeExpression(node->right, tableau, row, sign, table);
            break;

        default:
            break;
    }
}

LinearAtom extractLinearAtom(ASTNode* node, SymbolTable* table, bool invert) {
    Tableau* tempTab = createTableau(table->totalVariables, 1);

    double leftSign = invert ? -1.0 : 1.0;
    double rightSign = invert ? 1.0 : -1.0;

    linearizeExpression(node->left, tempTab, 0, leftSign, table);
    linearizeExpression(node->right, tempTab, 0, rightSign, table);

    double* coefficients = (double*)calloc(table->totalVariables, sizeof(double));
    for (int i = 0; i < table->totalVariables; i++) {
        coefficients[i] = tempTab->matrix[0][i];
    }

    double rhs = tempTab->matrix[0][tempTab->columns - 1];

    LinearAtom atom = createLinearAtom(table->totalVariables, coefficients, rhs);
    
    atom.type = NODE_OP_LE; 

    free(coefficients);
    freeTableau(tempTab);

    return atom;
}

void parseToLinearAtoms(ASTNode* node, LinearAtom* linearAtoms, int* counter, SymbolTable* table) {
    if (node == NULL) return;

    if (node->type == NODE_OP_OR) {
        parseToLinearAtoms(node->left, linearAtoms, counter, table);
        parseToLinearAtoms(node->right, linearAtoms, counter, table);

        return;
    }

    if (node->type == NODE_OP_LE) {
        linearAtoms[*counter] = extractLinearAtom(node, table, false);

        (*counter)++;
    }
    
    if (node->type == NODE_OP_GE) {
        linearAtoms[*counter] = extractLinearAtom(node, table, true);

        (*counter)++;
    }
}

bool atomsMatch(LinearAtom a, LinearAtom b, int numVars) {
    if (a.type != b.type) return false;
    
    if (fabs(a.rhs - b.rhs) > 1e-6) return false;
    
    for (int i = 0; i < numVars; i++) {
        if (fabs(a.coefficients[i] - b.coefficients[i]) > 1e-6) return false;
    }
    
    return true;
}

void mapAstToCnfBuffer(ASTNode* node, LinearAtom* linearAtoms, int totalAtoms,
                       int* clauseBuffer, int* literalCount, SymbolTable* table) {
    if (!node) return;

    if (node->type == NODE_OP_OR) {
        mapAstToCnfBuffer(node->left, linearAtoms, totalAtoms, clauseBuffer, literalCount, table);
        mapAstToCnfBuffer(node->right, linearAtoms, totalAtoms, clauseBuffer, literalCount, table);
        return;
    }

    if (node->type == NODE_OP_LE) {
        LinearAtom tempAtom = extractLinearAtom(node, table, false);

        for (int i = 0; i < totalAtoms; i++) {
            if (atomsMatch(linearAtoms[i], tempAtom, table->totalVariables)) {
                clauseBuffer[*literalCount] = i + 1;
                (*literalCount)++;

                break;
            }
        }

        freeLinearAtom(&tempAtom);
    }
    
    if (node->type == NODE_OP_GE) {
        LinearAtom tempAtomInverted = extractLinearAtom(node, table, true);

        for (int i = 0; i < totalAtoms; i++) {
            if (atomsMatch(linearAtoms[i], tempAtomInverted, table->totalVariables)) {
                clauseBuffer[*literalCount] = i + 1;
                (*literalCount)++;

                break;
            }
        }

        freeLinearAtom(&tempAtomInverted);
    }
}

void desugarEqualities(ASTNode** array, int* assertionsCount) {
    int originalCount = *assertionsCount;
    
    for (int i = 0; i < originalCount; i++) {
        if (array[i]->type == NODE_OP_EQ) {
            array[i]->type = NODE_OP_LE;
            strcpy(array[i]->token, "<=");

            ASTNode* duplicate = cloneAST(array[i]);
            duplicate->type = NODE_OP_GE;
            strcpy(duplicate->token, ">=");

            array[*assertionsCount] = duplicate;
            (*assertionsCount)++;
        }
    }
}

void printSolution(SymbolTable* table, int* solution) {
    printf("SAT!\n\n");

    for (int i = 0; i < table->totalVariables; i++) {
        printf("%s = %d\n", table->vars[i].name, solution[i]);
    }
}

bool checkTheory(int* interpretation, int totalVariables, SymbolTable* table, LinearAtom* linearAtoms) {
    int activeConstraints = 0;

    for (int i = 1; i <= totalVariables; i++) {
        if (interpretation[i] != 0) {
            activeConstraints++;
        }
    }

    if (activeConstraints == 0) return true;

    Tableau* solverTab = createTableau(table->totalVariables, activeConstraints);

    int currentRow = 0;

    for (int i = 1; i <= totalVariables; i++) {
        if (interpretation[i] == 0) continue;

        LinearAtom atom = linearAtoms[i - 1];

        if (interpretation[i] == 1) {
            for (int col = 0; col < table->totalVariables; col++) {
                solverTab->matrix[currentRow][col] = atom.coefficients[col];
            }

            solverTab->matrix[currentRow][solverTab->columns - 1] = atom.rhs;
        } else if (interpretation[i] == -1) {
            for (int col = 0; col < table->totalVariables; col++) {
                solverTab->matrix[currentRow][col] = -atom.coefficients[col];
            }

            solverTab->matrix[currentRow][solverTab->columns - 1] = -atom.rhs - 1.0;
        }

        currentRow++;
    }

    for (int col = 0; col < solverTab->columns; col++) {
        solverTab->matrix[solverTab->rows - 1][col] = 0.0;
    }

    int* dummySolution = (int*) malloc(table->totalVariables * sizeof(int));

    bool isConsistent = solveLIA(solverTab, &dummySolution);

    if (isConsistent) printSolution(table, dummySolution);

    free(dummySolution);
    freeTableau(solverTab);

    return isConsistent;
}

bool satSMT(Formula* formula, int* interpretation, int currentVariable, 
            SymbolTable* table, LinearAtom* linearAtoms) {
    
    int status = evaluateFormula(formula, interpretation);

    if (status == 1) {
        if (checkTheory(interpretation, formula->numberOfVariables, table, linearAtoms)) {
            return true;
        } else {
            return false;
        }
    }

    if (status == -1) return false;
    if (currentVariable > formula->numberOfVariables) return false;

    interpretation[currentVariable] = 1;
    if (satSMT(formula, interpretation, currentVariable + 1, table, linearAtoms)) {
        return true;
    }

    interpretation[currentVariable] = -1;
    if (satSMT(formula, interpretation, currentVariable + 1, table, linearAtoms)) {
        return true;
    }

    interpretation[currentVariable] = 0;
    return false;
}

bool smt(ASTNode** array, int assertionsCount, LinearAtom* linearAtoms, int linearAtomsCount, SymbolTable* table) {
    if (!assertionsCount) return true;

    Formula* formula = createFormula(linearAtomsCount, assertionsCount);

    int* temporaryBuffer = (int*) malloc(linearAtomsCount * sizeof(int));
    int numberOfClauses = 0;

    for (int i = 0; i < assertionsCount; i++) {
        int literalCount = 0;

        mapAstToCnfBuffer(array[i], linearAtoms, linearAtomsCount, temporaryBuffer, &literalCount, table);

        if (literalCount == 0) continue;

        initClause(&formula->clauses[i], temporaryBuffer, literalCount);

        numberOfClauses++;
    }

    formula->numberOfClauses = numberOfClauses;

    free(temporaryBuffer);

    int* interpretation = (int*)calloc(formula->numberOfVariables + 1, sizeof(int));

    bool result = satSMT(formula, interpretation, 1, table, linearAtoms);
    free(interpretation);

    return result;
}
