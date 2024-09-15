#include <stdio.h>
#include <stdlib.h>
#include "compress.h"
#include "pgm.h"
#include "quadtree.h"


int main(int argc, char *argv[])
{
	struct pgm img;
	if (argc != 3)
	{
		printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n", argv[0]);
		exit(1);
	}

	readPGMImage(&img, argv[1]);

	QuadTreeNode *root = construirQuadTree(&img, 0, 0, img.largura, img.altura);

	compressQuadTree(root, "saida.bin");

	imprimirQuadTree(root);

	writePGMImage(&img, argv[2]);

	viewPGMImage(&img);

	return 0;
}


