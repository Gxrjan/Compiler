SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Translator.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Types.cpp Translator_LLVM.cpp
all: gc G_runtime_library.o

gc: $(SOURCES)
	g++ -o gc -Wall -std=c++17 -g $(SOURCES)

G_runtime_library.o: G_runtime_library.cpp
	clang++ -Wall -c  G_runtime_library.cpp


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

benchmark: benchmark/insertion_sort benchmark/prime_sum benchmark/tag benchmark/insertion_sort.exe benchmark/prime_sum.exe benchmark/tag.exe benchmark/insertion_sort_cpp benchmark/prime_sum_cpp benchmark/tag_cpp
	python tester.py


benchmark/insertion_sort: gc benchmark/insertion_sort.g
	./gc benchmark/insertion_sort.g

benchmark/insertion_sort.exe: benchmark/insertion_sort.cs
	mcs benchmark/insertion_sort.cs

benchmark/prime_sum: gc benchmark/prime_sum.g
	./gc benchmark/prime_sum.g

benchmark/prime_sum.exe: benchmark/prime_sum.cs
	mcs benchmark/prime_sum.cs

benchmark/tag: gc benchmark/tag.g
	./gc benchmark/tag.g

benchmark/tag.exe: benchmark/tag.cs
	mcs benchmark/tag.cs


benchmark/insertion_sort_cpp: benchmark/insertion_sort.cpp
	g++ -o benchmark/insertion_sort_cpp benchmark/insertion_sort.cpp

benchmark/prime_sum_cpp: benchmark/prime_sum.cpp
	g++ -o benchmark/prime_sum_cpp benchmark/prime_sum.cpp

benchmark/tag_cpp: benchmark/tag.cs
	g++ -o benchmark/tag_cpp benchmark/tag.cpp