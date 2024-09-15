#ifndef DECODEFICACAO_H
#define DECODEFICACAO_H
#include<stdio.h>

#include "pgm.h"  

// Função para ler um número específico de bits de um arquivo
int lerBits(FILE *file, int numBits);

// Função recursiva para decodificar o bitstream e reconstruir a imagem
void decode(FILE *file, int x, int y, int altura, int largura, struct pgm *pio);

// Função para iniciar o processo de reconstrução da imagem a partir de um arquivo binário
void reconstruirImagem(const char *filename, struct pgm *img);

#endif 
