#ifndef QUADTREE_H
#define QUADTREE_H
#include "pgm.h"


typedef struct QuadTreeNode {
    int x, y;  
    int largura, altura;  
    int valor;  
    int folha;  
    struct QuadTreeNode *topLeft, *topRight, *bottomLeft, *bottomRight;  
} QuadTreeNode;



QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha);
QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura);
void imprimirQuadTree(QuadTreeNode *node);
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);

#endif