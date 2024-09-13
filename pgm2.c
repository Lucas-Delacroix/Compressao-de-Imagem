#include <stdio.h>
#include <stdlib.h>



struct pgm{
	int tipo;
	int altura;
	int largura;
	int mv;
	unsigned char *pData;
};

// Estrutura de nó da QuadTree
typedef struct QuadTreeNode {
    int x, y;  // Coordenadas do canto superior esquerdo do bloco
    int largura, altura;  // Dimensões do bloco
    int valor;  // Valor médio do bloco (por simplicidade)
    int folha;  // Indica se é um nó folha
    struct QuadTreeNode *topLeft, *topRight, *bottomLeft, *bottomRight;  // Filhos
} QuadTreeNode;

// Função para criar um novo nó da QuadTree
QuadTreeNode* Pintar(unsigned char *data, int incremento, int largura, int altura){
    for(int i = 0; i < (largura * altura); i++){
        data[i] = incremento;
    }
}

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

// Função para calcular a média dos valores no bloco
int calcularMedia(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int soma = 0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            soma += data[(y + i) * larguraImg + (x + j)];
        }
    }
    return soma / (largura * altura);
}

// Função para verificar se o bloco é homogêneo (todos os valores iguais)
// int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
//     int primeiroValor = data[y * larguraImg + x];
//     for (int i = 0; i < altura; i++) {
//         for (int j = 0; j < largura; j++) {
//             if (data[(y + i) * larguraImg + (x + j)] != primeiroValor) {
//                 return 0;  // Bloco não homogêneo
//             }
//         }
//     }
//     return 1;  // Bloco homogêneo
// }

int blocoHomogeneo(unsigned char *data, int larguraImg, int x, int y, int largura, int altura) {
    int tolerancia = 80;
    int primeiroValor = data[y * larguraImg + x];
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valorAtual = data[(y + i) * larguraImg + (x + j)];
            // Verifica se a diferença entre os valores está dentro da tolerância
            if (abs(valorAtual - primeiroValor) > tolerancia) {
                return 0;  // Bloco não homogêneo
            }
        }
    }
    return 1;  // Bloco homogêneo
}

// Função recursiva para construir a QuadTree
QuadTreeNode* construirQuadTree(struct pgm *pio, int x, int y, int largura, int altura, int incremento) {
    unsigned char *data = pio->pData;
    int larguraImg = pio->largura;
    
    // Se o bloco for homogêneo ou de tamanho mínimo, cria um nó folha
    if (blocoHomogeneo(data, larguraImg, x, y, largura, altura) || (largura == 1 && altura == 1)) {
        int media = calcularMedia(data, larguraImg, x, y, largura, altura);
        incremento+=3;
        
        Pintar(data, media, largura, altura);
        return criarNo(x, y, largura, altura, media, 1);  // Nó folha
    }
    // Dividir o bloco em quatro sub-blocos
    int metadeLargura = largura / 2;
    int metadeAltura = altura / 2;

    QuadTreeNode* node = criarNo(x, y, largura, altura, 0, 0);  // Nó interno

    // Chamada recursiva para os quatro sub-blocos
    node->topLeft = construirQuadTree(pio, x, y, metadeLargura, metadeAltura, incremento);
    node->topRight = construirQuadTree(pio, x + metadeLargura, y, largura - metadeLargura, metadeAltura, incremento);
    node->bottomLeft = construirQuadTree(pio, x, y + metadeAltura, metadeLargura, altura - metadeAltura, incremento);
    node->bottomRight = construirQuadTree(pio, x + metadeLargura, y + metadeAltura, largura - metadeLargura, altura - metadeAltura, incremento);

    return node;
}

void escreverBits(FILE *file, int valor, int numBits);
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);
void finalizarBitstream(FILE *file);
void compressQuadTree(QuadTreeNode *root, const char *filename);


// Função para imprimir a QuadTree (para visualização)
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


// Função principal para escrever a QuadTree em um arquivo binário
void compressQuadTree(QuadTreeNode *root, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir arquivo de bitstream");
        exit(1);
    }
    
    // Converte a QuadTree para bitstream
    QuadTreeParaBitstream(root, file);
    
    // Finaliza o bitstream (escreve o byte incompleto, se necessário)
    finalizarBitstream(file);
    
    fclose(file);
    printf("Compressão completa. Arquivo salvo como %s\n", filename);
}


// Função para fechar qualquer byte incompleto no arquivo
void finalizarBitstream(FILE *file) {
    static int buffer = 0;
    static int bitsDisponiveis = 8;
    
    if (bitsDisponiveis < 8) {
        buffer <<= bitsDisponiveis;
        fputc(buffer, file);
    }
}

// Função recursiva para converter a QuadTree em bitstream
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file) {
    if (node->folha) {
        escreverBits(file, 1, 1);  // Indica que é um nó folha
        escreverBits(file, node->valor, 8);  // Escreve o valor do nó folha (8 bits)
    } else {
        escreverBits(file, 0, 1);  // Indica que é um nó interno
        // Chama recursivamente para os quatro filhos
        QuadTreeParaBitstream(node->topLeft, file);
        QuadTreeParaBitstream(node->topRight, file);
        QuadTreeParaBitstream(node->bottomLeft, file);
        QuadTreeParaBitstream(node->bottomRight, file);
    }
}

// Função para escrever bits no arquivo de bitstream
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

void readPGMImage(struct pgm *, char *);
void viewPGMImage(struct pgm *);
void writePGMImage(struct pgm *, char *);
void compressImage(struct pgm *);


int main(int argc, char *argv[]){
	struct pgm img;
	if (argc!=3){
		printf("Formato: \n\t %s <imagemEntrada.pgm> <imagemSaida.pgm>\n",argv[0]);
		exit(1);
	}
    int incremento = 0;


	readPGMImage(&img,argv[1]);
	// compressImage(&img);
	QuadTreeNode *root = construirQuadTree(&img, 0, 0, img.largura, img.altura, incremento);
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
	printf("Size: %d\n", sizeof((pio->pData)));

	for (int k=0; k < (pio->largura * pio->altura); k++){
		// if (!( k % pio->altura)) printf("\n");
		// printf("%2hhu ",*(pio->pData+k));

	}	
	printf("\n");
}




void compressImage(struct pgm *pio){
	int x =0;
	// unsigned char data = *(pio->pData);
	
	// Loop for para percorrer a matriz inteira
	for (int k = 0; k < (pio->largura * pio->altura); k++){
        for (x = 0; x < (pio->largura) / 2; x++){
            *(pio->pData + k) = 255;
            k++;
        }
        k += (pio->largura) / 2 - 1; // Subtraímos 1 porque o for principal também incrementa k
    }
}