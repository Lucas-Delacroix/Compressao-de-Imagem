#ifndef COMPRESS_H
#define COMPRESS_H

#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>

void compressQuadTree(QuadTreeNode *root, int largura, int altura, const char *filename);

void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);


void finalizarBitstream(FILE *file);


void escreverBits(FILE *file, int valor, int numBits);

#endif