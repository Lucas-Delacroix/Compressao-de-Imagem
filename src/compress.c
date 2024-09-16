#include "compress.h"
#include <stdio.h>
#include <stdlib.h>

// Função para gerar o arquivo binário
void compressQuadTreeToBin(QuadTreeNode *root, int largura, int altura, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Erro ao abrir arquivo de bitstream");
        exit(1);
    }

    fwrite(&largura, sizeof(int), 1, file);
    fwrite(&altura, sizeof(int), 1, file);

    QuadTreeParaBitstream(root, file);
    finalizarBitstream(file);

    fclose(file);
    printf("Compressão binária completa. Arquivo salvo como %s\n", filename);
}

// Função para gerar um arquivo legível em texto com o bitstream
void compressQuadTreeToTxt(QuadTreeNode *root, int largura, int altura, const char *filenameTxt)
{
    FILE *fileTxt = fopen(filenameTxt, "w");
    if (!fileTxt)
    {
        perror("Erro ao abrir arquivo de texto para bitstream");
        exit(1);
    }

    fprintf(fileTxt, "Largura: %d, Altura: %d\n", largura, altura);

    QuadTreeParaTxtBitstream(root, fileTxt);  // Função que grava o bitstream em formato legível

    fclose(fileTxt);
    printf("Compressão para texto completa. Arquivo salvo como %s\n", filenameTxt);
}

// Função original para escrita em binário
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file)
{
    if (node->folha)
    {
        escreverBits(file, 1, 1);           // Indica que é folha
        escreverBits(file, node->valor, 8); // Escreve o valor
    }
    else
    {
        escreverBits(file, 0, 1); // Indica que não é folha
        QuadTreeParaBitstream(node->topLeft, file);
        QuadTreeParaBitstream(node->topRight, file);
        QuadTreeParaBitstream(node->bottomLeft, file);
        QuadTreeParaBitstream(node->bottomRight, file);
    }
}

// Função modificada para gerar o bitstream legível em texto
void QuadTreeParaTxtBitstream(QuadTreeNode *node, FILE *fileTxt)
{
    if (node->folha)
    {
        fprintf(fileTxt, "1, %d\n", node->valor); // Escreve de forma legível
    }
    else
    {
        fprintf(fileTxt, "0\n");
        QuadTreeParaTxtBitstream(node->topLeft, fileTxt);
        QuadTreeParaTxtBitstream(node->topRight, fileTxt);
        QuadTreeParaTxtBitstream(node->bottomLeft, fileTxt);
        QuadTreeParaTxtBitstream(node->bottomRight, fileTxt);
    }
}

// Função de escrita de bits no arquivo binário
void escreverBits(FILE *file, int valor, int numBits)
{
    static int buffer = 0;
    static int bitsDisponiveis = 8;

    while (numBits > 0)
    {
        int bitsParaEscrever = (numBits < bitsDisponiveis) ? numBits : bitsDisponiveis;
        buffer = (buffer << bitsParaEscrever) | (valor >> (numBits - bitsParaEscrever));
        bitsDisponiveis -= bitsParaEscrever;
        numBits -= bitsParaEscrever;
        valor &= (1 << numBits) - 1;

        if (bitsDisponiveis == 0)
        {
            fputc(buffer, file);
            buffer = 0;
            bitsDisponiveis = 8;
        }
    }
}

// Finaliza o bitstream
void finalizarBitstream(FILE *file)
{
    static int buffer = 0;
    static int bitsDisponiveis = 8;

    if (bitsDisponiveis < 8)
    {
        buffer <<= bitsDisponiveis;
        fputc(buffer, file);
    }
}
