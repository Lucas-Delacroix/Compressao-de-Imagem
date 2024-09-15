#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#define tolerancia 100

// Criar Nó
QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha) {
    QuadTreeNode* node = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));
    node->x = x;
    node->y = y;
    node->largura = largura;
    node->altura = altura;
    node->valor = valor;
    node->folha = folha;
    node->topLeft = node->topRight = node->bottomLeft = node->bottomRight = NULL;
    return node;
}

// Função recursiva para construir a QuadTree
QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura) {
    unsigned char *data = pio->pData;
    int larguraImg = pio->largura;
    
    // Se o bloco for homogêneo ou de tamanho mínimo, cria um nó folha
    if (blocoHomogeneo(data, larguraImg, x, y, largura, altura) || (largura == 1 && altura == 1)) {
        int media = calcularMedia(data, larguraImg, x, y, largura, altura);
        return criarNo(x, y, largura, altura, media, 1);  // Nó folha
    }
    
    // Dividir o bloco em quatro sub-blocos
    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    QuadTreeNode* node = criarNo(x, y, largura, altura, 0, 0);  // Nó interno

    // Chamada recursiva para os quatro sub-blocos
    node->topLeft = construirQuadTree(pio, x, y, metadeLargura, metadeAltura);
    node->topRight = construirQuadTree(pio, x + metadeLargura, y, largura - metadeLargura, metadeAltura);
    node->bottomLeft = construirQuadTree(pio, x, y + metadeAltura, metadeLargura, altura - metadeAltura);
    node->bottomRight = construirQuadTree(pio, x + metadeLargura, y + metadeAltura, largura - metadeLargura, altura - metadeAltura);

    return node;
}

// Função para imprimir a QuadTree (para visualização)
void imprimirQuadTree(QuadTreeNode *node) {
    if (node == NULL) return;

    if (node->folha) {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d - Valor: %d (Folha)\n", node->x, node->y, node->largura, node->altura, node->valor);
    } else {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d (Nó Interno)\n", node->x, node->y, node->largura, node->altura);
        imprimirQuadTree(node->topLeft);
        imprimirQuadTree(node->topRight);
        imprimirQuadTree(node->bottomLeft);
        imprimirQuadTree(node->bottomRight);
    }
}

// Função para calcular a média dos valores no bloco
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int soma = 0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            soma += data[(y + i) * larguraImg + (x + j)];
        }
    }
    return soma / (largura * altura);
}

int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int primeiroValor = data[y * larguraImg + x];
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valorAtual = data[(y + i) * larguraImg + (x + j)];
            // Verifica se a diferença entre os valores está dentro da tolerância
            if (abs(valorAtual - primeiroValor) > tolerancia) {
                return 0;  // Bloco não homogêneo
            }
        }
    }
    return 1;  // Bloco homogêneo
}

