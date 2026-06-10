#include "lia.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simplex.h"

/* ------------------------------------------------------------------ */
/* Constantes                                                           */
/* ------------------------------------------------------------------ */

#define LIA_EPSILON 1e-6     /* tolerância para verificar inteireza    */
#define LIA_MAX_NODES 100000 /* limite de nós para evitar loops inf.   */

/* ------------------------------------------------------------------ */
/* LIAProblem — criação e liberação                                     */
/* ------------------------------------------------------------------ */

LIAProblem* createLIAProblem(int variables, int equations, double* obj,
                             double** A, double* b) {
    LIAProblem* lia = (LIAProblem*)malloc(sizeof(LIAProblem));

    lia->variables = variables;
    lia->equations = equations;

    /* Copia obj */
    if (obj != NULL) {
        lia->obj = (double*)malloc(variables * sizeof(double));
        memcpy(lia->obj, obj, variables * sizeof(double));
    } else {
        lia->obj = NULL;
    }

    /* Copia A */
    lia->A = (double**)malloc(equations * sizeof(double*));
    int i, j;
    for (i = 0; i < equations; i++) {
        lia->A[i] = (double*)malloc(variables * sizeof(double));
        for (j = 0; j < variables; j++) {
            lia->A[i][j] = A[i][j];
        }
    }

    /* Copia b */
    lia->b = (double*)malloc(equations * sizeof(double));
    memcpy(lia->b, b, equations * sizeof(double));

    return lia;
}

void freeLIAProblem(LIAProblem* lia) {
    int i;
    if (lia->obj) free(lia->obj);
    for (i = 0; i < lia->equations; i++) free(lia->A[i]);
    free(lia->A);
    free(lia->b);
    free(lia);
}

/* ------------------------------------------------------------------ */
/* BBNode — utilitários internos                                        */
/* ------------------------------------------------------------------ */

static BBNode* createBBNode(int capacity) {
    BBNode* node = (BBNode*)malloc(sizeof(BBNode));
    node->numBounds = 0;
    node->varIndex = (int*)malloc(capacity * sizeof(int));
    node->direction = (int*)malloc(capacity * sizeof(int));
    node->bound = (double*)malloc(capacity * sizeof(double));
    return node;
}

static void freeBBNode(BBNode* node) {
    free(node->varIndex);
    free(node->direction);
    free(node->bound);
    free(node);
}

/*
 * Clona um BBNode e adiciona uma nova restrição de branching.
 * direction == -1  =>  x_varIdx <= bound  (ramo esquerdo / floor)
 * direction == +1  =>  x_varIdx >= bound  (ramo direito  / ceil)
 */
static BBNode* cloneAndAddBound(BBNode* parent, int varIdx, int direction,
                                double bound) {
    int n = parent->numBounds;
    BBNode* node = createBBNode(n + 1);
    node->numBounds = n + 1;

    if (n > 0) {
        memcpy(node->varIndex, parent->varIndex, n * sizeof(int));
        memcpy(node->direction, parent->direction, n * sizeof(int));
        memcpy(node->bound, parent->bound, n * sizeof(double));
    }

    node->varIndex[n] = varIdx;
    node->direction[n] = direction;
    node->bound[n] = bound;

    return node;
}

/* ------------------------------------------------------------------ */
/* Construção do Tableau LP                                             */
/* ------------------------------------------------------------------ */

/*
 * O tableau tem:
 *   linhas = equations (originais) + numBounds (branching) + 1 (objetivo)
 *   colunas = variables + slacks + 1 (RHS)
 *
 * Cada restrição original  A[i]*x <= b[i]  vira  A[i]*x + s_i = b[i].
 * Cada bound de branching:
 *   x_j <= v   =>   x_j + s = v         (direction == -1)
 *   x_j >= v   =>  -x_j + s = -v        (direction == +1)
 * A linha do objetivo contém -obj (Simplex minimiza maximizando -obj).
 * Se obj == NULL, objetivo é zero (apenas viabilidade).
 */
Tableau* buildLIATableau(LIAProblem* lia, BBNode* node) {
    int totalEquations = lia->equations + node->numBounds;
    Tableau* tableau = createTableau(lia->variables, totalEquations);

    int i, j;

    /* --- Restrições originais --- */
    for (i = 0; i < lia->equations; i++) {
        for (j = 0; j < lia->variables; j++) {
            tableau->matrix[i][j] = lia->A[i][j];
        }
        /* variável de folga */
        tableau->matrix[i][lia->variables + i] = 1.0;
        /* RHS */
        tableau->matrix[i][tableau->columns - 1] = lia->b[i];
    }

    /* --- Restrições de branching --- */
    for (i = 0; i < node->numBounds; i++) {
        int row = lia->equations + i;
        int v = node->varIndex[i];
        int dir = node->direction[i];
        double bnd = node->bound[i];

        if (dir == -1) {
            /* x_v <= bnd  =>  x_v + s = bnd */
            tableau->matrix[row][v] = 1.0;
            tableau->matrix[row][lia->variables + row] = 1.0;
            tableau->matrix[row][tableau->columns - 1] = bnd;
        } else {
            /* x_v >= bnd  =>  -x_v + s = -bnd */
            tableau->matrix[row][v] = -1.0;
            tableau->matrix[row][lia->variables + row] = 1.0;
            tableau->matrix[row][tableau->columns - 1] = -bnd;
        }
    }

    /* --- Linha do objetivo --- */
    int objRow = tableau->rows - 1;

    if (lia->obj != NULL) {
        for (j = 0; j < lia->variables; j++) {
            /* Minimizar obj^T x  <==>  Simplex: -obj na linha objetivo */
            tableau->matrix[objRow][j] = -lia->obj[j];
        }
    }
    /* Se obj == NULL, linha objetivo já é zero (calloc). */

    return tableau;
}

/* ------------------------------------------------------------------ */
/* Extração da solução LP do tableau resolvido                          */
/* ------------------------------------------------------------------ */

static double* extractLPSolution(Tableau* tableau, int variables) {
    double* sol = (double*)calloc(variables, sizeof(double));
    int i, j;

    for (j = 0; j < variables; j++) {
        int zeroCount = 0, oneCount = 0, rowWithOne = -1;

        for (i = 0; i < tableau->rows; i++) {
            if (fabs(tableau->matrix[i][j] - 0.0) < LIA_EPSILON)
                zeroCount++;
            else if (fabs(tableau->matrix[i][j] - 1.0) < LIA_EPSILON) {
                oneCount++;
                rowWithOne = i;
            }
        }

        if (oneCount == 1 && zeroCount == tableau->rows - 1)
            sol[j] = tableau->matrix[rowWithOne][tableau->columns - 1];
        else
            sol[j] = 0.0;
    }

    return sol;
}

/* ------------------------------------------------------------------ */
/* Verificação de inteireza                                             */
/* ------------------------------------------------------------------ */

int firstFractional(double* solution, int variables) {
    int j;
    for (j = 0; j < variables; j++) {
        double frac = fabs(solution[j] - round(solution[j]));
        if (frac > LIA_EPSILON) return j;
    }
    return -1; /* todas inteiras */
}

/* ------------------------------------------------------------------ */
/* Branch & Bound — núcleo recursivo                                    */
/* ------------------------------------------------------------------ */

typedef struct {
    bool found;
    double* bestSolution;
    double bestObj;
    int nodesExplored;
} BBState;

static void bbSolve(LIAProblem* lia, BBNode* node, BBState* state) {
    if (state->nodesExplored >= LIA_MAX_NODES) return;
    state->nodesExplored++;

    /* 1. Resolve a relaxação LP do nó atual */
    Tableau* tableau = buildLIATableau(lia, node);
    bool feasible = simplex(tableau);

    if (!feasible) {
        /* Nó inviável: poda por inviabilidade */
        freeTableau(tableau);
        return;
    }

    /* 2. Extrai solução LP */
    double* lpSol = extractLPSolution(tableau, lia->variables);

    /* 3. Poda por limite (bound pruning) se já temos uma solução inteira */
    if (lia->obj != NULL && state->found) {
        double objLP = 0.0;
        int j;
        for (j = 0; j < lia->variables; j++) objLP += lia->obj[j] * lpSol[j];

        if (objLP >= state->bestObj - LIA_EPSILON) {
            /* LP relaxação não melhora o melhor inteiro conhecido: poda */
            free(lpSol);
            freeTableau(tableau);
            return;
        }
    }

    freeTableau(tableau);

    /* 4. Verifica inteireza */
    int fracVar = firstFractional(lpSol, lia->variables);

    if (fracVar == -1) {
        /* Solução LP é inteira — candidata viável */
        double objVal = 0.0;
        int j;

        if (lia->obj != NULL)
            for (j = 0; j < lia->variables; j++)
                objVal += lia->obj[j] * lpSol[j];

        if (!state->found || objVal < state->bestObj - LIA_EPSILON) {
            state->found = true;
            if (state->bestSolution) free(state->bestSolution);
            state->bestSolution = lpSol;
            state->bestObj = objVal;
        } else {
            free(lpSol);
        }

        return;
    }

    /* 5. Branching na primeira variável fracionária */
    double fracVal = lpSol[fracVar];
    free(lpSol);

    double floorVal = floor(fracVal);
    double ceilVal = ceil(fracVal);

    /* Ramo esquerdo:  x_fracVar <= floor(fracVal) */
    BBNode* leftNode = cloneAndAddBound(node, fracVar, -1, floorVal);
    bbSolve(lia, leftNode, state);
    freeBBNode(leftNode);

    /* Ramo direito: x_fracVar >= ceil(fracVal) */
    BBNode* rightNode = cloneAndAddBound(node, fracVar, +1, ceilVal);
    bbSolve(lia, rightNode, state);
    freeBBNode(rightNode);
}

/* ------------------------------------------------------------------ */
/* API pública: solveLIA                                                */
/* ------------------------------------------------------------------ */

BBResult solveLIA(LIAProblem* lia) {
    BBResult result;
    result.sat = false;
    result.solution = NULL;
    result.objValue = 0.0;
    result.nodesExplored = 0;

    /* Nó raiz: sem restrições de branching */
    BBNode* root = createBBNode(1);

    BBState state;
    state.found = false;
    state.bestSolution = NULL;
    state.bestObj = DBL_MAX;
    state.nodesExplored = 0;

    bbSolve(lia, root, &state);
    freeBBNode(root);

    result.sat = state.found;
    result.solution = state.bestSolution;
    result.objValue = state.bestObj;
    result.nodesExplored = state.nodesExplored;

    return result;
}

/* ------------------------------------------------------------------ */
/* Impressão                                                            */
/* ------------------------------------------------------------------ */

void printLIAResult(LIAProblem* lia, BBResult* result) {
    printf("\n=== Resultado LIA (Branch & Bound) ===\n");
    printf("Nós explorados: %d\n", result->nodesExplored);

    if (!result->sat) {
        printf("UNSAT\n");
        return;
    }

    printf("SAT\n");
    printf("Valores inteiros:\n");
    int j;
    for (j = 0; j < lia->variables; j++)
        printf("  x%d = %.0f\n", j + 1, result->solution[j]);

    if (lia->obj != NULL) printf("Valor objetivo: %.6f\n", result->objValue);
}
