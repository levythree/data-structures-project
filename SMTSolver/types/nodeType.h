#ifndef nodeTypeh
#define nodeTypeh

typedef enum {
    NODE_VAR,
    NODE_NUM,
    NODE_OP_ADD,
    NODE_OP_SUB,
    NODE_OP_EQ,
    NODE_OP_LE,
    NODE_OP_GE,
    NODE_OP_MUL,
    NODE_OP_OR
} NodeType;

#endif