SOURCES = head.h main.cpp Scanner.cpp Token.cpp Expr.cpp Parser.cpp Types.cpp Program.cpp Statement.cpp Variable.cpp Checker.cpp Translator_LLVM.cpp
all: gc G_runtime_library.o Types.o


.PHONY: options_test test benchmark
gc: $(SOURCES) 
	g++ -o gc -Wall -std=c++17 -g $(SOURCES)

G_runtime_library.o: G_runtime_library.cpp
	clang++ -Wall -c -g -O2 G_runtime_library.cpp 

Types.o: Types.cpp
	clang++ -Wall -c -g -O2 Types.cpp


test: gc tests.g tests_neg.g test_mem.g
	./gc tests.g
	python tester.py

mem_leak: gc
	./gc test_mem.g
	valgrind ./test_mem


benchmark: benchmark/insertion_sort benchmark/prime_sum benchmark/tag benchmark/insertion_sort_cs benchmark/prime_sum_cs benchmark/tag_cs benchmark/insertion_sort_cpp benchmark/prime_sum_cpp benchmark/tag_cpp \
			benchmark/perm benchmark/perm_cs benchmark/perm_cpp benchmark/merge_sort benchmark/merge_sort_cs benchmark/merge_sort_cpp benchmark/hash_table benchmark/hash_table_cs benchmark/hash_table_cpp
	python benchmark.py


benchmark/insertion_sort: gc benchmark/insertion_sort.g
	./gc benchmark/insertion_sort.g

benchmark/insertion_sort_cs: benchmark/insertion_sort.cs
	mcs -out:benchmark/insertion_sort_cs benchmark/insertion_sort.cs

benchmark/prime_sum: gc benchmark/prime_sum.g
	./gc benchmark/prime_sum.g

benchmark/prime_sum_cs: benchmark/prime_sum.cs
	mcs -out:benchmark/prime_sum_cs benchmark/prime_sum.cs

benchmark/tag: gc benchmark/tag.g
	./gc benchmark/tag.g

benchmark/tag_cs: benchmark/tag.cs
	mcs -out:benchmark/tag_cs benchmark/tag.cs


benchmark/insertion_sort_cpp: benchmark/insertion_sort.cpp
	clang++ -O2 -o benchmark/insertion_sort_cpp benchmark/insertion_sort.cpp

benchmark/prime_sum_cpp: benchmark/prime_sum.cpp
	clang++ -O2 -o benchmark/prime_sum_cpp benchmark/prime_sum.cpp

benchmark/tag_cpp: benchmark/tag.cpp
	clang++ -O2 -o benchmark/tag_cpp benchmark/tag.cpp


benchmark/perm: gc benchmark/perm.g
	./gc benchmark/perm.g

benchmark/perm_cs: benchmark/perm.cs
	mcs -out:benchmark/perm_cs benchmark/perm.cs


benchmark/perm_cpp: benchmark/perm.cpp
	clang++ -O2 -o benchmark/perm_cpp benchmark/perm.cpp


benchmark/merge_sort: gc benchmark/merge_sort.g
	./gc benchmark/merge_sort.g

benchmark/merge_sort_cs: benchmark/merge_sort.cs
	mcs -out:benchmark/merge_sort_cs benchmark/merge_sort.cs


benchmark/merge_sort_cpp: benchmark/merge_sort.cpp
	clang++ -O2 -o benchmark/merge_sort_cpp benchmark/merge_sort.cpp


benchmark/prime_count: gc benchmark/prime_count.g
	./gc benchmark/prime_count.g

benchmark/prime_count_cs: benchmark/prime_count.cs
	mcs -out:benchmark/prime_count_cs benchmark/prime_count.cs

benchmark/prime_count_cpp: benchmark/prime_count.cpp
	clang++ -O2 -o benchmark/prime_count_cpp benchmark/prime_count.cpp


benchmark/hash_table: gc benchmark/hash_table.g
	./gc benchmark/hash_table.g

benchmark/hash_table_cs: benchmark/hash_table.cs
	mcs -out:benchmark/hash_table_cs benchmark/hash_table.cs

benchmark/hash_table_cpp: benchmark/hash_table.cpp
	clang++ -O2 -o benchmark/hash_table_cpp benchmark/hash_table.cpp

options_test: gc
	python options_test.py

clean:
	rm benchmark/insertion_sort benchmark/prime_sum benchmark/tag benchmark/perm benchmark/merge_sort