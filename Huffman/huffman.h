#ifndef huffmanH
#define huffmanH

typedef struct huffmanNode {
    unsigned char character;
    int frequency;
    struct huffmanNode* left;
    struct huffmanNode* right;
} HuffmanNode;

#endif
