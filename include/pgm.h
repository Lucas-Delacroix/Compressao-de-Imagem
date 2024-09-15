#ifndef PGM_H
#define PGM_H

struct pgm{
	int tipo;
	int altura;
	int largura;
	int mv;
	unsigned char *pData;
};

void readPGMImage(struct pgm *pio, char *filename);
void writePGMImage(struct pgm *pio, char *filename);
void viewPGMImage(struct pgm *pio);

#endif

