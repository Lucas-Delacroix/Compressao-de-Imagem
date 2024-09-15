#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"
#include "decodificacao.h"



int main(int argc, char *argv[])
{
	struct pgm img;
	if (argc != 2)
	{
		printf("Formato: \n\t %s <imagemSaida.pgm>\n", argv[0]);
		exit(1);
	}

	reconstruirImagem("saida.bin", &img);

	writePGMImage(&img, argv[1]);
	viewPGMImage(&img);


	return 0;
}


