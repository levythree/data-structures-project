#ifndef liaH
#define liaH

#include <stdbool.h>
#include "simplex.h"

/*
 * LIA (Linear Integer Arithmetic) — Branch & Bound sobre o Simplex
 *
 * Resolve o problema:
 *   Minimizar  c^T x
 *   Sujeito a  Ax <= b,  x >= 0,  x inteiro
 *
 * O Branch & Bound funciona em três fases:
 *   1. Relaxação LP: resolve o problema sem restrição de inteireza.
 *   2. Se a solução LP já é inteira, retorna (nó viável).
 *   3. Caso contrário, escolhe uma variável fracionária x_j = f e ramifica:
 *        - Ramo esquerdo:  adiciona x_j <= floor(f)
 *        - Ramo direito:   adiciona x_j >= ceil(f)  =>  -x_j <= -ceil(f)
 *      Repete recursivamente em cada ramo.
 *
 * A busca para quando encontra a primeira solução inteira viável
 * (modo satisfatibilidade, adequado para SMT/LIA).
 */

/*
 * Problema LIA:
 *   - variables  : número de variáveis de decisão (x1..xn)
 *   - equations  : número de restrições originais
 *   - obj        : coeficientes da função objetivo (tamanho = variables)
 *                  Passe NULL para apenas verificar viabilidade.
 *   - A          : matriz de coeficientes [equations x variables]
 *   - b          : lado direito [equations]
 *
 * Internamente, o solver mantém um tableau aumentado que recebe novas
 * linhas a cada branch.  Para não modificar o problema original,
 * cada nó do B&B cria seu próprio tableau.
 */
typedef struct {
    int      variables;
    int      equations;
    double*  obj;          /* coeficientes objetivo; NULL = só viabilidade */
    double** A;            /* A[i][j] */
    double*  b;            /* b[i]    */
} LIAProblem;

/*
 * Nó da árvore Branch & Bound — guarda as restrições de branching
 * acumuladas desde a raiz.
 */
typedef struct {
    int     numBounds;     /* número de restrições extras (bounds) */
    int*    varIndex;      /* índice da variável limitada (0-based) */
    int*    direction;     /* -1 => x_j <= bound;  +1 => x_j >= bound */
    double* bound;         /* valor do limite                          */
} BBNode;

/* Resultado de uma execução do B&B */
typedef struct {
    bool    sat;           /* true se encontrou solução inteira viável */
    double* solution;      /* vetor de valores inteiros; NULL se UNSAT  */
    double  objValue;      /* valor da função objetivo na solução        */
    int     nodesExplored; /* estatística: nós visitados                 */
} BBResult;

/* ------------------------------------------------------------------ */
/* API pública                                                          */
/* ------------------------------------------------------------------ */

/* Cria um LIAProblem (aloca internamente). */
LIAProblem* createLIAProblem(int variables, int equations,
                             double* obj,
                             double** A, double* b);

/* Libera um LIAProblem. */
void freeLIAProblem(LIAProblem* lia);

/*
 * Resolve o LIAProblem com Branch & Bound.
 * O chamador deve liberar result->solution com free() se result.sat == true.
 */
BBResult solveLIA(LIAProblem* lia);

/* Imprime o resultado. */
void printLIAResult(LIAProblem* lia, BBResult* result);

/* ------------------------------------------------------------------ */
/* Funções auxiliares (visíveis para testes unitários)                  */
/* ------------------------------------------------------------------ */

/*
 * Constrói um Tableau LP a partir do LIAProblem mais as restrições
 * extras de branching descritas em BBNode.
 */
Tableau* buildLIATableau(LIAProblem* lia, BBNode* node);

/*
 * Verifica se todos os valores de solution[0..variables-1] são inteiros
 * (dentro de EPSILON).  Retorna o índice da primeira variável fracionária
 * ou -1 se todas são inteiras.
 */
int firstFractional(double* solution, int variables);

#endif
