#ifndef COMPRESS_H
#define COMPRESS_H

#include "quadtree.h"
#include "pgm.h"
#include <stdio.h>

void compressQuadTreeToBin(QuadTreeNode *root, int largura, int altura, const char *filename);
void compressQuadTreeToTxt(QuadTreeNode *root, int largura, int altura, const char *filenameTxt);
void QuadTreeParaBitstream(QuadTreeNode *node, FILE *file);
void QuadTreeParaTxtBitstream(QuadTreeNode *node, FILE *fileTxt);
void escreverBits(FILE *file, int valor, int numBits);
void finalizarBitstream(FILE *file);



#endif