#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "BufferedLineScanner.hpp"
using namespace std;


void work_buffered(const std::string & fname) {
    BufferedLineScanner scanner;
    if (!scanner.open_scanner(fname)) {
        fprintf(stderr, "open %s fail\n", fname.c_str());
        return;
    }

    char* line_buf = NULL;

    int c = 0;
    int ret = 0;
    while ((ret = scanner.getline(line_buf)) == GET_LINE_SUCCESS) {
        ++ c;

        //fprintf(stderr, "%s\n", line_buf);
        scanner.freeline(line_buf);
    }
    if (ret == GET_LINE_ERRNO) {
        fprintf(stderr, "reading errno:%d\n", errno);
    } else if (ret == GET_LINE_TOO_LONG) {
        fprintf(stderr, "reading encounted a too long line, stop reading!\n");
    }

    fprintf(stderr, "file %s contains %d line\n", fname.c_str(), c);
    scanner.close_scanner();
}

void work_norm(const std::string & fname) {
    std::ifstream fin;
    fin.open(fname);
    if (!fin.is_open()) {
        fprintf(stderr, "open %s fail\n", fname.c_str());
        return;
    }

    const int buf_len = 4096;
    char line_buf[buf_len];

    int c = 0;
    while (fin.getline(line_buf, buf_len)) {
        ++ c;
    }

    fprintf(stderr, "file %s contains %d line\n", fname.c_str(), c);
    fin.close();
}

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: ./scan [file_name]\n");
        exit(1);
    }

    std::string file_name(argv[1]);
    work_buffered(file_name);
//    work_norm(file_name);

    return 0;
}
