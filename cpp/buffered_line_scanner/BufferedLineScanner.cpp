#include "BufferedLineScanner.hpp"

BufferedLineScanner::BufferedLineScanner() {
    m_buff_size = BufferedLineScanner::BUFF_SIZE;
    int ret = posix_memalign((void **)&m_buff, 4096, BUFF_SIZE);
    if (ret) {
        assert(0);
    }
    m_cursor = BufferedLineScanner::BUFF_SIZE;
    m_should_load = true;

    m_fd = -1;
}

BufferedLineScanner::~BufferedLineScanner() {
    if (m_buff) {
        free(m_buff);
        m_buff = NULL;
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

GetLineRet BufferedLineScanner::getline(char* & line_ptr) {
    assert(m_cursor <= m_buff_size);

    if (m_should_load) {
        size_t n_rest = m_buff_size - m_cursor;
        if (n_rest > 0) {
            memmove(m_buff, m_buff + m_cursor, n_rest);
        }
        m_buff_size = n_rest;
        char* buff_begin = m_buff + m_buff_size;
        ssize_t n_read = read(m_fd, buff_begin, BufferedLineScanner::BUFF_SIZE - m_buff_size);
        if (n_read == 0) {
            if (m_buff_size > 0) {
                line_ptr = new char[m_buff_size + 16];
                memcpy(line_ptr, m_buff, m_buff_size);
                line_ptr[m_buff_size] = '\n';
                line_ptr[m_buff_size + 1] = '\0';
                m_cursor = m_buff_size;
                return GET_LINE_SUCCESS;
            } else
                return GTE_LINE_FIN;
        } else if (n_read < 0) {
            // read errno, check errno
            // stop reading !
            return GET_LINE_ERRNO;
        } else {
            m_buff_size += n_read;
            m_cursor = 0;
            m_should_load = false;
        }
    }
    assert(m_buff_size <= BufferedLineScanner::BUFF_SIZE);

    size_t i;
    for (i = m_cursor; i < m_buff_size; ++ i) {
        if (m_buff[i] == '\n') {
            break;
        }
    }
    if (i >= m_buff_size) {
        if (m_cursor == 0) {
            // this line is larger than buff size
            // stop reading !
            return GET_LINE_TOO_LONG;
        }
        m_should_load = true;
        return getline(line_ptr);
    } else {
        size_t line_len = i - m_cursor + 1;
        line_ptr = new char[line_len + 16];
        memcpy(line_ptr, m_buff + m_cursor, line_len);
        line_ptr[line_len] = '\0';
        m_cursor = i + 1;
        return GET_LINE_SUCCESS;
    }
}
