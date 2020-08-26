scan: head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp
	g++ -o parse -Wall -g head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp
calc: calc.o
	gcc -o calc a.o -no-pie
calc.o: a.asm
	nasm -f elf64 -g -F dwarf a.asm -l a.lst
