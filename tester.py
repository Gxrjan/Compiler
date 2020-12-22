from pathlib import Path
import os
import re
import subprocess

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
    subprocess.run(['./gc', 'tests.g'])
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
    test_home = Path('tmp')
    file = open('tests_neg.g')
    lines = file.readlines()
    
    file_index = 0
    index = 0

    current_test = Path('test_prog0.g')
    outfile = open(test_home / current_test, 'w')

    while index < len(lines):
        if not lines[index].strip():
            outfile.close()
            file_index = file_index + 1
            current_test = Path('test_prog' + str(file_index)+'.g')
            outfile = open(test_home / current_test, 'w')
            while index < len(lines) and not lines[index].strip():
                index = index + 1
            index = index - 1
        else:
            outfile.write(lines[index])
        index = index + 1
    
    outfile.close()

    error_count = 0

    for folderName, subfolder, filenames in os.walk(test_home):
        for filename in filenames:
            msg = subprocess.run(['./gc', (test_home / filename)], capture_output=True)
            if (msg.returncode == 0):
                error_count = error_count + 1
                print(F'Error: compilation of tmp/{filename} succeeded unexpectedly:')
                test_file = open(test_home / filename)
                print(test_file.read())

    for folderName, subfolder, filenames in os.walk('tmp'):
        for filename in filenames:
            os.unlink(test_home / filename)
    return error_count

def main():
    pos_count = positive_test()
    neg_count = negative_test()
    print(F'There were total {pos_count} errors in positive test')
    print(F'There were total {neg_count} errors in negative test')
    



if __name__ == "__main__":
    main()