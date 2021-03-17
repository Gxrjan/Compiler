from pathlib import Path
import os
import re
import subprocess
import shutil
from helpers import *

def positive_test():
    gc_output_file = Path('result')
    tests_bin_file = Path('tests')
    pattern = re.compile(r'//[ ]*expect:(.*)')
    file = open(Path.cwd() / 'tests.g')
    gold = list()
    line = 1
    for l in file.readlines():
        mo = pattern.search(l)
        if mo is not None:
            gold.append((line, mo.group(1).strip()))
        line = line + 1


    outfile = open(gc_output_file, 'w')
    subprocess.run('./'+str(tests_bin_file), stdout=outfile)
    output = list()
    outfile = open(gc_output_file)
    for l in outfile.readlines():
        output.append(l.rstrip())

    # fill output list so it has the same number of elements as gold standart
    for i in range(len(gold)-len(output)):
        output.append(None)

    error_count = 0
    for i in range(len(gold)):
        if gold[i][1] != output[i]:
            error_count = error_count + 1
            print(F'error on line {gold[i][0]}: {gold[i][1]} != {output[i]}')

    file.close()
    os.unlink(gc_output_file)
    os.unlink(tests_bin_file)


    return error_count


def negative_test():
    try:
        os.mkdir('tmp')
    except:
        pass
    test_home = Path('tmp')
    break_into_separate_progs('tests_neg.g', test_home)

    error_count = 0

    for folderName, subfolder, filenames in os.walk(test_home):
        for filename in filenames:
            msg = subprocess.run(['./gc', (test_home / filename)], capture_output=True)
            if (msg.returncode == 0):
                error_count = error_count + 1
                print(F'Error: compilation of {test_home}/{filename} succeeded unexpectedly:')
                test_file = open(test_home / filename)
                print(test_file.read())

    shutil.rmtree(test_home)
    return error_count


def bounds_test():
    try:
        os.mkdir('tmp')
    except:
        pass
    test_home = Path('tmp')
    break_into_separate_progs('tests_bounds.g', test_home)

    error_count = 0

    for folderName, subfolder, filenames in os.walk(test_home):
        for filename in filenames:
            msg = subprocess.run(['./gc', (test_home / filename), '-o', (test_home / filename.strip('.g'))], capture_output=True)
            if (msg.returncode != 0):
                print(F'Error: compilation of {test_home}/{filename} failed unexpectedly:')
                test_file = open(test_home / filename)
                print(test_file.read())
                error_count += 1
            p = subprocess.Popen(['./'+str(test_home / filename.strip('.g'))], stderr=subprocess.DEVNULL)
            pid, exit_status, res_usage = os.wait4(p.pid, 0)
            if (exit_status == 0):
                error_count += 1
                print(F'Error: {test_home}/{filename} executed successfully:')
                test_file = open(test_home / filename.strip())
                print(test_file.read())

    shutil.rmtree(test_home)

    return error_count


def main():
    pos_count = positive_test()
    neg_count = negative_test()
    bounds_count = bounds_test()
    print(F'There were total {pos_count} errors in positive test')
    print(F'There were total {neg_count} errors in negative test')
    print(F'There were total {bounds_count} errors in bounds test')
    



if __name__ == "__main__":
    main()