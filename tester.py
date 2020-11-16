import subprocess
import time

def main():
    print("Running benchmarks...")


    # Insertion sort
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
    is_tmp = g_time / cs_time
    if is_tmp < 1:
        print(F"G is ~{is_tmp:.2f} times faster")
    else:
        print(F"G is ~{is_tmp:.2f} times slower")


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
    ps_tmp = g_time / cs_time
    if ps_tmp < 1:
        print(F"G is ~{ps_tmp:.2f} times faster")
    else:
        print(F"G is ~{ps_tmp:.2f} times slower")

    # tag
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
    tag_tmp = g_time / cs_time
    if tag_tmp < 1:
        print(F"G is ~{tag_tmp:.2f} times faster")
    else:
        print(F"G is ~{tag_tmp:.2f} times slower")




if __name__ == "__main__":
    main()