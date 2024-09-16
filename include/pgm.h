#ifndef PGM_H
#define PGM_H

struct pgm{
	int tipo;
	int altura;
	int largura;
	int mv;
	unsigned char *pData;
};

void readPGMImage(struct pgm *img, char *filename);
void writePGMImage(struct pgm *img, char *filename);
void viewPGMImage(struct pgm *img);

#endif

