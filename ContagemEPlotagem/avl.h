#ifndef avlH
#define avlH

typedef struct avl {
    int value;
    int height;
    struct avl *left;
    struct avl *right;
} AVL;

AVL* createAVL(int value);

int getHeight(AVL* node);

int max(int a, int b);

int getBalanceFactor(AVL* node);

int getUpdatedHeight(AVL* node);

AVL* rotateLeft(AVL* node);

AVL* rotateRight(AVL* node);

AVL* addAVL(AVL* root, int value);

int searchAVL(AVL* root, int value);

#endif
