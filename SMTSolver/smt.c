#include "./types/smt.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types/lia.h"
#include "./types/linearAtom.h"
#include "./types/sat.h"

bool lexerNextToken(FILE* file, char* token) {
    int character;

    while (true) {
        character = fgetc(file);
        if (character == EOF) return false;

        if (isspace(character)) continue;

        if (character == ';') {
            while ((character = fgetc(file)) != '\n' && character != EOF);
            continue;
        }

        break;
    }

    if (character == '(' || character == ')') {
        token[0] = character;
        token[1] = '\0';
        return true;
    }

    int len = 0;
    while (character != EOF && !isspace(character) && character != '(' &&
           character != ')') {
        if (len < MAX_LEN - 1) token[len++] = character;

        character = fgetc(file);
    }
    token[len] = '\0';

    if (character != EOF) ungetc(character, file);

    return true;
}

int lexerPeekChar(FILE* file) {
    int character;

    while (true) {
        character = fgetc(file);
        if (character == EOF) return EOF;

        if (isspace(character)) continue;

        if (character == ';') {
            while ((character = fgetc(file)) != '\n' && character != EOF);
            continue;
        }

        break;
    }

    if (character != EOF) ungetc(character, file);

    return character;
}

ASTNode* createNode(NodeType type, const char* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;

    strcpy(node->token, token);

    node->left = NULL;
    node->right = NULL;

    return node;
}

NodeType getNodeType(char* token) {
    if (!strcmp(token, "+")) return NODE_OP_ADD;
    if (!strcmp(token, "-")) return NODE_OP_SUB;
    if (!strcmp(token, "=")) return NODE_OP_EQ;
    if (!strcmp(token, "<=")) return NODE_OP_LE;
    if (!strcmp(token, ">=")) return NODE_OP_GE;
    if (!strcmp(token, "*")) return NODE_OP_MUL;
    if (!strcmp(token, "or")) return NODE_OP_OR;

    return NODE_VAR;
}

ASTNode* binaryTreeParser(FILE* file) {
    char token[MAX_LEN];

    int nextChar = lexerPeekChar(file);

    if (nextChar == '(') {
        lexerNextToken(file, token);

        char op[MAX_LEN];
        lexerNextToken(file, op);

        NodeType opType = getNodeType(op);
        ASTNode* current_node = createNode(opType, op);

        current_node->left = binaryTreeParser(file);

        if (lexerPeekChar(file) != ')')
            current_node->right = binaryTreeParser(file);

        while (lexerPeekChar(file) != ')') {
            ASTNode* new_parent = createNode(opType, op);
            new_parent->left = current_node;
            new_parent->right = binaryTreeParser(file);
            current_node = new_parent;
        }

        lexerNextToken(file, token);
        return current_node;
    }

    if (!lexerNextToken(file, token)) return NULL;

    if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        return createNode(NODE_NUM, token);
    else
        return createNode(NODE_VAR, token);
}

void printInOrder(ASTNode* tree, int depth) {
    if (!tree) return;

    printInOrder(tree->right, depth + 1);

    for (int i = 0; i < depth; i++) {
        printf("        ");
    }

    if (strcmp(tree->token, "-") == 0 && tree->right == NULL)
        printf("[ - unário ]\n");
    else
        printf("[ %s ]\n", tree->token);

    printInOrder(tree->left, depth + 1);
}

void parseSMTFile(FILE* file, ASTNode** array, int* counter) {
    char token[32];

    while (lexerNextToken(file, token)) {
        if (strcmp(token, "(") == 0) {
            char command[32];
            lexerNextToken(file, command);

            if (strcmp(command, "declare-const") == 0) {
                char variable[32];
                char type[32];

                lexerNextToken(file, variable);
                lexerNextToken(file, type);

                lexerNextToken(file, token);
                printf("[Gerenciador] Registrada variável inteira: %s\n",
                       variable);

            } else if (strcmp(command, "assert") == 0) {
                printf("[Gerenciador] Entrando na região do assert...\n");

                ASTNode* tree = binaryTreeParser(file);
                array[(*counter)++] = tree;

                printf(
                    "[Gerenciador] Árvore do assert construída com sucesso!\n");

                lexerNextToken(file, token);

            } else if (strcmp(command, "check-sat") == 0) {
                printf("[Gerenciador] Comando check-sat detectado!\n");

                lexerNextToken(file, token);

            } else {
                int openedParentheses = 1;
                while (openedParentheses > 0) {
                    lexerNextToken(file, token);
                    if (strcmp(token, "(") == 0) openedParentheses++;
                    if (strcmp(token, ")") == 0) openedParentheses--;
                }
            }
        }
    }
}

void initSymbolTable(SymbolTable* table) { table->totalVariables = 0; }

int getOrCreateVariableColumn(SymbolTable* table, char* name) {
    for (int i = 0; i < table->totalVariables; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return table->vars[i].column;
        }
    }

    strcpy(table->vars[table->totalVariables].name, name);
    table->vars[table->totalVariables].column = table->totalVariables;
    table->totalVariables++;

    return table->totalVariables - 1;
}

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

LinearAtom extractLinearAtom(ASTNode* node, SymbolTable* table) {
    Tableau* tempTab = createTableau(table->totalVariables, 1);

    if (node->type == NODE_OP_LE) {
        linearizeExpression(node->left, tempTab, 0, 1.0, table);
        linearizeExpression(node->right, tempTab, 0, -1.0, table);

    } else if (node->type == NODE_OP_GE) {
        linearizeExpression(node->left, tempTab, 0, -1.0, table);
        linearizeExpression(node->right, tempTab, 0, 1.0, table);

    } else if (node->type == NODE_OP_EQ) {
        linearizeExpression(node->left, tempTab, 0, 1.0, table);
        linearizeExpression(node->right, tempTab, 0, -1.0, table);
    }

    double* coefficients =
        (double*)calloc(table->totalVariables, sizeof(double));

    for (int i = 0; i < table->totalVariables; i++) {
        coefficients[i] = tempTab->matrix[0][i];
    }

    double rhs = -tempTab->matrix[0][tempTab->columns - 1];

    LinearAtom atom =
        createLinearAtom(table->totalVariables, coefficients, rhs);
    atom.type = node->type;

    free(coefficients);
    freeTableau(tempTab);

    return atom;
}

void mapAstToCnfBuffer(ASTNode* node, LinearAtom* linearAtoms, int totalAtoms,
                       int* clauseBuffer, int* literalCount) {
    if (!node) return;

    if (node->type == NODE_OP_OR) {
        mapAstToCnfBuffer(node->left, linearAtoms, totalAtoms, clauseBuffer,
                          literalCount);
        mapAstToCnfBuffer(node->right, linearAtoms, totalAtoms, clauseBuffer,
                          literalCount);
        return;
    }

    if (node->type == NODE_OP_LE || node->type == NODE_OP_GE ||
        node->type == NODE_OP_EQ) {
        for (int i = 0; i < totalAtoms; i++) {
            if (linearAtoms[i].type == node->type &&
                !strcmp(linearAtoms[i].coefficients != NULL ? node->token : "",
                        node->token)) {
                clauseBuffer[*literalCount] = i + 1;
                (*literalCount)++;
                return;
            }
        }
    }
}

bool satSMT(Formula* formula, int* interpretation, int currentVariable,
            SATNode** satNode, SymbolTable* table, LinearAtom* linearAtoms) {}

bool smt(ASTNode** array, int assertionsCount, LinearAtom* linearAtoms,
         int linearAtomsCount, SymbolTable* table) {
    if (!assertionsCount) return true;

    Formula* formula = createFormula(linearAtomsCount, assertionsCount);

    int* temporaryBuffer = (int*)malloc(linearAtomsCount * sizeof(int));
    int atomTracker = 0;
    for (int i = 0; i < assertionsCount; i++) {
        int literalCount = 0;

        mapAstToCnfBuffer(array[i], linearAtoms, linearAtomsCount,
                          temporaryBuffer, &literalCount);

        if (literalCount == 0) {
            temporaryBuffer[0] = ++atomTracker;
            literalCount = 1;
        } else {
            atomTracker += literalCount;
        }

        initClause(&formula->clauses[i], temporaryBuffer, literalCount);
    }

    free(temporaryBuffer);

    printf("[SMT] Fórmula booleana gerada com sucesso para o SAT Solver!\n");
    printf("[SMT] Cláusulas lógicas: %d, Variáveis booleanas (Átomos): %d\n",
           formula->numberOfClauses, formula->numberOfVariables);

    int* interpretation =
        (int*)calloc(formula->numberOfVariables + 1, sizeof(int));
    SATNode* satTree = NULL;

    bool result =
        satSMT(formula, interpretation, 1, &satTree, table, linearAtoms);
    free(interpretation);

    // return result;
    return true;
}

void parseToLinearAtoms(ASTNode* node, LinearAtom* linearAtoms, int* counter,
                        SymbolTable* table) {
    if (!node) return;

    if (node->type == NODE_OP_OR) {
        parseToLinearAtoms(node->left, linearAtoms, counter, table);
        parseToLinearAtoms(node->right, linearAtoms, counter, table);
        return;
    }

    if (node->type == NODE_OP_LE || node->type == NODE_OP_GE ||
        node->type == NODE_OP_EQ) {
        linearAtoms[*counter] = extractLinearAtom(node, table);
        (*counter)++;
        return;
    }
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "r");

    ASTNode** array = malloc(10 * sizeof(ASTNode*));
    LinearAtom* linearAtoms = malloc(100 * sizeof(LinearAtom));
    SymbolTable table;
    initSymbolTable(&table);

    int assertionsCount = 0;
    int linearAtomsCount = 0;

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        free(array);
        return 1;
    }

    parseSMTFile(file, array, &assertionsCount);

    for (int i = 0; i < assertionsCount; i++) {
        linearizeExpression(array[i], NULL, 0, 1.0, &table);
        parseToLinearAtoms(array[i], linearAtoms, &linearAtomsCount, &table);
    }

    if (smt(array, assertionsCount, linearAtoms, linearAtomsCount, &table))
        printf("SAT\n");
    else
        printf("UNSAT\n");

    fclose(file);

    return 0;
}