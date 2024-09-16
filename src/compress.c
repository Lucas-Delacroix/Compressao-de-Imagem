#include "compress.h"
#include <stdio.h>
#include <stdlib.h>

static int buffer = 0; //Vai armazenar os bits temporários que serão escritos no arquivo
static int bitsDisponiveis = 8; //quantos bits ainda podem ser armazenados no buffer antes de precisar escrever os dados no arquivo.
//Neste caso um byte


// Função para gerar o arquivo binário
void compressQuadTreeToBin(QuadTreeNode *root, int largura, int altura, const char *filename)
{
    
    FILE *file = fopen(filename, "wb");// Com "wb" abre o arquivo para escrita, truncando-o se já existir.
    if (!file)
    {
        perror("Erro ao abrir arquivo de bitstream");
        exit(1);
    }
    //Escreve no arquivo o valor no endereço de largura ou altura, aloca tamanho de inteiro e adiciona '1' elemento, largura ou altura
    //ao arquivo
    fwrite(&largura, sizeof(int), 1, file);
    fwrite(&altura, sizeof(int), 1, file);

    QuadTreeParaBitstream(root, file);
    finalizarBitstream(file);

    fclose(file);
    printf("Compressão binária completa. Arquivo salvo como %s\n", filename);
}

// Função que vai gerar um arquivo legível em texto com o bitstream
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

// Função para escrever bitstream em binário
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
        QuadTreeParaBitstream(node->superiorEsquerdo, file);
        QuadTreeParaBitstream(node->superiorDireito, file);
        QuadTreeParaBitstream(node->inferiorEsquerdo, file);
        QuadTreeParaBitstream(node->inferiorDireito, file);
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
        QuadTreeParaTxtBitstream(node->superiorEsquerdo, fileTxt);
        QuadTreeParaTxtBitstream(node->superiorDireito, fileTxt);
        QuadTreeParaTxtBitstream(node->inferiorEsquerdo, fileTxt);
        QuadTreeParaTxtBitstream(node->inferiorDireito, fileTxt);
    }
}

// Função de escrita de bits no arquivo binário
void escreverBits(FILE *file, int valor, int numBits)
{
    //A função decide se escreve todos os bits que foram passados ou apenas uma parte
    //Se o buffer já tenha alguns bits e esteja parcialmente preenchido.
    while (numBits > 0)
    {
        int bitsParaEscrever = (numBits < bitsDisponiveis) ? numBits : bitsDisponiveis;
        //Desloca os bits no buffer para a esquerda, 
        //Desloca o valor original para a direita
        // | (pipe) Combina o buffer com os novos bits que serão escritos.
        buffer = (buffer << bitsParaEscrever) | (valor >> (numBits - bitsParaEscrever));
        bitsDisponiveis -= bitsParaEscrever;
        numBits -= bitsParaEscrever;
        
        //garante que apenas os bits restantes do valor serão considerados na próxima iteração. 
        //Zeran todos os bits acima de numBits.
        valor &= (1 << numBits) - 1;

        if (bitsDisponiveis == 0)
        {
            fputc(buffer, file);
            buffer = 0;
            bitsDisponiveis = 8;
        }
    }
}

//Vai garantir que os bits que restaram no buffer sejam escritos no arquivo.
void finalizarBitstream(FILE *file)
{

    if (bitsDisponiveis < 8)
    {
        buffer <<= bitsDisponiveis;
        fputc(buffer, file);
    }

    // Resetar as variáveis
    buffer = 0;
    bitsDisponiveis = 8;
}
