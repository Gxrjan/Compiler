SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp

gc: $(SOURCES)
	g++ -o gc -Wall -g $(SOURCES)

test: gc
	./gc tests.g
	@ sed -n -e 's/^.*expect: *//p' tests.g > expected
	./tests > actual || true
	@ diff expected actual && echo 'All tests passed.' || echo 'Some tests failed.'
