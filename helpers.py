import subprocess
import time
from pathlib import Path
import functools
import operator
import os

def run_tests(name, args):
    n = 3

    total = 0
    for i in range(n):
        total = total + run_test_g(name, args)
    g_time = total / n

    total = 0
    for i in range(n):
        total = total + run_test_cs_mono(name, args)
    cs_mono_time = total / n

    total = 0
    for i in range(n):
        total = total + run_test_cs_mono_aot(name, args)
    cs_mono_aot_time = total / n

    total = 0
    for i in range(n):
        total = total + run_test_cpp(name, args)
    cpp_time = total / n
    return (g_time, cs_mono_time, cs_mono_aot_time, cpp_time)


def sub(t):
    return t[0] - t[1]

def sum_up(l):
    result = 0
    for v in l:
        result += v
    return result

def run_test_g(name, args):
    p = subprocess.Popen(([str(name)] + args))
    pid, exit_status, res_usage = os.wait4(p.pid, 0)
    result = res_usage.ru_utime + res_usage.ru_stime
    print(f"G Execution time: {(1000 * result):.0f} ms (user = {1000 * res_usage.ru_utime:.0f} ms, " +
          f"sys = {1000 * res_usage.ru_stime:.0f} ms)")
    return result


def run_test_cs_mono(name, args):
    p = subprocess.Popen(([str(name) + '_cs'] + args))
    pid, exit_status, res_usage = os.wait4(p.pid, 0)
    result = res_usage.ru_utime + res_usage.ru_stime
    print(f"CS(JIT) Execution time: {(1000 * result):.0f} ms (user = {1000 * res_usage.ru_utime:.0f} ms, " +
          f"sys = {1000 * res_usage.ru_stime:.0f} ms)")
    return result

def run_test_cs_mono_aot(name, args):
    args = ['mono', '--aot-path=benchmark/aot', (str(name) + '_cs')] + args
    p = subprocess.Popen(args)
    pid, exit_status, res_usage = os.wait4(p.pid, 0)
    result = res_usage.ru_utime + res_usage.ru_stime
    print(f"CS(AOT) Execution time: {(1000 * result):.0f} ms (user = {1000 * res_usage.ru_utime:.0f} ms, " +
          f"sys = {1000 * res_usage.ru_stime:.0f} ms)")
    return result


def run_test_cpp(name, args):
    p = subprocess.Popen(([str(name) + '_cpp'] + args))
    pid, exit_status, res_usage = os.wait4(p.pid, 0)
    result = res_usage.ru_utime + res_usage.ru_stime
    print(f"Cpp Execution time: {(1000 * result):.0f} ms (user = {1000 * res_usage.ru_utime:.0f} ms, " +
          f"sys = {1000 * res_usage.ru_stime:.0f} ms)")
    return result

def show_results(benchmark_name, g_time, cs_mono_time, cs_mono_aot_time, cpp_time):
    print("")
    print("Results of "+benchmark_name)
    if g_time < cs_mono_time:
        print(F"G is ~{(cs_mono_time / g_time):.2f} times faster than C#(JIT)")
    else:
        print(F"G is ~{(g_time / cs_mono_time):.2f} times slower than C#(JIT)")

    if g_time < cs_mono_aot_time:
        print(F"G is ~{(cs_mono_aot_time / g_time):.2f} times faster than C#(AOT)")
    else:
        print(F"G is ~{(g_time / cs_mono_aot_time):.2f} times slower than C#(AOT)")

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
    (g_time, cs_mono_time, cs_mono_aot_time, cpp_time) = run_tests(benchmark_home / bin_name, process_args(args))
    show_results(name, g_time, cs_mono_time, cs_mono_aot_time, cpp_time)



def build(options):
    subprocess.call(['make', 'clean'])
    subprocess.call(['./gc'] + options + ['benchmark/merge_sort.g'])
    subprocess.call(['./gc'] + options + ['benchmark/insertion_sort.g'])
    subprocess.call(['./gc'] + options + ['benchmark/prime_sum.g'])
    subprocess.call(['./gc'] + options + ['benchmark/tag.g'])
    subprocess.call(['./gc'] + options + ['benchmark/perm.g'])
    subprocess.call(['./gc'] + options + ['benchmark/prime_count.g'])
    subprocess.call(['./gc'] + options + ['benchmark/hash_table.g'])

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


def break_into_separate_progs(source, test_home):
    file = open(source)
    lines = file.readlines()
    file_index = 0
    current_code = ''
    
    for l in lines:
        if not l.strip():
            if (current_code):
                current_test = Path(f'test_prog{file_index}.g')
                outfile = open(test_home/current_test, 'w')
                outfile.write(current_code)
                outfile.close()
                file_index += 1
            current_code = ''
        else:
            current_code += l
    if (current_code):
        current_test = Path(f'test_prog{file_index}.g')
        outfile = open(test_home/current_test, 'w')
        outfile.write(current_code)
        outfile.close()
    
