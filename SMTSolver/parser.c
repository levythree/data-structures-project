#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 32

typedef enum { NODE_OP, NODE_VAR, NODE_NUM } NodeType;

typedef struct ASTNode {
    NodeType type;
    char token[MAX_LEN];
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

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

ASTNode* createNode(NodeType type, const char* token, int val) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;

    strcpy(node->token, token);

    node->left = NULL;
    node->right = NULL;

    return node;
}

ASTNode* binaryTreeParser(FILE* file) {
    char token[MAX_LEN];

    if (!lexerNextToken(file, token)) return NULL;

    if (strcmp(token, "(") == 0) {
        char op[MAX_LEN];
        lexerNextToken(file, op);

        ASTNode* current_node = createNode(NODE_OP, op, 0);

        current_node->left = binaryTreeParser(file);

        if (lexerPeekChar(file) != ')')
            current_node->right = binaryTreeParser(file);

        while (lexerPeekChar(file) != ')') {
            ASTNode* new_parent = createNode(NODE_OP, op, 0);

            new_parent->left = current_node;
            new_parent->right = binaryTreeParser(file);

            current_node = new_parent;
        }

        lexerNextToken(file, token);
        return current_node;
    }

    if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
        ASTNode* node = createNode(NODE_NUM, token, 0);
        return node;
    } else
        return createNode(NODE_VAR, token, 0);
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

                lexerNextToken(file, type);  // INT

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
                printf(
                    "[Gerenciador] Comando check-sat! Disparando o "
                    "Simplex...\n");
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

int smt(ASTNode** array, int size) {
    for (int i = 0; i < size; i++) {
        }

    return 1;
}

int main() {
    FILE* file = fopen("./file.smt2", "r");
    ASTNode** array = malloc(10 * sizeof(ASTNode));
    int counter = 0;

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    parseSMTFile(file, array, &counter);

    printf("%d\n", counter);

    if (smt(array, counter))
        printf("SAT\n");
    else
        printf("UNSAT\n");

    fclose(file);

    return 0;
}