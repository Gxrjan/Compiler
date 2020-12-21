import subprocess
import time

def main():
    print("Running benchmarks...")
    print("")
    # ===========================================================================
    # Insertion sort
    print("========================================================")
    print("")
    n = 40000
    rand_seed = 100
    print(F"Insertion sort on {n} elements")
    start_time = time.time()
    subprocess.call(['./benchmark/insertion_sort', str(n), str(rand_seed)])
    g_time = (time.time() - start_time)
    print(F"G Execution time: {(1000*g_time):.0f}ms")

    start_time = time.time()
    subprocess.call(['./benchmark/insertion_sort.exe', str(n), str(rand_seed)])
    cs_time = (time.time() - start_time)
    print(F"CS Execution time: {(1000*cs_time):.0f}ms")


    start_time = time.time()
    subprocess.call(['./benchmark/insertion_sort_cpp', str(n), str(rand_seed)])
    cpp_time = (time.time() - start_time)
    print(F"C++ Execution time: {(1000*cpp_time):.0f}ms")

    print("")
    print("Results of Insertion Sort")
    # Results of Insertion sort
    if g_time < cs_time:
        print(F"G is ~{(cs_time / g_time):.2f} times faster than C#")
    else:
        print(F"G is ~{(g_time / cs_time):.2f} times slower than C#")

    if g_time < cpp_time:
        print(F"G is ~{(cpp_time / g_time):.2f} times faster than C++")
    else:
        print(F"G is ~{g_time / cpp_time:.2f} times slower than C++")
    print("")
    print("========================================================")
    print("")
    # ===========================================================================
    # prime sum
    n = 20000
    print(F"Prime_sum with prime_count={n}")
    start_time = time.time()
    subprocess.call(['./benchmark/prime_sum', str(n)])
    g_time = (time.time() - start_time)
    print(F"G Execution time: {(1000*g_time):.0f}ms")

    start_time = time.time()
    subprocess.call(['./benchmark/prime_sum.exe', str(n)])
    cs_time = (time.time() - start_time)
    print(F"CS Execution time: {(1000*cs_time):.0f}ms")


    start_time = time.time()
    subprocess.call(['./benchmark/prime_sum_cpp', str(n)])
    cpp_time = (time.time() - start_time)
    print(F"C++ Execution time: {(1000*cpp_time):.0f}ms")

    print("")
    print("Results of Prime sum")
    # Results of Prime sum
    if g_time < cs_time:
        print(F"G is ~{(cs_time / g_time):.2f} times faster than C#")
    else:
        print(F"G is ~{(g_time / cs_time):.2f} times slower than C#")

    if g_time < cpp_time:
        print(F"G is ~{(cpp_time / g_time):.2f} times faster than C++")
    else:
        print(F"G is ~{(g_time / cpp_time):.2f} times slower than C++")
    print("")
    print("========================================================")
    print("")
    # ===========================================================================
    # Tag
    n = 60
    print(F"Tag with n={n}")
    start_time = time.time()
    subprocess.call(['./benchmark/tag', str(n)])
    g_time = (time.time() - start_time)
    print(F"G Execution time: {(1000*g_time):.0f}ms")

    start_time = time.time()
    subprocess.call(['./benchmark/tag.exe', str(n)])
    cs_time = (time.time() - start_time)
    print(F"CS Execution time: {(1000*cs_time):.0f}ms")

    start_time = time.time()
    subprocess.call(['./benchmark/tag_cpp', str(n)])
    cpp_time = (time.time() - start_time)
    print(F"C++ Execution time: {(1000*cpp_time):.0f}ms")

    print("")
    print("Results of Tag")
    # Results of Tag
    if g_time < cs_time:
        print(F"G is ~{(cs_time / g_time):.2f} times faster than C#")
    else:
        print(F"G is ~{(g_time / cs_time):.2f} times slower than C#")

    if g_time < cpp_time:
        print(F"G is ~{(cpp_time / g_time):.2f} times faster than C++")
    else:
        print(F"G is ~{(g_time / cpp_time):.2f} times slower than C++")
    print("")
    print("========================================================")



if __name__ == "__main__":
    main()