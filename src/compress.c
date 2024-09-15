#include "compress.h"
#include <stdio.h>
#include <stdlib.h>

void compressQuadTree(QuadTreeNode *root, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Erro ao abrir arquivo de bitstream");
        exit(1);
    }

    
    QuadTreeParaBitstream(root, file);

    
    finalizarBitstream(file);

    fclose(file);
    printf("Compressão completa. Arquivo salvo como %s\n", filename);
}

void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file)
{
    if (node->folha)
    {
        escreverBits(file, 1, 1);           
        escreverBits(file, node->valor, 8); 
    }
    else
    {
        escreverBits(file, 0, 1); 
        QuadTreeParaBitstream(node->topLeft, file);
        QuadTreeParaBitstream(node->topRight, file);
        QuadTreeParaBitstream(node->bottomLeft, file);
        QuadTreeParaBitstream(node->bottomRight, file);
    }
}

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
