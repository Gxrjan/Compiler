from helpers import *
    
def main():
    tests = ['Merge sort', 'Insertion sort', 'Prime sum', 'Tag', 'String perm']
    print("Running options test...")
    print_separator()
    # No options
    no_options = do_option_tests([])
    print_separator()

    # --nobounds
    no_bounds = do_option_tests(['--nobounds'])
    print_separator()

    # --nofree
    no_free = do_option_tests(['--nofree'])
    print_separator()

    # --noref
    no_ref = do_option_tests(['--noref'])
    print_separator()

    # for testing purposes
    # no_options = [1, 1, 1, 1, 1]
    # no_bounds = [1, 1, 1, 1, 1]
    # no_free = [1, 1, 1, 1, 1]
    # no_ref = [1, 1, 1, 1, 1]

    print(F'{"no options":>29}, {"--nobounds":>11}, {"--nofree":>10}, {"--noref":>10}')
    for i in range(len(tests)):
        print(F'{tests[i]:>14}: {(1000*no_options[i]):11.0f}ms {(1000*no_bounds[i]):10.0f}ms {(1000*no_free[i]):9.0f}ms {(1000*no_ref[i]):9.0f}ms')
    

    print_separator()





if __name__ == "__main__":
    main()