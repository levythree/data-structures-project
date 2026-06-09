#ifndef smtH
#define smtH

#include <stdbool.h>
#include "simplex.h"
#include "sat.h"
#include "lia.h"

/*
 * SMT (Satisfiability Modulo Theories)
 *
 * Teorias suportadas:
 *   - LA  (Linear Arithmetic sobre racionais) — Simplex direto
 *   - LIA (Linear Integer Arithmetic)         — Branch & Bound sobre Simplex
 *
 * Cada átomo linear representa uma inequação da forma:
 *   a1*x1 + a2*x2 + ... + an*xn <= b
 *
 * O SAT solver atribui valores booleanos aos átomos (DPLL).
 * O solver de teoria verifica se a conjunção dos átomos ativos é viável
 * na teoria escolhida.
 *
 * Para LIA, a verificação usa Branch & Bound:
 *   1. Resolve o LP relaxado (Simplex).
 *   2. Se a solução é inteira → viável na teoria.
 *   3. Se é fracionária → branch em uma variável fracionária.
 *   4. Repete recursivamente nos dois ramos.
 */

/* Modo de teoria */
typedef enum {
    THEORY_LA,   /* Linear Arithmetic (racionais)        */
    THEORY_LIA   /* Linear Integer Arithmetic (inteiros) */
} TheoryMode;

/* Um átomo linear: coefficients[] * x <= rhs */
typedef struct {
    double* coefficients; /* coeficiente de cada variável LP */
    double  rhs;          /* lado direito da inequação        */
    int     numVars;      /* número de variáveis LP           */
} LinearAtom;

/*
 * Problema SMT:
 *   - formula   : fórmula booleana em CNF (variáveis = índices de átomos)
 *   - atoms     : vetor de átomos lineares
 *   - numAtoms  : deve ser == formula->numberOfVariables
 *   - numLPVars : número de variáveis LP (x1, x2, ...)
 *   - mode      : THEORY_LA ou THEORY_LIA
 */
typedef struct {
    Formula*    formula;
    LinearAtom* atoms;
    int         numAtoms;
    int         numLPVars;
    TheoryMode  mode;
} SMTProblem;

/* Cria um problema SMT */
SMTProblem* createSMTProblem(Formula* formula,
                             LinearAtom* atoms,
                             int numAtoms,
                             int numLPVars,
                             TheoryMode mode);

/* Libera um problema SMT */
void freeSMTProblem(SMTProblem* smt);

/*
 * Resolve o problema SMT (DPLL(T)).
 *
 * Retorna true se SAT, preenchendo:
 *   - satNode   : árvore de decisão booleana (DPLL)
 *   - lpSolution: vetor com os valores das variáveis LP/LIA
 *
 * O chamador deve liberar lpSolution com free().
 */
bool solveSMT(SMTProblem* smt, SATNode** satNode, double** lpSolution);

/*
 * Verifica a teoria para uma interpretação booleana completa.
 *
 * Para LA  → usa o Simplex diretamente.
 * Para LIA → usa Branch & Bound (solveLIA).
 *
 * Retorna true se viável, preenchendo lpSolution.
 */
bool checkTheory(SMTProblem* smt,
                 int* interpretation,
                 double** lpSolution);

/* Constrói o tableau a partir dos átomos ativos na interpretação */
Tableau* buildTableau(SMTProblem* smt, int* interpretation);

/* Imprime a solução SMT */
void printSMTSolution(SMTProblem* smt,
                      SATNode* satNode,
                      double* lpSolution);

#endif
