#ifndef QUADTREE_H
#define QUADTREE_H
#include "pgm.h"
// Estrutura de nó da QuadTree
typedef struct QuadTreeNode {
    int x, y;  // Coordenadas do canto superior esquerdo do bloco
    int largura, altura;  // Dimensões do bloco
    int valor;  // Valor médio do bloco (por simplicidade)
    int folha;  // Indica se é um nó folha
    struct QuadTreeNode *topLeft, *topRight, *bottomLeft, *bottomRight;  // Filhos
} QuadTreeNode;


// Protótipo da função criarNo
QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha);
QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura);
void imprimirQuadTree(QuadTreeNode *node);
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);

#endif