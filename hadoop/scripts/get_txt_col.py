#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import json
import traceback

# run:
#   python get_col_value.py "0,2" "txt"
#   获取文件(txt格式)的第0，第2列字段，以txt格式输出

def main():
    if len(sys.argv) != 3:
        sys.stderr.write("[ERROR] arg num is wrong, exit!\n")
        exit(1)

    keys = sys.argv[1].split(',')
    fmt = sys.argv[2]
    assert(fmt in ['json', 'txt'])
    if fmt == 'json':
        sys.stderr.write("[ERROR] can not output json, exit!\n")
        exit(1)

    for line in sys.stdin:
        line = line.strip()
        p = line.split('\t')
        try:
            print '\t'.join([p[int(i)] for i in keys])
        except Exception as exp:
            sys.stderr.write("[EXP] %s: %s\n" % (line, str(exp)))
            traceback.print_exc()


if __name__ == "__main__":
    main()
