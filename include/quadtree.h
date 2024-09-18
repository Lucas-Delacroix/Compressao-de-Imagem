#ifndef QUADTREE_H
#define QUADTREE_H
#include "pgm.h"


typedef struct QuadTreeNode {
    int x, y;  
    int largura, altura;  
    int valor;  
    int folha;  
    struct QuadTreeNode *superiorEsquerdo, *superiorDireito, *inferiorEsquerdo, *inferiorDireito;  
} QuadTreeNode;



QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha);
QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura);
void imprimirQuadTree(QuadTreeNode *node);
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
void liberarQuadTree(QuadTreeNode *node);
float calcularDesvioPadrao(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
float calcularMAE(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);

#endif