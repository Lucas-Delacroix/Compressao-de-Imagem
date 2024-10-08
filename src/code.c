/************************************************************************** */
/* Aluno: Lucas Delacroix
* Guilherme Pereira
* Antonio Lucas
* Louise Sampaio Araújo Gonçalves
* Avaliacao 04: Trabalho Final
* 2024.1 - Prof. Daniel Ferreira
* Compilador:gcc (Ubuntu 11.4.0-1ubuntu1~22.04)
***************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include "compress.h"
#include "pgm.h"
#include "quadtree.h"


int main(int argc, char *argv[])
{
	//Declarar variavel img do tipo struct pertecente a pgm.h
	struct pgm img;

	//Verificar argumentos de execução
	if (argc != 4)
	{
		printf("Formato: \n\t %s <imagemEntrada.pgm> <binarioSaida.bin> <binarioSaida.txt>\n", argv[0]);
		exit(1);
	}

	//leitura da imagem PGM do arquivo de entrada
	readPGMImage(&img, argv[1]);

	// compressImage(&img);

	//Declaração de um ponteiro root do tipo quadtree (nó) e criação da imagem representada por img
	QuadTreeNode *root = construirQuadTree(&img, 0, 0, img.largura, img.altura);

	//Comprimir quadtree e salva os dados como saida.bin e saida.txt
	compressQuadTreeToBin(root, img.largura, img.altura, argv[2]);
	compressQuadTreeToTxt(root, img.largura, img.altura, argv[3]);

	//Visualizar estrutura da quadtree
	imprimirQuadTree(root);

	//Gravar a imagem img no arquivo de saída no segundo argumento de entrada
	writePGMImage(&img, "code.pgm");

	//Exibir PGM na tela.
	viewPGMImage(&img);

	return 0;
}


