#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define tolerancia 40
#define toleranciaDP 5 // Maior, mais perda
#define toleranciaEMQ 100 // Maior, mais perda


QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha) {
    QuadTreeNode* node = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));
    node->x = x;
    node->y = y;
    node->largura = largura;
    node->altura = altura;
    node->valor = valor;
    node->folha = folha;
    node->superiorEsquerdo = node->superiorDireito = node->inferiorEsquerdo = node->inferiorDireito = NULL;
    return node;
}


QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura) {
    unsigned char *data = pio->pData;
    int larguraImg = pio->largura;
    
    
    if (blocoHomogeneo(data, larguraImg, x, y, largura, altura) || (largura == 1 && altura == 1)) {
        int media = calcularMedia(data, larguraImg, x, y, largura, altura);
        return criarNo(x, y, largura, altura, media, 1);  
    }
    
    
    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    QuadTreeNode* node = criarNo(x, y, largura, altura, 0, 0);  // Nó interno

    
    node->superiorEsquerdo = construirQuadTree(pio, x, y, metadeLargura, metadeAltura);
    node->superiorDireito = construirQuadTree(pio, x + metadeLargura, y, largura - metadeLargura, metadeAltura);
    node->inferiorEsquerdo = construirQuadTree(pio, x, y + metadeAltura, metadeLargura, altura - metadeAltura);
    node->inferiorDireito = construirQuadTree(pio, x + metadeLargura, y + metadeAltura, largura - metadeLargura, altura - metadeAltura);

    return node;
}


void imprimirQuadTree(QuadTreeNode *node) {
    if (node == NULL) return;

    if (node->folha) {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d - Valor: %d (Folha)\n", node->x, node->y, node->largura, node->altura, node->valor);
    } else {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d (Nó Interno)\n", node->x, node->y, node->largura, node->altura);
        imprimirQuadTree(node->superiorEsquerdo);
        imprimirQuadTree(node->superiorDireito);
        imprimirQuadTree(node->inferiorEsquerdo);
        imprimirQuadTree(node->inferiorDireito);
    }
}


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
    int soma = 0;
    int somaQuadrados = 0;
    int totalPixels = largura * altura;

    // Calcula a soma dos valores dos pixels e a soma dos quadrados dos valores
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valorAtual = data[(y + i) * larguraImg + (x + j)];
            soma += valorAtual;
            somaQuadrados += valorAtual * valorAtual;
        }
    }

    // Calcula a média e a média dos quadrados
    float media = soma / (float)totalPixels;
    float mediaQuadrados = somaQuadrados / (float)totalPixels;

    // Calcula o desvio padrão
    float desvioPadrao = sqrt(mediaQuadrados - media * media);

    // Calcula o erro médio quadrático
    float erroMedioQuadratico = 0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valorAtual = data[(y + i) * larguraImg + (x + j)];
            erroMedioQuadratico += (valorAtual - media) * (valorAtual - media);
        }
    }
    erroMedioQuadratico /= totalPixels;

    // Verifica se o bloco é homogêneo com base nas tolerâncias
    if (desvioPadrao <= toleranciaDP && erroMedioQuadratico <= toleranciaEMQ) {
        return 1;  // Bloco é considerado homogêneo
    }

    return 0;  // Bloco não é homogêneo
}
