scan: head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp
	g++ -o parse -Wall -g head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp
