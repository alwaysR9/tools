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

//===============================//
// Notice:
//   This Class Is Not Thread Safe
//===============================//

enum GetLineRet {
    GET_LINE_SUCCESS,
    GET_LINE_FIN,
    GET_LINE_ERRNO,
    GET_LINE_TOO_LONG
};

class BufferedLineScanner {

public:
        BufferedLineScanner();
        ~BufferedLineScanner();

        bool open_scanner(const std::string & fname);
        void close_scanner();

        // do not return '\n'
        GetLineRet getline(char* & line_ptr);
        void freeline(char* & line_ptr);

private:
    bool prepare_to_read_file();
    void extract_line(char* & line_ptr, size_t & b, size_t e);
    void copy_data_to_ready_buff(const int n_read);


private:
        static const size_t READ_SIZE = 1024 * 1024 * 16;
        int m_fd;

        // if m_buff[0] do not contain a line,
        // then do step 1
        //
        // step 1:
        // always read file to m_buff[1] with READ_SIZE
        // step 2:
        // then copy m_buff[1] to m_buff[0], return a line to user from m_buff[0]
        //
        // m_end[0] is the end position in m_buff[0]
        // m_end[1] is the end position in m_buff[1]
        //
        // m_begin[0] is the beginning of next line
        // m_begin[1] is a discarded variable
        char*  m_buff[2];
        size_t m_end[2];
        size_t m_begin[2];
        bool   m_should_read_file;
};

#endif
