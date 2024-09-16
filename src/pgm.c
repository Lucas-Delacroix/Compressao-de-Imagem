/************************************************************************** */
/* Aluno: Lucas Delacroix
* Guilherme Pereira
* Antonio Lucas
* Louse
* Avaliacao 04: Trabalho Final
* 2024.1 - Prof. Daniel Ferreira
* Compilador:gcc (Ubuntu 11.4.0-1ubuntu1~22.04)
***************************************************************************************/




#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>


//Função para ler uma imagem PGM e armazenar os dados na struct pgm
void readPGMImage(struct pgm *pio, char *filename){

	//Criação do ponteiro para um arquivo fp(manipular o arquivo da imagem PGM)
	FILE *fp;
	char ch;


	//Tenta abrir o código com o nome filename para leitura(r), se não conseguir imprime erro e termina
	if (!(fp = fopen(filename,"r"))){
		perror("Erro.");
		exit(1);
	}

	//Verificar formato do arquivo (se é pgm ou não), se não conseguir printa a mensagem e termina 
	if ( (ch = getc(fp))!='P'){
		puts("A imagem fornecida não está no formato pgm");
		exit(2);
	}
	
	pio->tipo = getc(fp)-48;
	
	fseek(fp,1, SEEK_CUR);

	//Ignora os comentários # no arquivo PGM, continua lendo ate que não encontre linhas com #
	while((ch=getc(fp))=='#'){
		while( (ch=getc(fp))!='\n');
	}

	//garantir que a leitura continue no lugar certo, volta depois de ler o último caractere não comentado
	fseek(fp,-1, SEEK_CUR);

	//ler a altura e a largura e armazenar os valores nos campos altura e largura da struct pgm
	fscanf(fp, "%d %d",&pio->altura,&pio->largura);

	//Verificar se houve algum erro na leitura do arquivo, se ocorrer ele termina
	if (ferror(fp)){ 
		perror(NULL);
		exit(3);
	}	

	//ler o valor máximo (mv) que pode ser ate 255 
	fscanf(fp, "%d",&pio->mv);

	//ignorar caractere
	fseek(fp,1, SEEK_CUR);

	//Alocação de memoria de cada pixel com exatidão
	pio->pData = (unsigned char*) malloc(pio->largura * pio->altura * sizeof(unsigned char));


	switch(pio->tipo){
		//Se o tipo for em imagem de texto ele indentifica e lê os valores em formato de texto com fscan e são armazenados em pData

		case 2:
			puts("Lendo imagem PGM (dados em texto)");
			for (int k=0; k < (pio->largura * pio->altura); k++){
				fscanf(fp, "%hhu", pio->pData+k);
			}
		break;	
		//Se o tipo for em binário ele lê os pixel diretamente do arquivo em formato binario com o fread
		case 5:
			puts("Lendo imagem PGM (dados em binário)");
			fread(pio->pData,sizeof(unsigned char),pio->largura * pio->altura, fp);
		break;
		default:
			puts("Não está implementado");
	}
	
	//fechar arquivo
	fclose(fp);


}

void writePGMImage(struct pgm *pio, char *filename){
	FILE *fp;
	char ch;


	if (!(fp = fopen(filename,"wb"))){
		perror("Erro.");
		exit(1);
	}


	fprintf(fp, "%s\n","P5");
	fprintf(fp, "%d %d\n",pio->altura, pio->largura);
	fprintf(fp, "%d\n", 255);


	fwrite(pio->pData, sizeof(unsigned char),pio->altura * pio->largura, fp);


	fclose(fp);


}


void viewPGMImage(struct pgm *pio){
	printf("Tipo: %d\n",pio->tipo);
	printf("Dimensões: [%d %d]\n",pio->altura, pio->largura);
	printf("Size: %ld\n", sizeof((pio->pData)));

	for (int k=0; k < (pio->largura * pio->altura); k++){
		// if (!( k % pio->altura)) printf("\n");
		// printf("%2hhu ",*(pio->pData+k));

	}	
	printf("\n");
}

