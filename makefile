SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp

parse: $(SOURCES)
	g++ -o parse -Wall -g $(SOURCES)
calc: calc.o
	gcc -o calc a.o -no-pie
calc.o: a.asm
	nasm -f elf64 -g -F dwarf a.asm -l a.lst
