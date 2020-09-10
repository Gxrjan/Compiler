SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp

gc: $(SOURCES)
	g++ -o gc -Wall -g $(SOURCES)
prog: a.o
	gcc -o prog a.o -no-pie
a.o: a.asm
	nasm -Werror -f elf64 -g -F dwarf a.asm -l a.lst
