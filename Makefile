# Regra padrão
all: prog.e

# Regra para criar o executável
prog.e: code.o compress.o quadtree.o pgm.o
	gcc -o prog.e code.o compress.o quadtree.o pgm.o

# Regras para compilar arquivos .c em arquivos .o
code.o: src/code.c
	gcc -Iinclude -c src/code.c -o code.o

compress.o: src/compress.c
	gcc -Iinclude -c src/compress.c -o compress.o

quadtree.o: src/quadtree.c
	gcc -Iinclude -c src/quadtree.c -o quadtree.o

pgm.o: src/pgm.c
	gcc -Iinclude -c src/pgm.c -o pgm.o

# Regra para limpar arquivos gerados
clean:
	rm -f code.o compress.o quadtree.o pgm.o prog.e
