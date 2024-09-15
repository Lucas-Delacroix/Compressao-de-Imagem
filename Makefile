# prog.e = exe codificação
# decode.e = exe decodificação
all: prog.e decode.e

prog.e: code.o compress.o quadtree.o pgm.o
	gcc -o prog.e code.o compress.o quadtree.o pgm.o

decode.e: decode.o decodificacao.o pgm.o
	gcc -o decode.e decode.o decodificacao.o pgm.o

code.o: src/code.c
	gcc -Iinclude -c src/code.c -o code.o

decode.o: src/decode.c
	gcc -Iinclude -c src/decode.c -o decode.o

compress.o: src/compress.c
	gcc -Iinclude -c src/compress.c -o compress.o

quadtree.o: src/quadtree.c
	gcc -Iinclude -c src/quadtree.c -o quadtree.o

pgm.o: src/pgm.c
	gcc -Iinclude -c src/pgm.c -o pgm.o

decodificacao.o: src/decodificacao.c
	gcc -Iinclude -c src/decodificacao.c -o decodificacao.o

# Limpar Arquivos
clean:
	rm -f code.o compress.o quadtree.o pgm.o prog.e decode.o decodificacao.o decode.e
