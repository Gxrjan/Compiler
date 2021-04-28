SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Types.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Translator_LLVM.cpp
all: gc G_runtime_library.o Types.o

BENCHMARKS = insertion_sort prime_sum tag perm merge_sort hash_table prime_count
BENCHMARK_PROGS = $(BENCHMARKS) $(addsuffix _cs, $(BENCHMARKS)) $(addsuffix _cpp, $(BENCHMARKS)) \
					$(addsuffix _dotnet, $(BENCHMARKS))
BENCHMARK_HOME = benchmark/
DOTNET_HOME = ${BENCHMARK_HOME}dotnet/
DOTNET_DIRS = $(addsuffix /, $(addprefix ${DOTNET_HOME}, ${BENCHMARKS}))
DOTNET_BINS = $(join ${DOTNET_DIRS}/, $(addprefix bin/Release/net5.0/, ${BENCHMARKS}))
AOT_HOME = ${BENCHMARK_HOME}aot/

dotnet_build: $(addprefix ${BENCHMARK_HOME}, $(addsuffix _dotnet, ${BENCHMARKS}))

${BENCHMARK_HOME}%_dotnet: ${BENCHMARK_HOME}%.cs | ${DOTNET_DIRS}
	dotnet new console -o ${DOTNET_HOME}${*}
	rm ${DOTNET_HOME}${*}/Program.cs
	cp ${<} ${DOTNET_HOME}${*}
	cd ${DOTNET_HOME}${*}; dotnet build --configuration Release
	cp ${DOTNET_HOME}${*}/bin/Release/net5.0/${*} ${BENCHMARK_HOME}${*}_dotnet
	cp ${DOTNET_HOME}${*}/bin/Release/net5.0/${*}.dll ${BENCHMARK_HOME}
	cp ${DOTNET_HOME}${*}/bin/Release/net5.0/${*}.runtimeconfig.json ${BENCHMARK_HOME}

${DOTNET_DIRS}: | ${DOTNET_HOME}
	mkdir $@

${DOTNET_HOME}:
	mkdir $@

dotnet_clean:
	rm -fr ${DOTNET_HOME}


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


benchmark: $(addprefix benchmark/, $(BENCHMARK_PROGS)) \
$(addprefix benchmark/aot/, $(addsuffix _cs.so, $(BENCHMARKS)))
	python benchmark.py


${BENCHMARK_HOME}%: ${BENCHMARK_HOME}%.g gc
	./gc ${<}

${BENCHMARK_HOME}%_cs: ${BENCHMARK_HOME}%.cs
	mcs -out:$@ ${<}

${AOT_HOME}%_cs.so: ${BENCHMARK_HOME}%_cs | ${AOT_HOME}
	mono --aot=outfile=${@} -O=all ${<}

${BENCHMARK_HOME}%_cpp: ${BENCHMARK_HOME}%.cpp
	clang++ -O2 -g -fno-inline-functions -fno-unroll-loops -fno-vectorize -fno-builtin -o $@ ${<}

${AOT_HOME}:
	mkdir $@

options_test: gc
	python options_test.py

clean:
	rm -f $(addprefix benchmark/, $(BENCHMARK_PROGS))
	rm -fr ${AOT_HOME}
	rm -rf ${DOTNET_HOME}
	rm -f $(addsuffix .dll, $(addprefix ${BENCHMARK_HOME}, $(BENCHMARK_PROGS)))
	rm -f $(addsuffix .runtimeconfig.json, $(addprefix ${BENCHMARK_HOME}, $(BENCHMARK_PROGS)))
	rm -f $(addsuffix .ll, $(addprefix ${BENCHMARK_HOME}, $(BENCHMARK_PROGS)))
