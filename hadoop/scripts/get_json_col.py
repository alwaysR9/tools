#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import json

# run:
#   python get_json_col.py "wareid,storeid,cid3" "json"
#   python get_json_col.py "wareid,storeid,cid3" "txt"
#   获取数据中(json格式)的wareid,storeid,cid3字段, 其中，json数据是最后一列

def main():
    if len(sys.argv) != 3:
        sys.stderr.write("[ERROR] arg num is wrong, exit!\n")
        exit(1)

    keys = set(sys.argv[1].split(','))
    fmt = sys.argv[2]
    assert(fmt in ['json', 'txt'])

    for line in sys.stdin:
        line = line.strip()
        p = line.split('\t')

        try:
            obj = json.loads(p[-1])
            if fmt == 'txt':
                print '\t'.join([obj[k] for k in iter(keys)])
            elif fmt == 'json':
                out = {}
                for k in iter(keys):
                    out[k] = obj[k]
                print json.dumps(out) + "\n"
        except Exception as exp:
            sys.stderr.write("[EXP] %s: %s\n" % (line, str(exp)))


if __name__ == "__main__":
    main()
