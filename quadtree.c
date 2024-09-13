#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"

#define MAX_DIFF 10  // Diferença máxima permitida para considerar um bloco homogêneo

// Função para construir a quadtree a partir da imagem
QuadtreeNode* construirQuadTree(unsigned char *pData, int x, int y, int largura, int altura, int imgLargura) {
    //Alocação dinamica para cada nó da quadtree
    QuadtreeNode *node = (QuadtreeNode *)malloc(sizeof(QuadtreeNode));

    //Verificar homogeanidade do bloco atual
    if (homogeneo(pData, x, y, largura, altura, imgLargura)) {
        //Se for homogêneo, marca como nó folha e atribue a cor media
        node->nodoFolha = 1;
        node->color = media(pData, x, y, largura, altura, imgLargura);
        node->topLeft = node->topRight = node->bottomLeft = node->bottomRight = NULL;
    } else {
        //Se não for homogêneo, divide o bloco em quatro sub-blocos
        node->nodoFolha = 0;
        int metadeLargura = largura / 2;
        int metadeAltura = altura / 2;

        //Criar os quatros filhos recursivamente 
        node->topLeft = construirQuadTree(pData, x, y, metadeLargura, metadeAltura, imgLargura);
        node->topRight = construirQuadTree(pData, x, y + metadeLargura, metadeLargura, metadeAltura, imgLargura);
        node->bottomLeft = construirQuadTree(pData, x + metadeAltura, y, metadeLargura, metadeAltura, imgLargura);
        node->bottomRight = construirQuadTree(pData, x + metadeAltura, y + metadeLargura, metadeLargura, metadeAltura, imgLargura);
    }

    return node;
}

// Função para verificar se o bloco é homogêneo
int homogeneo(unsigned char *pData, int x, int y, int largura, int altura, int imgLargura) {
    //Primeiro pixel do bloco pra comparação
    unsigned char first = pData[x * imgLargura + y];

    //Verificar se todos os pixels do bloco estão dentro da diferença permitida
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            //Se a diferença for maior que MAX_DIFF, o bloco não é homogêneo
            if (abs(pData[(x + i) * imgLargura + (y + j)] - first) > MAX_DIFF)
                return 0;
        }
    }
    return 1;
}

// Função para obter a média de tons de cinza em um bloco
unsigned char media(unsigned char *pData, int x, int y, int largura, int altura, int imgLargura) {
    int soma = 0;
    //Somar todos os valores de tons de cinza do bloco
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            soma += pData[(x + i) * imgLargura + (y + j)];
        }
    }
    //Retornar Media
    return soma / (largura * altura);
}

// Função para escrever a quadtree no arquivo
void escreverQuadtree(QuadtreeNode *node, FILE *fp) {
    if (node->nodoFolha) {
        //Se for um nó Folha, grava a informação no nodoFolha e a cor 
        fwrite(&node->nodoFolha, sizeof(int), 1, fp);
        fwrite(&node->color, sizeof(unsigned char), 1, fp);
    } else {
        //Se não for folha, grava a informação no nodoNaoFolha e escreve recusrivamente os filhos
        fwrite(&node->nodoFolha, sizeof(int), 1, fp);
        escreverQuadTree(node->topLeft, fp);
        escreverQuadTree(node->topRight, fp);
        escreverQuadTree(node->bottomLeft, fp);
        escreverQuadTree(node->bottomRight, fp);
    }
}

// Função para liberar a memória da quadtree
void freeQuadtree(QuadtreeNode *node) {
    if (node != NULL) {
        //libera rescursivamente todos os filhos do nó
        freeQuadtree(node->topLeft);
        freeQuadtree(node->topRight);
        freeQuadtree(node->bottomLeft);
        freeQuadtree(node->bottomRight);
        free(node);
    }
}
