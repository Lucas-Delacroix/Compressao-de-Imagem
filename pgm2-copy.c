#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define tolerancia 1
#define maxDiff 15  

struct pgm{
	int tipo;
	int altura;
	int largura;
	int mv;
	unsigned char *pData;
};


typedef struct QuadTreeNode {
    int x, y;  
    int largura, altura;  
    int valor;  
    int folha;  
    struct QuadTreeNode *topLeft, *topRight, *bottomLeft, *bottomRight;  
} QuadTreeNode;


QuadTreeNode* criarNo(int x, int y, int largura, int altura, int valor, int folha) {
    QuadTreeNode* node = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));
    node->x = x;
    node->y = y;
    node->largura = largura;
    node->altura = altura;
    node->valor = valor;
    node->folha = folha;
    node->topLeft = node->topRight = node->bottomLeft = node->bottomRight = NULL;
    return node;
}


float calcularDesvioPadrao(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
float calcularErroPadrao(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
double calcularEntropia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);

int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura);
void escreverBits(FILE *file, int valor, int numBits);
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);
void finalizarBitstream(FILE *file);
void compressQuadTree(QuadTreeNode *root, const char *filename);
void imprimirQuadTree(QuadTreeNode *node);
void compressQuadTree(QuadTreeNode *root, const char *filename);
void finalizarBitstream(FILE *file);
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);
void escreverBits(FILE *file, int valor, int numBits);
void readPGMImage(struct pgm *, char *);
void viewPGMImage(struct pgm *);
void writePGMImage(struct pgm *, char *);



QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura) {
    unsigned char *data = pio->pData;
    int larguraImg = pio->largura;
    
    
    if (blocoHomogeneo(data, larguraImg, x, y, largura, altura) || (largura == 1 && altura == 1)) {
        int media = calcularMedia(data, larguraImg, x, y, largura, altura);
        return criarNo(x, y, largura, altura, media, 1);  
    }
    
    
    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    QuadTreeNode* node = criarNo(x, y, largura, altura, 0, 0);  

    
    node->topLeft = construirQuadTree(pio, x, y, metadeLargura, metadeAltura);
    node->topRight = construirQuadTree(pio, x + metadeLargura, y, largura - metadeLargura, metadeAltura);
    node->bottomLeft = construirQuadTree(pio, x, y + metadeAltura, metadeLargura, altura - metadeAltura);
    node->bottomRight = construirQuadTree(pio, x + metadeLargura, y + metadeAltura, largura - metadeLargura, altura - metadeAltura);

    return node;
}


int main(int argc, char *argv[]){
	struct pgm img;
	if (argc!=3){
		printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n",argv[0]);
		exit(1);
	}


	readPGMImage(&img,argv[1]);
	QuadTreeNode *root = construirQuadTree(&img, 0, 0, img.largura, img.altura);
	compressQuadTree(root, "saida.bin");
    imprimirQuadTree(root);
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


	// fprintf(fp, "%s\n","P5");
	// fprintf(fp, "%d %d\n",pio->altura, pio->largura);
	// fprintf(fp, "%d\n", 255);


	fwrite(pio->pData, sizeof(unsigned char),pio->altura * pio->largura, fp);


	fclose(fp);


}


void viewPGMImage(struct pgm *pio){
	printf("Tipo: %d\n",pio->tipo);
	printf("Dimensões: [%d %d]\n",pio->altura, pio->largura);
	printf("Max: %d\n",pio->mv);
	printf("Size: %ld\n", sizeof((pio->pData)));

	for (int k=0; k < (pio->largura * pio->altura); k++){
		// if (!( k % pio->altura)) printf("\n");
		// printf("%2hhu ",*(pio->pData+k));

	}	
	printf("\n");
}


int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int soma = 0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            soma += data[(y + i) * larguraImg + (x + j)];
        }
    }
    return soma / (largura * altura);
}


int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int primeiroValor = data[y * larguraImg + x];
    int maximoValor = primeiroValor;
    int minimoValor = primeiroValor;
    
    // for (int i = 0; i < altura; i++) {
    //     for (int j = 0; j < largura; j++) {
    //         int valorAtual = data[(y + i) * larguraImg + (x + j)];
    //         if (valorAtual > maximoValor) maximoValor = valorAtual;
    //         if (valorAtual < minimoValor) minimoValor = valorAtual;
    //         // Verifica se a diferença entre os valores está dentro da tolerância
    //         if (abs(valorAtual - primeiroValor) > tolerancia || (maximoValor - minimoValor) > maxDiff) {
    //             return 0;  // Bloco não homogêneo
    //         }
    //     }
    // }
    
    // float erroPadrao = calcularErroPadrao(data, larguraImg, x, y, largura, altura);
    // if (erroPadrao > tolerancia) {
    //     return 0;  // Bloco não homogêneo
    // }



    // Alternativamente, verificar o desvio padrão para decidir se o bloco é homogêneo
    float desvioPadrao = calcularDesvioPadrao(data, larguraImg, x, y, largura, altura);
    double entropia = calcularEntropia(data, larguraImg, x, y, largura, altura);
    // Define os limiares para desvio padrão e entropia
    double limiteDesvioPadrao = 15.0;  // Ajuste conforme necessário
    double limiteEntropia = 3.0;  // Ajuste conforme necessário


    // Verifica se o bloco é homogêneo com base no desvio padrão e na entropia
    if (desvioPadrao < limiteDesvioPadrao && entropia < limiteEntropia) {
        return 1;  
    } else {
        return 0;  
    }
    // if (desvioPadrao > tolerancia) {
    //     return 0;  // Bloco não homogêneo
    // }

    // return 1;  // Bloco homogêneo
}



void imprimirQuadTree(QuadTreeNode *node) {
    if (node == NULL) return;

    if (node->folha) {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d - Valor: %d (Folha)\n", node->x, node->y, node->largura, node->altura, node->valor);
    } else {
        printf("Bloco [%d, %d] - Largura: %d, Altura: %d (Nó Interno)\n", node->x, node->y, node->largura, node->altura);
        imprimirQuadTree(node->topLeft);
        imprimirQuadTree(node->topRight);
        imprimirQuadTree(node->bottomLeft);
        imprimirQuadTree(node->bottomRight);
    }
}


void compressQuadTree(QuadTreeNode *root, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir arquivo de bitstream");
        exit(1);
    }
    
    
    QuadTreeParaBitstream(root, file);
    
    
    finalizarBitstream(file);
    
    fclose(file);
    printf("Compressão completa. Arquivo salvo como %s\n", filename);
}



void finalizarBitstream(FILE *file) {
    static int buffer = 0;
    static int bitsDisponiveis = 8;
    
    if (bitsDisponiveis < 8) {
        buffer <<= bitsDisponiveis;
        fputc(buffer, file);
    }
}


void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file) {
    if (node->folha) {
        escreverBits(file, 1, 1);  
        escreverBits(file, node->valor, 8);  
    } else {
        escreverBits(file, 0, 1);  
        
        QuadTreeParaBitstream(node->topLeft, file);
        QuadTreeParaBitstream(node->topRight, file);
        QuadTreeParaBitstream(node->bottomLeft, file);
        QuadTreeParaBitstream(node->bottomRight, file);
    }
}



void escreverBits(FILE *file, int valor, int numBits) {
    static int buffer = 0;
    static int bitsDisponiveis = 8;
    
    while (numBits > 0) {
        int bitsParaEscrever = (numBits < bitsDisponiveis) ? numBits : bitsDisponiveis;
        buffer = (buffer << bitsParaEscrever) | (valor >> (numBits - bitsParaEscrever));
        bitsDisponiveis -= bitsParaEscrever;
        numBits -= bitsParaEscrever;
        valor &= (1 << numBits) - 1;
        
        if (bitsDisponiveis == 0) {
            fputc(buffer, file);
            buffer = 0;
            bitsDisponiveis = 8;
        }
    }
}



float calcularDesvioPadrao(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    float soma = 0, somaQuadrada = 0;
    int numPixels = largura * altura;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valor = data[(y + i) * larguraImg + (x + j)];
            soma += valor;
            somaQuadrada += valor * valor;
        }
    }

    float media = soma / numPixels;
    return sqrt((somaQuadrada / numPixels) - (media * media));
}

float calcularErroPadrao(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int soma = 0;
    int totalPixels = largura * altura;
    float media, variancia = 0.0;

    
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            soma += data[(y + i) * larguraImg + (x + j)];
        }
    }
    media = soma / (float)totalPixels;

    
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            float valor = data[(y + i) * larguraImg + (x + j)];
            variancia += (valor - media) * (valor - media);
        }
    }
    variancia /= totalPixels;

    
    float desvioPadrao = sqrt(variancia);

    
    float erroPadrao = desvioPadrao / sqrt(totalPixels);

    return erroPadrao;
}


double calcularEntropia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int histograma[256] = {0};
    int totalPixels = largura * altura;

    
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valor = data[(y + i) * larguraImg + (x + j)];
            histograma[valor]++;
        }
    }

    
    double entropia = 0.0;
    for (int i = 0; i < 256; i++) {
        if (histograma[i] > 0) {
            double probabilidade = (double)histograma[i] / totalPixels;
            entropia -= probabilidade * log2(probabilidade);
        }
    }

    return entropia;
}