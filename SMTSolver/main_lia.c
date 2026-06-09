#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "simplex.h"
#include "sat.h"
#include "lia.h"
#include "smt.h"

/* ------------------------------------------------------------------ */
/* Utilitário                                                            */
/* ------------------------------------------------------------------ */

static LinearAtom makeAtom(int numVars, double* coefs, double rhs) {
    LinearAtom atom;
    atom.numVars      = numVars;
    atom.rhs          = rhs;
    atom.coefficients = (double*) malloc(numVars * sizeof(double));
    memcpy(atom.coefficients, coefs, numVars * sizeof(double));
    return atom;
}

/* ------------------------------------------------------------------ */
/* Testes LIA puro (Branch & Bound direto)                             */
/* ------------------------------------------------------------------ */

/*
 * LIA Caso 1 — Inteiros viáveis
 *
 * Maximizar  x1 + x2       (equivale a minimizar -x1 - x2)
 * Sujeito a: 2x1 +  x2 <= 10
 *            -x1 + 2x2 <=  6
 *             x1        >=  0  (i.e. -x1 <= 0)
 *                   x2  >=  0  (i.e. -x2 <= 0)
 *             x1, x2 inteiros
 *
 * Solução LP ótima: x1 = 14/5 = 2.8,  x2 = 22/5 = 4.4  (fracionária)
 * Solução ILP ótima esperada: x1 = 2, x2 = 4  ou  x1 = 3, x2 = 4
 * (B&B encontra a primeira inteira viável com objetivo mínimo)
 */
static void testLIA1() {
    printf("========================================\n");
    printf("LIA Caso 1: Inteiros viáveis (ILP)\n");
    printf("  max x1+x2  s.t. 2x1+x2<=10, -x1+2x2<=6, x1,x2>=0, inteiros\n");
    printf("========================================\n");

    int vars = 2, eqs = 4;

    double* rows[4];
    double r0[] = { 2.0,  1.0};
    double r1[] = {-1.0,  2.0};
    double r2[] = {-1.0,  0.0};
    double r3[] = { 0.0, -1.0};
    rows[0] = r0; rows[1] = r1; rows[2] = r2; rows[3] = r3;

    double b[] = {10.0, 6.0, 0.0, 0.0};

    /* Minimizar -x1 - x2 (maximizar x1+x2) */
    double obj[] = {-1.0, -1.0};

    LIAProblem* lia = createLIAProblem(vars, eqs, obj, rows, b);
    BBResult result = solveLIA(lia);
    printLIAResult(lia, &result);

    if (result.sat) free(result.solution);
    freeLIAProblem(lia);
}

/*
 * LIA Caso 2 — Inviável em inteiros mas viável em racionais
 *
 * Minimizar x1
 * Sujeito a:  x1 >= 0.5  (i.e. -x1 <= -0.5)
 *             x1 <= 0.9  (i.e.  x1 <=  0.9)
 *             x1 inteiro
 *
 * LP relaxado: x1 = 0.5 (viável)
 * ILP: não existe inteiro em [0.5, 0.9] → UNSAT
 */
static void testLIA2() {
    printf("\n========================================\n");
    printf("LIA Caso 2: Viável em LP, UNSAT em ILP\n");
    printf("  0.5 <= x1 <= 0.9, x1 inteiro  =>  UNSAT\n");
    printf("========================================\n");

    int vars = 1, eqs = 2;

    double* rows[2];
    double r0[] = {-1.0};  /* x1 >= 0.5  =>  -x1 <= -0.5 */
    double r1[] = { 1.0};  /* x1 <= 0.9                   */
    rows[0] = r0; rows[1] = r1;

    double b[]   = {-0.5, 0.9};
    double obj[] = {1.0};

    LIAProblem* lia = createLIAProblem(vars, eqs, obj, rows, b);
    BBResult result = solveLIA(lia);
    printLIAResult(lia, &result);

    if (result.sat) free(result.solution);
    freeLIAProblem(lia);
}

/*
 * LIA Caso 3 — Sistema 2D com vários candidatos inteiros
 *
 * Minimizar x1 + 2*x2
 * Sujeito a:  x1 + x2 >=  3   (-x1 - x2 <= -3)
 *             x1       <=  5
 *                  x2  <=  4
 *             x1        >= 0   (-x1 <= 0)
 *                   x2  >= 0   (-x2 <= 0)
 *             x1, x2 inteiros
 *
 * Ótimo ILP esperado: x1=3, x2=0  (obj=3)
 */
static void testLIA3() {
    printf("\n========================================\n");
    printf("LIA Caso 3: Minimizar x1+2x2, sistema 2D\n");
    printf("  x1+x2>=3, x1<=5, x2<=4, x1,x2>=0, inteiros\n");
    printf("  Ótimo esperado: x1=3, x2=0 (obj=3)\n");
    printf("========================================\n");

    int vars = 2, eqs = 5;

    double* rows[5];
    double r0[] = {-1.0, -1.0};
    double r1[] = { 1.0,  0.0};
    double r2[] = { 0.0,  1.0};
    double r3[] = {-1.0,  0.0};
    double r4[] = { 0.0, -1.0};
    rows[0] = r0; rows[1] = r1; rows[2] = r2; rows[3] = r3; rows[4] = r4;

    double b[]   = {-3.0, 5.0, 4.0, 0.0, 0.0};
    double obj[] = { 1.0, 2.0};

    LIAProblem* lia = createLIAProblem(vars, eqs, obj, rows, b);
    BBResult result = solveLIA(lia);
    printLIAResult(lia, &result);

    if (result.sat) free(result.solution);
    freeLIAProblem(lia);
}

/* ------------------------------------------------------------------ */
/* Testes SMT(LIA) — DPLL(T) com Branch & Bound na teoria              */
/* ------------------------------------------------------------------ */

/*
 * SMT(LIA) Caso 1 — Fórmula booleana satisfeita com solução inteira
 *
 * Variáveis LP: x1, x2 (inteiras)
 * Átomos:
 *   A1: 2x1 +  x2 <= 10
 *   A2: -x1 + 2x2 <=  6
 *   A3: -x1        <= 0   (x1 >= 0)
 *   A4:      -x2   <= 0   (x2 >= 0)
 *
 * Fórmula: A1 ∧ A2 ∧ A3 ∧ A4  (todas ativas)
 * Esperado: SAT com x1, x2 inteiros (e.g. x1=2, x2=4 ou x1=3, x2=4)
 */
static void testSMTLIA1() {
    printf("\n========================================\n");
    printf("SMT(LIA) Caso 1: Fórmula conjuntiva, inteiros\n");
    printf("  A1 ∧ A2 ∧ A3 ∧ A4, x1,x2 inteiros\n");
    printf("========================================\n");

    int numAtoms = 4, numLPVars = 2;

    LinearAtom atoms[4];
    double c1[] = { 2.0,  1.0}; atoms[0] = makeAtom(2, c1, 10.0);
    double c2[] = {-1.0,  2.0}; atoms[1] = makeAtom(2, c2,  6.0);
    double c3[] = {-1.0,  0.0}; atoms[2] = makeAtom(2, c3,  0.0);
    double c4[] = { 0.0, -1.0}; atoms[3] = makeAtom(2, c4,  0.0);

    Formula* formula = createFormula(numAtoms, 4);
    int i;
    for (i = 0; i < 4; i++) {
        int lit = i + 1;
        initClause(&formula->clauses[i], &lit, 1);
    }

    SMTProblem* smt = createSMTProblem(formula, atoms, numAtoms, numLPVars,
                                       THEORY_LIA);

    SATNode* satNode    = NULL;
    double*  lpSolution = NULL;

    if (solveSMT(smt, &satNode, &lpSolution)) {
        printf("SAT!\n");
        printSMTSolution(smt, satNode, lpSolution);
        free(lpSolution);
    } else {
        printf("UNSAT!\n");
    }

    for (i = 0; i < numAtoms; i++) free(atoms[i].coefficients);
    freeSMTProblem(smt);
}

/*
 * SMT(LIA) Caso 2 — Alternativa: ramo LP inviável, mas inteiro viável
 *
 * Variáveis LP: x1, x2 (inteiras)
 * Átomos:
 *   A1: x1 + x2 <= -1  (inviável para x1,x2 inteiros >= 0)
 *   A2: x1      <=  5
 *   A3:      x2 <=  5
 *   A4: -x1     <=  0  (x1 >= 0)
 *   A5:     -x2 <=  0  (x2 >= 0)
 *
 * Fórmula: (A1 ∨ A2) ∧ A3 ∧ A4 ∧ A5
 * DPLL tenta A1 → B&B descobre inviável (inteira) → backtrack → tenta A2 → SAT
 */
static void testSMTLIA2() {
    printf("\n========================================\n");
    printf("SMT(LIA) Caso 2: Alternativa booleana com backtrack de teoria\n");
    printf("  (A1∨A2) ∧ A3 ∧ A4 ∧ A5, x1,x2 inteiros\n");
    printf("  A1 inviável em ILP → backtrack → A2 viável\n");
    printf("========================================\n");

    int numAtoms = 5, numLPVars = 2;

    LinearAtom atoms[5];
    double c1[] = { 1.0,  1.0}; atoms[0] = makeAtom(2, c1, -1.0);
    double c2[] = { 1.0,  0.0}; atoms[1] = makeAtom(2, c2,  5.0);
    double c3[] = { 0.0,  1.0}; atoms[2] = makeAtom(2, c3,  5.0);
    double c4[] = {-1.0,  0.0}; atoms[3] = makeAtom(2, c4,  0.0);
    double c5[] = { 0.0, -1.0}; atoms[4] = makeAtom(2, c5,  0.0);

    Formula* formula = createFormula(numAtoms, 4);
    int buf01[] = {1, 2}; initClause(&formula->clauses[0], buf01, 2);
    int buf1[]  = {3};    initClause(&formula->clauses[1], buf1,  1);
    int buf2[]  = {4};    initClause(&formula->clauses[2], buf2,  1);
    int buf3[]  = {5};    initClause(&formula->clauses[3], buf3,  1);

    SMTProblem* smt = createSMTProblem(formula, atoms, numAtoms, numLPVars,
                                       THEORY_LIA);

    SATNode* satNode    = NULL;
    double*  lpSolution = NULL;

    if (solveSMT(smt, &satNode, &lpSolution)) {
        printf("SAT!\n");
        printSMTSolution(smt, satNode, lpSolution);
        free(lpSolution);
    } else {
        printf("UNSAT!\n");
    }

    int i;
    for (i = 0; i < numAtoms; i++) free(atoms[i].coefficients);
    freeSMTProblem(smt);
}

/*
 * SMT(LIA) Caso 3 — UNSAT puro: restrições inteiras impossíveis
 *
 * Átomo A1: x1 >= 1  (-x1 <= -1)
 * Átomo A2: x1 <= 0  ( x1 <=  0)
 *
 * Fórmula: A1 ∧ A2
 * UNSAT: não existe inteiro com 1 <= x1 <= 0.
 */
static void testSMTLIA3() {
    printf("\n========================================\n");
    printf("SMT(LIA) Caso 3: UNSAT — sem inteiro viável\n");
    printf("  A1: x1>=1 ∧ A2: x1<=0  =>  UNSAT\n");
    printf("========================================\n");

    int numAtoms = 2, numLPVars = 1;

    LinearAtom atoms[2];
    double c1[] = {-1.0}; atoms[0] = makeAtom(1, c1, -1.0);
    double c2[] = { 1.0}; atoms[1] = makeAtom(1, c2,  0.0);

    Formula* formula = createFormula(numAtoms, 2);
    int pos1 = 1; initClause(&formula->clauses[0], &pos1, 1);
    int pos2 = 2; initClause(&formula->clauses[1], &pos2, 1);

    SMTProblem* smt = createSMTProblem(formula, atoms, numAtoms, numLPVars,
                                       THEORY_LIA);

    SATNode* satNode    = NULL;
    double*  lpSolution = NULL;

    if (solveSMT(smt, &satNode, &lpSolution)) {
        printf("SAT!\n");
        printSMTSolution(smt, satNode, lpSolution);
        free(lpSolution);
    } else {
        printf("UNSAT!\n");
    }

    int i;
    for (i = 0; i < numAtoms; i++) free(atoms[i].coefficients);
    freeSMTProblem(smt);
}

/*
 * SMT(LIA) Caso 4 — Comparação LA vs LIA: solução racional ≠ solução inteira
 *
 * Sujeito a: 3x1 + 2x2 <=  7
 *            -x1        <=  0  (x1 >= 0)
 *                  -x2  <=  0  (x2 >= 0)
 *
 * Em LA: x1=7/3 ≈ 2.33, x2=0 (viável)
 * Em LIA: x1=2, x2=0 (primeiro inteiro viável encontrado)
 */
static void testSMTLIAvsLA() {
    printf("\n========================================\n");
    printf("SMT: LA vs LIA — mesmos átomos, teorias diferentes\n");
    printf("  3x1+2x2<=7, x1>=0, x2>=0\n");
    printf("========================================\n");

    int numAtoms = 3, numLPVars = 2;

    LinearAtom atoms[3];
    double c1[] = { 3.0,  2.0}; atoms[0] = makeAtom(2, c1, 7.0);
    double c2[] = {-1.0,  0.0}; atoms[1] = makeAtom(2, c2, 0.0);
    double c3[] = { 0.0, -1.0}; atoms[2] = makeAtom(2, c3, 0.0);

    Formula* formula = createFormula(numAtoms, 3);
    int i;
    for (i = 0; i < 3; i++) {
        int lit = i + 1;
        initClause(&formula->clauses[i], &lit, 1);
    }

    /* --- LA --- */
    printf("\n-- Teoria LA (racionais) --\n");
    SMTProblem* smtLA = createSMTProblem(formula, atoms, numAtoms, numLPVars,
                                         THEORY_LA);
    SATNode* satNodeLA    = NULL;
    double*  lpSolutionLA = NULL;

    if (solveSMT(smtLA, &satNodeLA, &lpSolutionLA)) {
        printf("SAT!\n");
        printSMTSolution(smtLA, satNodeLA, lpSolutionLA);
        free(lpSolutionLA);
    } else {
        printf("UNSAT!\n");
    }
    freeSMTProblem(smtLA);

    /* --- LIA --- */
    printf("\n-- Teoria LIA (inteiros) --\n");
    SMTProblem* smtLIA = createSMTProblem(formula, atoms, numAtoms, numLPVars,
                                          THEORY_LIA);
    SATNode* satNodeLIA    = NULL;
    double*  lpSolutionLIA = NULL;

    if (solveSMT(smtLIA, &satNodeLIA, &lpSolutionLIA)) {
        printf("SAT!\n");
        printSMTSolution(smtLIA, satNodeLIA, lpSolutionLIA);
        free(lpSolutionLIA);
    } else {
        printf("UNSAT!\n");
    }

    for (i = 0; i < numAtoms; i++) free(atoms[i].coefficients);
    freeSMTProblem(smtLIA);
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */

int main() {
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  LIA — Branch & Bound sobre Simplex      ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    printf("===== Testes LIA puro (B&B direto) =====\n\n");
    testLIA1();
    testLIA2();
    testLIA3();

    printf("\n\n===== Testes SMT(LIA) — DPLL(T) + B&B =====\n");
    testSMTLIA1();
    testSMTLIA2();
    testSMTLIA3();
    testSMTLIAvsLA();

    return 0;
}
