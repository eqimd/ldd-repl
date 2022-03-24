from sys import argv
import subprocess

def main(orig_file, repl_file):
    out_orig = set()
    for p_str in open(orig_file, 'r').read().split('\n')[1:-1]:
        out_orig.add(p_str.strip().split()[0].split('/')[-1])

    out_repl = set()
    for p_str in open(repl_file, 'r').read().split('\n')[:-1]:
        p_list = p_str.strip().split()
        out_repl.add(p_list[0])

    if out_orig != out_repl:
        print('Not equal!')
        print('Original:', out_orig)
        print('Replica:', out_repl)
        exit(1)


if __name__ == '__main__':
    main(argv[1], argv[2])