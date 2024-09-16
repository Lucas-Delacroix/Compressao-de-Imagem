/************************************************************************** */
/* Aluno: Lucas Delacroix
* Guilherme Pereira
* Antonio Lucas
* Louse
* Avaliacao 04: Trabalho Final
* 2024.1 - Prof. Daniel Ferreira
* Compilador:gcc (Ubuntu 11.4.0-1ubuntu1~22.04)
***************************************************************************************/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decodificacao.h"

//Mesma coisa da compressao em compress.c
static int buffer = 0;
static int bitsDisponiveis = 0;



int lerBits(FILE *file, int numBits)
{
	int resultado = 0;

	while (numBits > 0)
	{
		if (bitsDisponiveis == 0)
		{
			buffer = fgetc(file);
			if (buffer == EOF)
			{
				return -1; // Fim do arquivo
			}
			bitsDisponiveis = 8;
		}

		int bitsParaLer = (numBits < bitsDisponiveis) ? numBits : bitsDisponiveis;
		resultado = (resultado << bitsParaLer) | (buffer >> (bitsDisponiveis - bitsParaLer));
		bitsDisponiveis -= bitsParaLer;
		numBits -= bitsParaLer;
		buffer &= (1 << bitsDisponiveis) - 1; // Limpa os bits lidos
	}

	return resultado;
}


void decode(FILE *file, int x, int y, int altura, int largura, struct pgm *pio)
{
	int folha = lerBits(file, 1); // Ler 1 bit para verificar se é nó folha
	if (folha == 1)
	{
		int media = lerBits(file, 8); // Ler 8 bits para o valor médio
		if (media == -1)
		{
			printf("Erro: fim de arquivo atingido durante a leitura do valor médio.\n");
			return;
		}
		for (int i = 0; i < altura; i++)
		{
			for (int j = 0; j < largura; j++)
			{
				int posX = x + j;
				int posY = y + i;
				pio->pData[posY * pio->largura + posX] = media;
			}
		}
	}
	else
	{
		int metadeAltura = altura / 2;
		int metadeLargura = largura / 2;
		if(metadeAltura % 2 != 0){
			metadeAltura+=1;
		}
		if(metadeLargura % 2 != 0){
			metadeLargura+=1;
		}

		// Chamar recursivamente para os quatro sub-blocos
		decode(file, x, y, metadeAltura, metadeLargura, pio);													// superiorEsquerda
		decode(file, x + metadeLargura, y, metadeAltura, largura - metadeLargura, pio);							// superiorDireita
		decode(file, x, y + metadeAltura, altura - metadeAltura, metadeLargura, pio);							// inferiorEsquerda
		decode(file, x + metadeLargura, y + metadeAltura, altura - metadeAltura, largura - metadeLargura, pio); // inferiorDireita
	}
}


void reconstruirImagem(const char *filename, struct pgm *img)
{
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		perror("Erro ao abrir o arquivo binário");
		exit(1);
	}

	if (fread(&img->largura, sizeof(int), 1, file) != 1 ||
        fread(&img->altura, sizeof(int), 1, file) != 1)
    {
        perror("Erro ao ler as dimensões da imagem");
        fclose(file);
        exit(1);
    }
	// Verificação: Certifique-se de que os valores de largura e altura são válidos
    if (img->largura <= 0 || img->altura <= 0)
    {
        printf("Erro: Dimensões inválidas da imagem (%d x %d).\n", img->largura, img->altura);
        fclose(file);
        exit(1);
    }
	img->tipo = 5;
	img->pData = (unsigned char *)malloc(img->largura * img->altura * sizeof(unsigned char));
    
	if (img->pData == NULL)
    {
        perror("Erro ao alocar memória para os dados da imagem");
        fclose(file);
        exit(1);
    }

	memset(img->pData, 255, img->largura * img->altura);
	decode(file, 0, 0, img->altura, img->largura, img);

	fclose(file);
	printf("Reconstrução completa.\n");
}