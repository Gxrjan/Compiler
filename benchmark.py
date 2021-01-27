import subprocess
import time
from pathlib import Path

def run_tests(name, args):
    start_time = time.time()
    subprocess.call(([str(name)] + args))
    g_time = (time.time() - start_time)
    print(F"G Execution time: {(1000*g_time):.0f}ms")

    start_time = time.time()
    subprocess.call([str(name) + '_cs'] + args)
    cs_time = (time.time() - start_time)
    print(F"CS Execution time: {(1000*cs_time):.0f}ms")


    start_time = time.time()
    subprocess.call([str(name) + '_cpp'] + args)
    cpp_time = (time.time() - start_time)
    print(F"C++ Execution time: {(1000*cpp_time):.0f}ms")
    return (g_time, cs_time, cpp_time)

def show_results(benchmark_name, g_time, cs_time, cpp_time):
    print("")
    print("Results of "+benchmark_name)
    if g_time < cs_time:
        print(F"G is ~{(cs_time / g_time):.2f} times faster than C#")
    else:
        print(F"G is ~{(g_time / cs_time):.2f} times slower than C#")

    if g_time < cpp_time:
        print(F"G is ~{(cpp_time / g_time):.2f} times faster than C++")
    else:
        print(F"G is ~{g_time / cpp_time:.2f} times slower than C++")

def print_separator():
    print("")
    print("========================================================")
    print("")

def process_args(args):
    result = list()
    for a in args:
        result.append(str(a))
    return result

def do_test(name, bin_name, args):
    benchmark_home = Path.cwd() / 'benchmark'
    print_separator()
    print(F"{name}. {args[0]} elements")
    (g_time, cs_time, cpp_time) = run_tests(benchmark_home / bin_name, process_args(args))
    show_results(name, g_time, cs_time, cpp_time)

def main():
    print("Running benchmarks...")

    # Merge sort 10000000 elements, 100 - random seed
    do_test("Merge sort", 'merge_sort', [10000000, 100])

    # Insertion sort 100000 elements, 100 - random seed
    do_test("Insertion sort", 'insertion_sort', [100000, 100])

    # prime sum 20000 elements
    do_test("Prime sum", 'prime_sum', [20000])

    # Tag 300 elements
    do_test("Tag", 'tag', [300])

    # String permutations on ABCDEFGHI
    do_test("String permutations", 'perm', ["ABCDEFGHIJ"])




if __name__ == "__main__":
    main()