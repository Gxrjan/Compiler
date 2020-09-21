SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Types.cpp

all: gc G_runtime_library.o

gc: $(SOURCES)
	g++ -o gc -Wall -g $(SOURCES)

G_runtime_library.o: G_runtime_library.cpp
	gcc -Wall -c  G_runtime_library.cpp


test: gc
	./gc tests.g
	@ sed -n -e 's/^.*expect: *//p' tests.g > expected
	./tests > actual || true
	@ diff expected actual && echo 'All tests passed.' || echo 'Some tests failed.'
