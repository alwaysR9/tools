#ifndef __BUFFERED_LINE_SCANNER__
#define __BUFFERED_LINE_SCANNER__

#include <fstream>
#include <string>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

enum GetLineRet {
    GET_LINE_SUCCESS,
    GTE_LINE_FIN,
    GET_LINE_ERRNO,
    GET_LINE_TOO_LONG
};

class BufferedLineScanner {
public:
        BufferedLineScanner();
        ~BufferedLineScanner();

        bool open_scanner(const std::string & fname);
        void close_scanner();
        GetLineRet getline(char* & line_ptr);
        void freeline(char* & line_ptr);
private:
        static const size_t BUFF_SIZE = 1024 * 1024 * 16;
        int m_fd;
        size_t m_buff_size;
        char*  m_buff;
        size_t m_cursor;
        bool m_should_load;
};

#endif
