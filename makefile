SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Types.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Translator_LLVM.cpp
all: gc G_runtime_library.o Types.o

BENCHMARKS = insertion_sort prime_sum tag perm merge_sort hash_table prime_count
BENCHMARK_PROGS = $(BENCHMARKS) $(addsuffix _cs, $(BENCHMARKS)) $(addsuffix _cpp, $(BENCHMARKS))
BENCHMARK_HOME = benchmark/
DOTNET_HOME = ${BENCHMARK_HOME}dotnet/
DOTNET_DIRS = $(addsuffix /, $(addprefix ${DOTNET_HOME}, ${BENCHMARKS}))
DOTNET_BINS = $(join ${DOTNET_DIRS}/, $(addprefix bin/Release/net5.0/, ${BENCHMARKS}))
AOT_HOME = ${BENCHMARK_HOME}aot/

dotnet_build: $(addprefix dotnet_, ${BENCHMARKS})

dotnet_%: benchmark/%.cs | ${DOTNET_DIRS}           
	dotnet new console -o benchmark/dotnet/${*}
	rm benchmark/dotnet/${*}/Program.cs
	cp ${<} benchmark/dotnet/${*}
	cd benchmark/dotnet/${*}; dotnet build --configuration Release

${DOTNET_DIRS}: | ${DOTNET_HOME} # Creates separate directories for each benchmark
	mkdir $@

${DOTNET_HOME}: # Creates home for dotnet
	mkdir $@


dotnet_clean:
	rm -fr benchmark/dotnet



.PHONY: options_test test benchmark
gc: $(SOURCES) 
	g++ -o gc -Wall -std=c++17 -g $(SOURCES)

G_runtime_library.o: G_runtime_library.cpp
	clang++ -Wall -c -g -O2 G_runtime_library.cpp 

Types.o: Types.cpp
	clang++ -Wall -c -g -O2 Types.cpp


test: gc tests.g tests_neg.g test_mem.g tests_bounds.g
	./gc tests.g
	python tester.py

mem_leak: gc
	./gc test_mem.g
	valgrind ./test_mem


benchmark: $(addprefix benchmark/, $(BENCHMARK_PROGS)) $(addprefix benchmark/aot/, $(addsuffix _cs.so, $(BENCHMARKS)))
	python benchmark.py


${BENCHMARK_HOME}%: ${BENCHMARK_HOME}%.g gc
	./gc ${<}

${BENCHMARK_HOME}%_cs: ${BENCHMARK_HOME}%.cs
	mcs -out:$@ ${<}

${AOT_HOME}%_cs.so: ${BENCHMARK_HOME}%_cs | ${AOT_HOME}
	mono --aot=outfile=${@} -O=all ${<}

${BENCHMARK_HOME}%_cpp: ${BENCHMARK_HOME}%.cpp
	clang++ -O2 -o $@ ${<}

${AOT_HOME}:
	mkdir $@

options_test: gc
	python options_test.py

clean:
	rm -f $(addprefix benchmark/, $(BENCHMARK_PROGS))
	rm -fr ${AOT_HOME}
	rm -rf ${DOTNET_HOME}

clean_benchmarks:
	rm $(addprefix benchmark/, $(BENCHMARK_PROGS))

clean_aot:
	rm benchmark/aot/*
