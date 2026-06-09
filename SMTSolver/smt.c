#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "smt.h"
#include "simplex.h"
#include "sat.h"
#include "lia.h"

#define SMT_EPSILON 1e-6

/* ------------------------------------------------------------------ */
/* Criação / liberação                                                  */
/* ------------------------------------------------------------------ */

SMTProblem* createSMTProblem(Formula* formula,
                             LinearAtom* atoms,
                             int numAtoms,
                             int numLPVars,
                             TheoryMode mode) {
    SMTProblem* smt = (SMTProblem*) malloc(sizeof(SMTProblem));
    smt->formula   = formula;
    smt->atoms     = atoms;
    smt->numAtoms  = numAtoms;
    smt->numLPVars = numLPVars;
    smt->mode      = mode;
    return smt;
}

void freeSMTProblem(SMTProblem* smt) {
    free(smt);
}

/* ------------------------------------------------------------------ */
/* Construção do Tableau (LA — Simplex direto)                          */
/* ------------------------------------------------------------------ */

/*
 * Átomo positivo (val == +1):  coef*x + s = rhs
 * Átomo negado  (val == -1): -coef*x + s = -rhs - epsilon
 */
Tableau* buildTableau(SMTProblem* smt, int* interpretation) {
    int activeConstraints = 0;
    int v;

    for (v = 1; v <= smt->numAtoms; v++)
        if (interpretation[v] != 0) activeConstraints++;

    if (activeConstraints == 0) return NULL;

    Tableau* tableau = createTableau(smt->numLPVars, activeConstraints);

    int row = 0;

    for (v = 1; v <= smt->numAtoms; v++) {
        int val = interpretation[v];
        if (val == 0) continue;

        LinearAtom* atom = &smt->atoms[v - 1];
        int col;

        if (val == 1) {
            for (col = 0; col < smt->numLPVars; col++)
                tableau->matrix[row][col] = atom->coefficients[col];

            tableau->matrix[row][smt->numLPVars + row]  = 1.0;
            tableau->matrix[row][tableau->columns - 1]  = atom->rhs;
        } else {
            for (col = 0; col < smt->numLPVars; col++)
                tableau->matrix[row][col] = -atom->coefficients[col];

            tableau->matrix[row][smt->numLPVars + row]  = 1.0;
            tableau->matrix[row][tableau->columns - 1]  = -atom->rhs - SMT_EPSILON;
        }

        row++;
    }

    return tableau;
}

/* ------------------------------------------------------------------ */
/* Construção do LIAProblem a partir dos átomos ativos                 */
/* ------------------------------------------------------------------ */

/*
 * Converte os átomos ativos em um LIAProblem para o Branch & Bound.
 * Mesma lógica de buildTableau, mas na estrutura LIAProblem.
 */
static LIAProblem* buildLIAProblem(SMTProblem* smt, int* interpretation) {
    /* Conta restrições ativas */
    int activeConstraints = 0;
    int v;

    for (v = 1; v <= smt->numAtoms; v++)
        if (interpretation[v] != 0) activeConstraints++;

    if (activeConstraints == 0) return NULL;

    /* Monta A e b */
    double** A = (double**) malloc(activeConstraints * sizeof(double*));
    double*  b = (double*)  malloc(activeConstraints * sizeof(double));

    int row = 0;

    for (v = 1; v <= smt->numAtoms; v++) {
        int val = interpretation[v];
        if (val == 0) continue;

        LinearAtom* atom = &smt->atoms[v - 1];
        A[row] = (double*) malloc(smt->numLPVars * sizeof(double));

        int col;

        if (val == 1) {
            for (col = 0; col < smt->numLPVars; col++)
                A[row][col] = atom->coefficients[col];
            b[row] = atom->rhs;
        } else {
            for (col = 0; col < smt->numLPVars; col++)
                A[row][col] = -atom->coefficients[col];
            b[row] = -atom->rhs - SMT_EPSILON;
        }

        row++;
    }

    /* Objetivo NULL: apenas viabilidade */
    LIAProblem* lia = createLIAProblem(smt->numLPVars, activeConstraints,
                                       NULL, A, b);

    /* Libera arrays temporários (createLIAProblem fez cópias internas) */
    for (row = 0; row < activeConstraints; row++) free(A[row]);
    free(A);
    free(b);

    return lia;
}

/* ------------------------------------------------------------------ */
/* Verificação de teoria                                                */
/* ------------------------------------------------------------------ */

bool checkTheory(SMTProblem* smt,
                 int* interpretation,
                 double** lpSolution) {

    /* --- Caso LA: Simplex direto --- */
    if (smt->mode == THEORY_LA) {
        Tableau* tableau = buildTableau(smt, interpretation);

        if (tableau == NULL) {
            *lpSolution = (double*) calloc(smt->numLPVars, sizeof(double));
            return true;
        }

        bool feasible = simplex(tableau);

        if (feasible) {
            double* sol = (double*) calloc(smt->numLPVars, sizeof(double));
            int i, j;

            for (j = 0; j < smt->numLPVars; j++) {
                int zeroCount = 0, oneCount = 0, rowWithOne = -1;

                for (i = 0; i < tableau->rows; i++) {
                    if (tableau->matrix[i][j] == 0.0)
                        zeroCount++;
                    else if (fabs(tableau->matrix[i][j] - 1.0) < SMT_EPSILON) {
                        oneCount++;
                        rowWithOne = i;
                    }
                }

                if (oneCount == 1 && zeroCount == tableau->rows - 1)
                    sol[j] = tableau->matrix[rowWithOne][tableau->columns - 1];
                else
                    sol[j] = 0.0;
            }

            *lpSolution = sol;
        }

        freeTableau(tableau);
        return feasible;
    }

    /* --- Caso LIA: Branch & Bound --- */
    LIAProblem* lia = buildLIAProblem(smt, interpretation);

    if (lia == NULL) {
        /* Nenhum átomo ativo: trivialmente viável */
        *lpSolution = (double*) calloc(smt->numLPVars, sizeof(double));
        return true;
    }

    BBResult result = solveLIA(lia);
    freeLIAProblem(lia);

    if (result.sat) {
        *lpSolution = result.solution; /* transfere a posse */
    }

    return result.sat;
}

/* ------------------------------------------------------------------ */
/* DPLL(T) — núcleo recursivo                                           */
/* ------------------------------------------------------------------ */

typedef struct {
    SMTProblem* smt;
    double*     lpSolution;
} SMTContext;

static bool smtSolve(SMTContext* ctx,
                     int* interpretation,
                     int currentVariable,
                     SATNode** satNode) {
    Formula*    formula = ctx->smt->formula;
    SMTProblem* smt     = ctx->smt;

    int status = evaluateFormula(formula, interpretation);

    if (status == -1) return false; /* conflito booleano: backtrack */

    /* Fórmula satisfeita com todas as variáveis atribuídas → checa teoria */
    if (status == 1) {
        double* sol = NULL;

        if (!checkTheory(smt, interpretation, &sol)) return false;

        ctx->lpSolution = sol;
        return true;
    }

    if (currentVariable > formula->numberOfVariables) return false;

    *satNode = createSATNode(currentVariable, 0);

    /* Tenta TRUE */
    interpretation[currentVariable] = 1;
    (*satNode)->valueAssigned = 1;

    if (smtSolve(ctx, interpretation, currentVariable + 1, &((*satNode)->left)))
        return true;

    /* Tenta FALSE */
    interpretation[currentVariable] = -1;
    (*satNode)->valueAssigned = -1;

    if (smtSolve(ctx, interpretation, currentVariable + 1, &((*satNode)->right)))
        return true;

    /* Backtrack */
    interpretation[currentVariable] = 0;
    free(*satNode);
    *satNode = NULL;

    return false;
}

bool solveSMT(SMTProblem* smt, SATNode** satNode, double** lpSolution) {
    int* interpretation = (int*) calloc(smt->formula->numberOfVariables + 1,
                                        sizeof(int));
    SMTContext ctx;
    ctx.smt        = smt;
    ctx.lpSolution = NULL;

    bool result = smtSolve(&ctx, interpretation, 1, satNode);

    free(interpretation);

    if (result) *lpSolution = ctx.lpSolution;

    return result;
}

/* ------------------------------------------------------------------ */
/* Impressão                                                            */
/* ------------------------------------------------------------------ */

void printSMTSolution(SMTProblem* smt,
                      SATNode* satNode,
                      double* lpSolution) {
    printf("\n=== Solução SMT (%s) ===\n",
           smt->mode == THEORY_LA ? "LA" : "LIA");

    printf("\nAtribuição booleana (átomos ativos):\n");
    printSATTree(satNode);

    printf("\nValores das variáveis:\n");
    int j;

    for (j = 0; j < smt->numLPVars; j++) {
        if (smt->mode == THEORY_LIA)
            printf("  x%d = %.0f\n", j + 1, lpSolution[j]);
        else
            printf("  x%d = %.6f\n", j + 1, lpSolution[j]);
    }

    printf("\nVerificação dos átomos:\n");
    int v;

    for (v = 0; v < smt->numAtoms; v++) {
        LinearAtom* atom = &smt->atoms[v];
        double lhs = 0.0;
        int col;

        for (col = 0; col < atom->numVars; col++)
            lhs += atom->coefficients[col] * lpSolution[col];

        printf("  Átomo %d: %.6f <= %.6f  [%s]\n",
               v + 1, lhs, atom->rhs,
               (lhs <= atom->rhs + SMT_EPSILON) ? "OK" : "VIOLADO");
    }
}
