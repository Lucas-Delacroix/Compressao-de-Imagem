/************************************************************************** */
/* Aluno: Lucas Delacroix Alves do Rêgo
* Guilherme Pereira de Souza
* Antonio Lucas Barbosa Salvador
* Louise Sampaio Araújo Gonçalves
* Avaliacao 04: Trabalho Final
* 2024.1 - Prof. Daniel Ferreira
* Compilador:gcc (Ubuntu 11.4.0-1ubuntu1~22.04)
***************************************************************************************/
#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define toleranciaDP 5 // Maior, mais perda

QuadTreeNode *criarNo(int x, int y, int largura, int altura, int valor, int folha)
{
    QuadTreeNode *node = (QuadTreeNode *)malloc(sizeof(QuadTreeNode));
    node->x = x;
    node->y = y;
    node->largura = largura;
    node->altura = altura;
    node->valor = valor;
    node->folha = folha;

    // Como é uma folha, não possui filhos, logo será NULL.
    node->superiorEsquerdo = node->superiorDireito = node->inferiorEsquerdo = node->inferiorDireito = NULL;
    return node;
}

QuadTreeNode *construirQuadTree(struct pgm *img, int x, int y, int largura, int altura)
{
    unsigned char *data = img->pData;
    int larguraImg = img->largura;

    if (blocoHomogeneo(data, larguraImg, x, y, largura, altura) || (largura == 1 && altura == 1))
    {
        int media = calcularMedia(data, larguraImg, x, y, largura, altura);
        // 'media' será usado como o valor do nível de cinza do nó da imagem.
        return criarNo(x, y, largura, altura, media, 1);
    }

    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    // Como não é um nó folha não receberá valor
    QuadTreeNode *node = criarNo(x, y, largura, altura, 0, 0); // Nó interno

    // Acessa os membros desse nó para criar seus filhos.
    node->superiorEsquerdo = construirQuadTree(img, x, y, metadeLargura, metadeAltura);
    node->superiorDireito = construirQuadTree(img, x + metadeLargura, y, largura - metadeLargura, metadeAltura);
    node->inferiorEsquerdo = construirQuadTree(img, x, y + metadeAltura, metadeLargura, altura - metadeAltura);
    node->inferiorDireito = construirQuadTree(img, x + metadeLargura, y + metadeAltura, largura - metadeLargura, altura - metadeAltura);

    // Retorna um ponteiro para um QuadTreeNode
    return node;
}

void imprimirQuadTree(QuadTreeNode *node)
{
    if (node == NULL)
        return;
    // Exibe informação dos nós de forma recursiva.
    if (node->folha)
    {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d - Valor: %d (Folha)\n", node->x, node->y, node->largura, node->altura, node->valor);
    }
    else
    {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d (Nó Interno)\n", node->x, node->y, node->largura, node->altura);
        imprimirQuadTree(node->superiorEsquerdo);
        imprimirQuadTree(node->superiorDireito);
        imprimirQuadTree(node->inferiorEsquerdo);
        imprimirQuadTree(node->inferiorDireito);
    }
}

// Calcula a media para utilizar como parametro posteriormente
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura)
{
    int soma = 0;
    for (int i = 0; i < altura; i++)
    {
        for (int j = 0; j < largura; j++)
        {
            soma += *(data + ((y + i) * larguraImg + (x + j)));
        }
    }
    return soma / (largura * altura);
}

int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura)
{
    int soma = 0;
    int somaQuadrados = 0;
    int totalPixels = largura * altura;

    // Calcula a soma dos valores dos pixels e a soma dos quadrados dos valores
    for (int i = 0; i < altura; i++)
    {
        for (int j = 0; j < largura; j++)
        {
            int valorAtual = *(data + ((y + i) * larguraImg + (x + j)));
            soma += valorAtual;
            somaQuadrados += valorAtual * valorAtual;
        }
    }

    // Calcula a média e a média dos quadrados
    float media = soma / (float)totalPixels;
    float mediaQuadrados = somaQuadrados / (float)totalPixels;

    // Calculo do desvio padrão
    float desvioPadrao = sqrt(mediaQuadrados - media * media);

    // Verifica se o bloco é homogêneo com base nas tolerâncias
    if (desvioPadrao <= toleranciaDP)
    {
        return 1; // Bloco é considerado homogêneo
    }

    return 0; // Bloco não é homogêneo
}

void liberarQuadTree(QuadTreeNode *node)
{
    if (node == NULL)
    {
        return;
    }

    // Libera os filhos recursivamente
    liberarQuadTree(node->superiorEsquerdo);
    liberarQuadTree(node->superiorDireito);
    liberarQuadTree(node->inferiorEsquerdo);
    liberarQuadTree(node->inferiorDireito);

    // Libera o próprio nó
    free(node);
}