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
void decode(FILE *, int, int, int, int, struct pgm *);
void compress(struct pgm *pio);
void reconstruirImagem(const char *, struct pgm *);
int lerBits(FILE *, int);

int main(int argc, char *argv[]){
	struct pgm img;
    if (argc != 3) {
        printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n", argv[0]);
        exit(1);
    }

    readPGMImage(&img, argv[1]);
    
    reconstruirImagem("saida.bin", &img);

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

int lerBits(FILE *file, int numBits) {
    static int buffer = 0;
    static int bitsDisponiveis = 0;
    int resultado = 0;
    
    while (numBits > 0) {
        if (bitsDisponiveis == 0) {
            buffer = fgetc(file);
            if (buffer == EOF) {
                return -1;  // Fim do arquivo
            }
            bitsDisponiveis = 8;
        }
        
        int bitsParaLer = (numBits < bitsDisponiveis) ? numBits : bitsDisponiveis;
        resultado = (resultado << bitsParaLer) | (buffer >> (bitsDisponiveis - bitsParaLer));
        bitsDisponiveis -= bitsParaLer;
        numBits -= bitsParaLer;
        buffer &= (1 << bitsDisponiveis) - 1;
    }
    
    return resultado;
}

// Função recursiva para reconstruir a imagem a partir do bitstream
void decode(FILE *file, int x, int y, int altura, int largura, struct pgm *pio) {
    int folha = lerBits(file, 1);  // Ler 1 bit para verificar se é nó folha
    if (folha == 1) {
        int media = lerBits(file, 8);  // Ler 8 bits para o valor médio
        for (int i = 0; i < altura; i++) {
            for (int j = 0; j < largura; j++) {
                pio->pData[(y + i) * pio->largura + (x + j)] = media;
            }
        }
    } else {
        int metadeAltura = altura / 2;
        int metadeLargura = largura / 2;
        
        // Chamar recursivamente para os quatro sub-blocos
        decode(file, x, y, metadeAltura, metadeLargura, pio);  // Top-Left
        decode(file, x + metadeLargura, y, metadeAltura, largura - metadeLargura, pio);  // Top-Right
        decode(file, x, y + metadeAltura, altura - metadeAltura, metadeLargura, pio);  // Bottom-Left
        decode(file, x + metadeLargura, y + metadeAltura, altura - metadeAltura, largura - metadeLargura, pio);  // Bottom-Right
    }
}

// Função para reconstruir a imagem a partir do arquivo binário
void reconstruirImagem(const char *filename, struct pgm *img) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo binário");
        exit(1);
    }
    
    decode(file, 0, 0, img->altura, img->largura, img);
    
    fclose(file);
    printf("Reconstrução completa.\n");
}