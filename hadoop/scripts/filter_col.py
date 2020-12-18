#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import random

# run:
#   python filter_col.py 0 123
#   将第0列字符串，包含'123'的行，过滤出来

def main():
    if len(sys.argv) != 2:
        sys.stderr.write("[ERROR] arg num is wrong, exit!\n")
        exit(1)

    col = int(sys.argv[1])
    val = str(sys.argv[2])

    for line in sys.stdin:
        line = line.strip()
        p = line.split('\t')
        if len(p) < col+1:
            continue
        if val in p[col]:
            print line


if __name__ == "__main__":
    main()
