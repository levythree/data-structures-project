#include <stdio.h>
#include <stdlib.h>

#include "./types/ast.h"
#include "./types/parser.h"
#include "./types/smt.h"
#include "./types/atom.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Erro: Nenhum arquivo fornecido.\n");
        printf("Uso: %s <caminho_para_arquivo.smt2>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int MAX_ASSERTIONS = 1000;
    int MAX_ATOMS = 2000;
    
    ASTNode** astNodes = malloc(MAX_ASSERTIONS * sizeof(ASTNode*));
    LinearAtom* linearAtoms = malloc(MAX_ATOMS * sizeof(LinearAtom));
    
    SymbolTable table;
    initSymbolTable(&table);

    int assertionsCount = 0;
    int linearAtomsCount = 0;

    parseSMTFile(file, astNodes, &assertionsCount);
    fclose(file); 

    desugarEqualities(astNodes, &assertionsCount);

    for (int i = 0; i < assertionsCount; i++) {
        if (linearAtomsCount >= MAX_ATOMS - 2) {
            printf("Erro: Muitas equações para a memória alocada!\n");

            break;
        }

        linearizeExpression(astNodes[i], NULL, 0, 1.0, &table);

        parseToLinearAtoms(astNodes[i], linearAtoms, &linearAtomsCount, &table);
    }

    if (!smt(astNodes, assertionsCount, linearAtoms, linearAtomsCount, &table)) printf("UNSAT!\n");

    for (int i = 0; i < assertionsCount; i++) {
        freeAST(astNodes[i]);
    }

    free(astNodes);

    for (int i = 0; i < linearAtomsCount; i++) {
        freeLinearAtom(&linearAtoms[i]);
    }

    free(linearAtoms);

    return 0;
}
