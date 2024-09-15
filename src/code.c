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
	if (argc != 3)
	{
		printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n", argv[0]);
		exit(1);
	}

	//leitura da imagem PGM do arquivo de entrada
	readPGMImage(&img, argv[1]);

	// compressImage(&img);

	//Declaração de um ponteiro root do tipo quadtree (nó) e criação da imagem representada por img
	QuadTreeNode *root = construirQuadTree(&img, 0, 0, img.largura, img.altura);

	//Comprimir quadtree e salva os dados como saida.bin 
	compressQuadTree(root, img.largura, img.altura, "saida.bin");

	//Visualizar estrutura da quadtree
	imprimirQuadTree(root);

	//Gravar a imagem img no arquivo de saída no segundo argumento de entrada
	writePGMImage(&img, argv[2]);

	//Exibir PGM na tela.
	viewPGMImage(&img);

	return 0;
}


