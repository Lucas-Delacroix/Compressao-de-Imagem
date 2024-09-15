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

    // Converte a QuadTree para bitstream
    QuadTreeParaBitstream(root, file);

    // Finaliza o bitstream (escreve o byte incompleto, se necessário)
    finalizarBitstream(file);

    fclose(file);
    printf("Compressão completa. Arquivo salvo como %s\n", filename);
}

void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file)
{
    if (node->folha)
    {
        escreverBits(file, 1, 1);           // Indica que é um nó folha
        escreverBits(file, node->valor, 8); // Escreve o valor do nó folha (8 bits)
    }
    else
    {
        escreverBits(file, 0, 1); // Indica que é um nó interno
        // Chama recursivamente para os quatro filhos
        QuadTreeParaBitstream(node->topLeft, file);
        QuadTreeParaBitstream(node->topRight, file);
        QuadTreeParaBitstream(node->bottomLeft, file);
        QuadTreeParaBitstream(node->bottomRight, file);
    }
}

// Função para fechar qualquer byte incompleto no arquivo
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


void compressImage(struct pgm *pio){
	int x =0;
	// unsigned char data = *(pio->pData);
	
	// Loop for para percorrer a matriz inteira
	for (int k = 0; k < (pio->largura * pio->altura); k++){
        for (x = 0; x < (pio->largura) / 2; x++){
            *(pio->pData + k) = 255;
            k++;
        }
        k += (pio->largura) / 2 - 1; // Subtraímos 1 porque o for principal também incrementa k
    }
}

// Função para escrever bits no arquivo de bitstream
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
