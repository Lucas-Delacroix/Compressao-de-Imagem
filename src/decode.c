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
#include "pgm.h"
#include "decodificacao.h"



int main(int argc, char *argv[])
{
	struct pgm img;
	if (argc != 3)
	{
		printf("Formato: \n\t %s <binarioEntrada.bin> <imagemSaida.pgm>\n", argv[0]);
		exit(1);
	}

	reconstruirImagem(argv[1], &img);

	writePGMImage(&img, argv[2]);
	viewPGMImage(&img);


	return 0;
}


