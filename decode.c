#include <stdio.h>
#include <stdlib.h>


struct pgm{
	int tipo;
	int altura;
	int largura;
	int mv;
	unsigned char *pData;
};


void readPGMImage(struct pgm *, char *);
void viewPGMImage(struct pgm *);
void writePGMImage(struct pgm *, char *);
void decode(int, int, int, int, int, struct pgm *, unsigned char **);
void compress(struct pgm *pio);

int main(int argc, char *argv[]){
	struct pgm img;


	if (argc!=3){
		printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n",argv[0]);
		exit(1);
	}

    unsigned char bitstream[7][2] = {
    {1, 0},    // Bloco principal não homogêneo, divide em 4
    {0, 100},  // Sub-bloco 1: homogêneo, valor 100
    {0, 150},  // Sub-bloco 2: homogêneo, valor 150
    {0, 200},  // Sub-bloco 3: homogêneo, valor 200
    {0, 250},  // Sub-bloco 4: homogêneo, valor 250
};
    unsigned char *ptr = &bitstream[0][0];
    unsigned char **div = &ptr;

	readPGMImage(&img,argv[1]);

    decode(0, 0, 0, img.altura, img.largura, &img, div);
	writePGMImage(&img, argv[2]);


	viewPGMImage(&img);


	return 0;


}


void readPGMImage(struct pgm *pio, char *filename){
	FILE *fp;
	char ch;


	if (!(fp = fopen(filename,"r"))){
		perror("Erro.");
		exit(1);
	}


	if ( (ch = getc(fp))!='P'){
		puts("A imagem fornecida não está no formato pgm");
		exit(2);
	}
	
	pio->tipo = getc(fp)-48;
	
	fseek(fp,1, SEEK_CUR);


	while((ch=getc(fp))=='#'){
		while( (ch=getc(fp))!='\n');
	}


	fseek(fp,-1, SEEK_CUR);


	fscanf(fp, "%d %d",&pio->altura,&pio->largura);
	if (ferror(fp)){ 
		perror(NULL);
		exit(3);
	}	
	fscanf(fp, "%d",&pio->mv);
	fseek(fp,1, SEEK_CUR);


	pio->pData = (unsigned char*) malloc(pio->largura * pio->altura * sizeof(unsigned char));


	switch(pio->tipo){
		case 2:
			puts("Lendo imagem PGM (dados em texto)");
			for (int k=0; k < (pio->largura * pio->altura); k++){
				fscanf(fp, "%hhu", pio->pData+k);
			}
		break;	
		case 5:
			puts("Lendo imagem PGM (dados em binário)");
			fread(pio->pData,sizeof(unsigned char),pio->largura * pio->altura, fp);
		break;
		default:
			puts("Não está implementado");
	}
	
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
	printf("Max: %d\n",pio->mv);


	for (int k=0; k < (pio->largura * pio->altura); k++){
		if (!( k % pio->altura)) printf("\n");
		printf("%2hhu ",*(pio->pData+k));
	}	
	printf("\n");
}

void decode(int x, int y, int div, int altura, int largura, struct pgm *pio, unsigned char **bit){
    int media = *(*(bit+div)+1);
    int indice = div;
    if (**(bit+div) == 0) {
        for (int i = 0; i < altura; i++){
            for(int j = 0; j<largura; j++){
                *((pio->pData)+((x+i)*pio->largura+(y+j))) = media; 
            }
        }
    } else {
        int novaAltura = (altura)/2;
        int novaLargura = (largura)/2;
        div++;
        decode(x, y, novaAltura, novaLargura, div, pio, bit);
        decode(x+novaAltura, y, novaAltura, novaLargura, div, pio, bit);
        decode(x, y+novaLargura, novaAltura, novaLargura, div, pio, bit);
        decode(x+novaAltura, y+novaLargura, novaAltura, novaLargura, div, pio, bit);
    }
}