# Repositório dos Projetos de Estrutura de Dados 🚀

Projeto realizado para obtenção de nota na AB2 na disciplina de **estruturas de dados**.
O projeto traz inúmeros aprendizados na área de pesquisa operacional, principalmente pelo uso de algoritmos como o simplex.
Este repositório contém os seguintes projetos:

- Plotagem de Árvore desbalanceada x Árvore AVL
- SAT Solver
- SMT Solver
- Huffman para compactação de arquivo
- Seminário sobre máquina de Turing utilizando duas pilhas

## Plotagem de Árvore desbalanceada x Árvore AVL

Esse projeto foi uma pesquisa a fim de comparar o desempenho de uma árvore binária desbalanceada e uma árvore binária AVL (naturalmente balanceada). Foram utilizados 32768 números distintos (0 <= X <= 32768) inseridos de forma aleatória para ter um teste mais fiel à realidade.

Este foi o resultado:

![Comparativo da plotagem](assets/chart.png)

### Conclusões

1. Nota-se que a árvore desbalanceada se aproxima de $\mathcal{O}(n^2)$.

2. É possível analisar também que a árvore AVL sempre permanece na curva logaritma proposta pela estrutura de dados. $\mathcal{O}(log(n))$

3. Além disso, é interessante destacar o _pico_ no fim do gráfico da árvore AVL. Isso acontece porque há uma busca adicional para o valor 32768, uma vez uma árvore completa tem $\mathcal 2^n - 1$ itens.

## SATSolver

SatSolvers são programas que resolvem um problema de satisfabilidade de uma fórmula booleana num formato CNF (Conjuntive Normal Form ou Forma Normal Conjuntiva).
O problema de satisfabilidade de uma fórmula booleana é considerado um problema NP-Completo (Nondeterministic Polynomial), ou seja, ainda não foi descoberto nenhuma forma eficiente (determinística) para resolver este problema.

O SATSolver implementado é dividido em quatro elementos principais

- Formula (Referente à fórmula inteira do problema, ou seja, um conjunto de cláusula)
- Clause (Referente a uma cláusula do problema)
- evaluateFormula (Função para avaliar a fórmula a partir de cada chute)
- sat (Função recursiva principal para validar e chutar um array de interpretações para ser avaliado)

Exemplo de arquivo de entrada <code>test.cnf</code>:

```text
    c 1 variável, 2 cláusulas
    p cnf 1 2
    1 0
   -1 0
```

Isso é equivalente a essa fórmula:
(A) ∧ (¬A)

Obviamente, esta fórmula resulta em **UNSAT**.

### Lógica por trás do algoritmo

O algoritmo realiza o parse do arquivo .cnf e transforma isso em cláusulas, o que gera uma fórmula. A partir disso, é utilizado um array de interpretação usando índice base como 1, permitindo que os literais (1, 2, 3) sejam os índices desse array.
Como próximo passo, o sat começa a testar valores para os literais alterando o array de interpretação parcial e montando a árvore de tentativas. Dessa forma, o programa consegue testar os caminhos para tentar verificar se uma fórmula é satisfazível ou não, utilizando como estado para os literais os valores:

```text
 1: True
-1: False
 0: Unassigned
```

### Backtracking

O resolvedor utiliza o algoritmo DPLL com um sistema de Backtracking Cronológico via recursão. Em vez de testar todas as combinações possíveis em uma matriz de tabela verdade completa ($\mathcal{O}(2^N)$), o motor avalia a fórmula a cada atribuição parcial. Se a função evaluateFormula detectar que qualquer cláusula tornou-se inteiramente falsa antes de todas as variáveis serem preenchidas, o algoritmo realiza uma poda por conflito: ele limpa os estados locais de volta para 0, aborta a descida naquele ramo e retrocede (backtrack) para tentar outro caminho.

## Huffman

O algoritmo de Huffman é um método de compressão de dados sem perdas baseado na frequência de ocorrência de cada caractere. Caracteres que aparecem com mais frequência recebem códigos binários menores, enquanto os menos frequentes recebem códigos maiores. Isso resulta em uma redução significativa no tamanho final do arquivo.

O compressor implementado utiliza uma abordagem completa, lidando com a leitura bit a bit dos arquivos e garantindo que o arquivo original possa ser perfeitamente reconstruído na descompressão.

### Lógica por trás do algoritmo

A implementação foi modularizada para garantir clareza e manutenção do código. O fluxo de compressão e descompressão se baseia em três pilares principais:

- **Fila de Prioridade (priorityQueue):** Responsável por armazenar e ordenar os nós da árvore com base em suas frequências (do menor para o maior). O custo de inserção para construir a fila dita grande parte da performance do algoritmo.

- **Árvore de Huffman (huffman):** Constrói a árvore unindo os dois nós de menor frequência iterativamente até sobrar apenas a raiz. A partir da árvore montada, é gerado um dicionário que mapeia cada caractere (0 a 255) para sua nova representação em bits. O algoritmo apresenta uma complexidade de tempo de $\mathcal{O}(N \log N)$ para a construção da árvore, onde N é o número de caracteres únicos.

- **Manipulação de Arquivos (fileHandle):** Trabalha com operações em nível de bit (operações bitwise) utilizando máscaras, garantindo que possamos escrever e ler "0"s e "1"s de forma empacotada em bytes (unsigned char).

### Estrutura do Cabeçalho (Header) e Salvamento da Árvore

Para que seja possível descompactar o arquivo no futuro, a própria árvore de Huffman precisa ser salva no arquivo comprimido, além de algumas informações vitais de cabeçalho. O arquivo de saída possui o seguinte formato:

- **Tamanho do Lixo (Trash Size):** Ocupa os primeiros 3 bits do arquivo. Representa os bits extras adicionados ao último byte do arquivo para preenchê-lo, já que só podemos escrever dados em blocos de 8 bits.

- **Tamanho da Árvore (Tree Size):** Ocupa os próximos 13 bits, definindo quantos bytes a árvore ocupará no cabeçalho.

- **Serialização da Árvore:** A árvore é escrita usando percurso em pré-ordem. Nós internos são representados por um asterisco (*). Folhas recebem o caractere original. Caso o próprio caractere seja um asterisco ou uma barra invertida ('\'), ele é escapado com uma barra invertida para evitar colisões durante a remontagem (rebuildTree).

## Contribuição 👨🏽‍💻

1. Helder Nicollas Leão Peixoto
2. Levy Abreu Cavalcante
3. Rafael Luiz Bandeira Almeida

**Este readme está em desenvolvimento... 👀**
