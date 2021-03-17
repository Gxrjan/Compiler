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

    # String permutations on ABCDEFGHIJ
    do_test("String permutations", 'perm', ["ABCDEFGHIJ"])

    # Prime count on 1000000000
    do_test("Prime count", 'prime_count', [1000000000])

    # Hash table on 100000
    do_test("Hash table", 'hash_table', [1000000, 13])




if __name__ == "__main__":
    main()