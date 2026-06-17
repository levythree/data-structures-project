# Repositório dos Projetos de Estrutura de Dados 🚀

Projeto realizado para obtenção de nota na AB2 na disciplina de **estruturas de dados**.
O projeto traz inúmeros aprendizados na área de pesquisa operacional, principalmente pelo uso de algoritmos como o simplex.
Este repositório contém os seguintes projetos:

- SAT Solver
- SMT Solver
- Plotagem de Árvore desbalanceada x Árvore AVL
- Huffman para compactação de arquivo
- Seminário sobre máquina de Turing utilizando duas pilhas

## SATSolver

SatSolvers são programas que resolvem um problema de satisfabilidade de uma fórmula booleana num formato CNF (Conjuntive Normal Form ou Forma Normal Conjuntiva).
O problema de satisfabilidade de uma fórmula booleana é considerado um problema NP-Completo (Nondeterministic Polynomial), ou seja, ainda não foi descoberto nenhuma forma eficiente (determinística) para resolver este problema.

O SATSolver implementado é dividido em quatro elementos principais

- Formula (Referente à fórmula inteira do problema, ou seja, um conjunto de cláusula)
- Clause (Referente a uma cláusula do problema)
- evaluateFormula (Função para avaliar a fórmula a partir de cada chute)
- sat (Função recursiva principal para validar e chutar um array de interpretações para ser avaliado)

Exemplo de arquivo de entrada <code>test.cnf</code>:

```bash
    c 1 variáveis, 2 cláusulas
    p cnf 1 2
    1 0
   -1 0
```

Isso é equivalente a essa fórmula:
(A) ∧ (¬A)

Obviamente, esta fórmula resulta em **UNSAT**.

### Lógica por traz do algoritmo

O algoritmo realiza o parse do arquivo .cnf e transforma isso em cláusulas, o que gera uma fórmula. A partir disso, é utilizado um array de interpretação utilizando índice base como 0, permitindo que os literais (1, 2, 3) sejam os índices desse array.
Como próximo passo, o sat começa a testar valores para os literais alterando o array de interpretação parcial e montando a árvore de tentativas. Dessa forma, o programa consegue testar os caminhos para tentar verificar se uma fórmula é satisfazível ou não, utilizando como estado para os literais os valores:

```bash
 1: True
-1: False
 0: Unassingned
```
