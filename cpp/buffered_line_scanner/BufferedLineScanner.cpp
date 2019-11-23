#include "BufferedLineScanner.hpp"

#include <string.h>

BufferedLineScanner::BufferedLineScanner() {
    for (int i = 0; i < 2; ++ i) {
        int ret = posix_memalign((void **)&m_buff[i], 4096, READ_SIZE);
        if (ret) {
            assert(0);
        }
    }

    // ready to read from file
    m_begin[0] = READ_SIZE;
    m_end[0]   = READ_SIZE;
    m_begin[1] = 0;
    m_end[1]   = 0;
    m_should_read_file = true;

    m_fd = -1;
}

BufferedLineScanner::~BufferedLineScanner() {
    for (int i = 0; i < 2; ++ i) {
        if (m_buff[i]) {
            free(m_buff[i]);
            m_buff[i] = NULL;
        }
    }
}

bool BufferedLineScanner::open_scanner(const std::string & fname) {
    m_fd = open(fname.c_str(), O_RDONLY | O_DIRECT);
    return m_fd >= 0;
}

void BufferedLineScanner::close_scanner() {
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
}

void BufferedLineScanner::freeline(char* & line_ptr) {
    if (line_ptr) {
        delete [] line_ptr;
        line_ptr = NULL;
    }
}

bool BufferedLineScanner::prepare_to_read_file() {
    // there is no enough room
    if ((m_end[0] - m_begin[0]) + (m_end[1] - m_begin[1]) >= READ_SIZE)
        return false;

    // there are enough room
    if (m_end[0] - m_begin[0] > 0)
        memmove(m_buff[0], m_buff[0] + m_begin[0], m_end[0] - m_begin[0]);
    m_end[0] = m_end[0] - m_begin[0];
    m_begin[0] = 0;

    if (m_end[1] - m_begin[1] > 0)
        memcpy(m_buff[0] + m_end[0], m_buff[1] + m_begin[1], m_end[1] - m_begin[1]);
    m_end[0] += m_end[1] - m_begin[1];
    m_begin[1] = m_end[1];
    return true;
}

void BufferedLineScanner::extract_line(char* & line_ptr, size_t & b, size_t e) {
    line_ptr = new char[e - b + 16];
    memcpy(line_ptr, m_buff[0] + b, e - b);
    line_ptr[e - b] = '\0';
    b = e + 1;
}

void BufferedLineScanner::copy_data_to_ready_buff(const int n_read) {
    if (n_read >= READ_SIZE - m_end[0]) {
        memcpy(m_buff[0] + m_end[0], m_buff[1], READ_SIZE - m_end[0]);
        m_begin[1] = READ_SIZE - m_end[0];
        m_end[1] = n_read;
        m_end[0] = READ_SIZE;
    } else {
        memcpy(m_buff[0] + m_end[0], m_buff[1], n_read);
        m_begin[1] = n_read;
        m_end[1] = n_read;
        m_end[0] += n_read;
    }
}

GetLineRet BufferedLineScanner::getline(char* & line_ptr) {
    assert(m_begin[0] <= m_end[0] && m_begin[1] <= m_end[1]);
//    fprintf(stderr, "[begin] m_begin[0]:%lu, m_end[0]:%lu\n", m_begin[0], m_end[0]);
//    fprintf(stderr, "[begin] m_begin[1]:%lu, m_end[1]:%lu\n", m_begin[1], m_end[1]);

    if (m_should_read_file) {
        assert(m_begin[0] > 0);

//        fprintf(stderr, "[before prepare] m_begin[0]:%lu, m_end[0]:%lu\n", m_begin[0], m_end[0]);
//        fprintf(stderr, "[before prepare] m_begin[1]:%lu, m_end[1]:%lu\n", m_begin[1], m_end[1]);
        if (!prepare_to_read_file()) {
//            fprintf(stderr, "[too long] m_begin[0]:%lu, m_end[0]:%lu\n", m_begin[0], m_end[0]);
//            fprintf(stderr, "[too long] m_begin[1]:%lu, m_end[1]:%lu\n", m_begin[1], m_end[1]);
            return GET_LINE_TOO_LONG;
        }
//        fprintf(stderr, "[after prepare] m_begin[0]:%lu, m_end[0]:%lu\n", m_begin[0], m_end[0]);
//        fprintf(stderr, "[after prepare] m_begin[1]:%lu, m_end[1]:%lu\n", m_begin[1], m_end[1]);

        ssize_t n_read = read(m_fd, m_buff[1], READ_SIZE);
//        fprintf(stderr, "[read] n_read:%d\n", n_read);
        if (n_read < 0) {
            return GET_LINE_ERRNO;
        }
        if (n_read == 0) {
            if (m_end[0] - m_begin[0] == 0) {
                return GET_LINE_FIN;
            } else {
                extract_line(line_ptr, m_begin[0], m_end[0]);
                return GET_LINE_SUCCESS;
            }
        } else {
            copy_data_to_ready_buff(n_read);
            m_should_read_file = false;
//           fprintf(stderr, "[after copy] m_begin[0]:%lu, m_end[0]:%lu\n", m_begin[0], m_end[0]);
//           fprintf(stderr, "[after copy] m_begin[1]:%lu, m_end[1]:%lu\n", m_begin[1], m_end[1]);
        }
//        getchar();
    }
    assert(m_begin[0] <= m_end[0] && m_begin[1] <= m_end[1]);

    int i;
    for (i = m_begin[0]; i < m_end[0]; ++ i) {
        if (m_buff[0][i] == '\n') {
            break;
        }
    }
    if (i < m_end[0]) {
        extract_line(line_ptr, m_begin[0], i);
        return GET_LINE_SUCCESS;
    } else {
        if (m_begin[0] == 0) {
            return GET_LINE_TOO_LONG;
        }
        m_should_read_file = true;
        return getline(line_ptr);
    }
}
