#ifndef COMPRESS_H
#define COMPRESS_H

#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>

void compressQuadTree(QuadTreeNode *root, const char *filename);
// Função recursiva para converter a QuadTree em bitstream
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);

// Função para fechar qualquer byte incompleto no arquivo
void finalizarBitstream(FILE *file);

void compressImage(struct pgm *);

void escreverBits(FILE *file, int valor, int numBits);

#endif