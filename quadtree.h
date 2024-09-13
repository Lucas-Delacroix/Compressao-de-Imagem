//Garantir que o conteudo não seja processado mais de uma vez durante a compilação

//Por isso usaremos ifndef
#ifndef QUADTREE_H
#define QUADTREE_H
#include "pgm.h" // acessando a estrutura 'pgm'

typedef struct QuadtreeNode{
    int nodoFolha; //indica se o nó é uma região homogênea
    unsigned char color; //valor da cor se for uma região homogênea
    struct QuadtreeNode *topLeft, *topRight, *bottomLeft, *bottomRight;
    
}QuadtreeNode;

//Função para construir a quadtree a partir de uma imagem PGM
QuadtreeNode* construirQuadTree (unsigned char *pData, int x, int y, int largura, int altura, int imgLargura );


//Função para gravar a quadtree em um arquivo
void escreverQuadTree(QuadtreeNode * node, FILE *fp);

//Função para ler a quadtree de um arquivo
QuadtreeNode* lerQuadtree(FILE *fp);

//Função para liberar a memória do quadtree
void freeQuadTree(QuadtreeNode *node);

#endif