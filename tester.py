import subprocess
import time

def main():
    print("Compiling...")
    subprocess.call(['./gc', 'benchmark/insertion_sort.g'])
    subprocess.call(['mcs', 'benchmark/insertion_sort.cs'])
    subprocess.call(['./gc', 'benchmark/prime_sum.g'])
    subprocess.call(['mcs', 'benchmark/prime_sum.cs'])
    subprocess.call(['./gc', 'benchmark/tag.g'])
    subprocess.call(['mcs', 'benchmark/tag.cs'])
    print("Running benchmarks...")


    # Insertion sort
    n = 40000
    rand_seed = 100
    print("Insertion sort on %d elements" % n)
    start_time = time.time()
    subprocess.call(['./benchmark/insertion_sort', str(n), str(rand_seed)], stdout=subprocess.DEVNULL)
    g_time = (time.time() - start_time)
    print("G Execution time: %s" % g_time)

    start_time = time.time()
    subprocess.call(['./benchmark/insertion_sort.exe', str(n), str(rand_seed)], stdout=subprocess.DEVNULL)
    cs_time = (time.time() - start_time)
    print("CS Execution time: %s" % cs_time)
    is_tmp = g_time / cs_time
    if is_tmp < 1:
        print("G is ~%f times faster" % is_tmp)
    else:
        print("G is ~%f times slower" % is_tmp)


    # prime sum
    n = 20000
    print("Prime_sum with prime_count=%d" % n)
    start_time = time.time()
    subprocess.call(['./benchmark/prime_sum', str(n)], stdout=subprocess.DEVNULL)
    g_time = (time.time() - start_time)
    print("G Execution time: %s" % g_time)

    start_time = time.time()
    subprocess.call(['./benchmark/prime_sum.exe', str(n)], stdout=subprocess.DEVNULL)
    cs_time = (time.time() - start_time)
    print("CS Execution time: %s" % cs_time)
    ps_tmp = g_time / cs_time
    if ps_tmp < 1:
        print("G is ~%f times faster" % ps_tmp)
    else:
        print("G is ~%f times slower" % ps_tmp)

    # tag
    n = 100
    print("Tag with n=%d" % n)
    start_time = time.time()
    subprocess.call(['./benchmark/tag', str(n)], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
    g_time = (time.time() - start_time)
    print("G Execution time: %s" % g_time)

    start_time = time.time()
    subprocess.call(['./benchmark/tag.exe', str(n)], stdout=subprocess.DEVNULL)
    cs_time = (time.time() - start_time)
    print("CS Execution time: %s" % cs_time)
    tag_tmp = g_time / cs_time
    if tag_tmp < 1:
        print("G is ~%f times faster" % tag_tmp)
    else:
        print("G is ~%f times slower" % tag_tmp)




if __name__ == "__main__":
    main()