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

def run_test_g(name, args):
    start_time = time.time()
    subprocess.call(([str(name)] + args), stdout=subprocess.DEVNULL)
    g_time = (time.time() - start_time)
    print(F"G Execution time: {(1000*g_time):.0f}ms")
    return g_time

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



def build(options):
    subprocess.call(['make', 'clean'])
    subprocess.call(['./gc'] + options + ['benchmark/merge_sort.g'])
    subprocess.call(['./gc'] + options + ['benchmark/insertion_sort.g'])
    subprocess.call(['./gc'] + options + ['benchmark/prime_sum.g'])
    subprocess.call(['./gc'] + options + ['benchmark/tag.g'])
    subprocess.call(['./gc'] + options + ['benchmark/perm.g'])

def do_option_test(name, bin_name, args):
    benchmark_home = Path.cwd() / 'benchmark'
    print(F"{name}. {args[0]} elements")
    total = 0
    count = 3
    for i in range(count):
        g_time = run_test_g(benchmark_home / bin_name, process_args(args))
        total = total + g_time
    average = total / count
    return average

def do_option_tests(tests_specs, options):
    print(F'Options provided: {options}')
    build(options)
    result = list()
    for s in tests_specs:
        result.append(do_option_test(s[0], s[1], s[2]))
    
    return result