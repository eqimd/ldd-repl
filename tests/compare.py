from os import listdir, popen, getcwd
from os.path import isfile, join
import subprocess

def get_ldd_repl_out(filename):
    return subprocess.check_output([getcwd() + '/ldd-repl', filename])

def get_ldd_orig_out(filename):
    return subprocess.check_output(['ldd', filename])

def main():
    bin_path = '/bin'
    print('Bin path:', bin_path)
    bin_files = [(bin_path + '/' + f) for f in listdir(bin_path) if isfile(join(bin_path, f))]
    print('Bin files count:', len(bin_files))

    test_files_counts = min(len(bin_files), 100)

    for filename in bin_files[:test_files_counts]:
        print('Current file:', filename)
        try:
            out_orig = set()
            for p_str in get_ldd_orig_out(filename).decode('ISO-8859-1').split('\n')[1:-1]:
                out_orig.add(p_str.strip().split()[0].split('/')[-1])

            out_repl = set()
            for p_str in get_ldd_repl_out(filename).decode('ISO-8859-1').split('\n')[:-1]:
                p_list = p_str.strip().split()
                out_repl.add(p_list[0])

            if out_orig != out_repl:
                print('Not equal on', filename)
                print('Original:', out_orig)
                print('Replica:', out_repl)
                exit(1) 

        except subprocess.CalledProcessError:
            continue


if __name__ == '__main__':
    main()