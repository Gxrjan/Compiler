from helpers import *



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