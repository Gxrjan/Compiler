SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp

gc: $(SOURCES)
	g++ -o gc -Wall -g $(SOURCES)
