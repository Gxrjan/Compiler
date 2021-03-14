from helpers import *
    
def main():
    tests_specs = [
        ("Merge sort", 'merge_sort', [10000000, 100]),
        ("Insertion sort", 'insertion_sort', [100000, 100]),
        ("Prime sum", 'prime_sum', [20000]),
        ("Tag", 'tag', [300]),
        ("String perm", 'perm', ["ABCDEFGHIJ"]),
        ("Prime count", 'prime_count', [1000000000])
    ]
    print("Running options test...")
    print_separator()
    
    # No options
    no_options = do_option_tests(tests_specs, [])
    print_separator()

    # --nobounds
    no_bounds = do_option_tests(tests_specs, ['--nobounds'])
    print_separator()

    # --nofree
    no_free = do_option_tests(tests_specs, ['--nofree'])
    print_separator()

    # --noref
    no_ref = do_option_tests(tests_specs, ['--noref'])
    print_separator()

    # --nonull
    no_null = do_option_tests(tests_specs, ['--nonull'])
    print_separator()

    # for testing purposes
    # no_options = [1, 1, 1, 1, 1]
    # no_bounds = [1, 1, 1, 1, 1]
    # no_free = [1, 1, 1, 1, 1]
    # no_ref = [1, 1, 1, 1, 1]

    print(F'{"no options":>29}, {"--nobounds":>11}, {"--nofree":>10}, {"--noref":>10}, {"--nonull":>10}')
    for i in range(len(tests_specs)):
        print(F'{tests_specs[i][0]:>14}: {(1000*no_options[i]):11.0f}ms {(1000*no_bounds[i]):10.0f}ms {(1000*no_free[i]):9.0f}ms {(1000*no_ref[i]):9.0f}ms {(1000*no_null[i]):9.0f}ms')
    

    print_separator()





if __name__ == "__main__":
    main()