SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Types.cpp

all: gc G_runtime_library.o

gc: $(SOURCES)
	g++ -o gc -Wall -g $(SOURCES)

G_runtime_library.o: G_runtime_library.cpp
	gcc -Wall -c  G_runtime_library.cpp


test: gc
	./gc tests.g
	
	@ echo Running tests in tests.g...
	@ sed -n -e 's/^.*expect: *//p' tests.g > expected
	./tests > actual || true
	@ diff expected actual || (echo 'Error: some tests in tests.g failed.'; false)

	@ echo Running negative tests...
	@ rm -rf tmp
	@ mkdir tmp
	@ awk -v RS= '{print > ("tmp/test" NR ".g")}' tests_neg.g
	@ for f in tmp/*.g ; do \
		[ "$$( (./gc $$f || false) 2>&1)" ] || \
			(echo "Error: compilation of $$f succeeded unexpectedly:"; cat $$f; false) ; \
	  done
	
	@ echo 'All tests passed.'
