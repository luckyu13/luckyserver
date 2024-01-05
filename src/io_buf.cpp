
#include <string>
#include <string.h>

#include "io_buf.h"

IOBuffer::IOBuffer(): m_len(0), m_capacity(IO_BUFFER_DAEFUALT_SIZE), m_data(nullptr) {
    m_data = new char[m_capacity];
    memset(m_data, 0, m_capacity);
}

IOBuffer::IOBuffer(size_t capacity): m_len(0), m_capacity(capacity), m_data(nullptr) {
    m_data = new char[m_capacity];
    memset(m_data, 0, m_capacity);
}

IOBuffer::IOBuffer(const IOBuffer& o): m_len(o.m_len), m_capacity(o.m_capacity), m_data(nullptr) {
    m_data = new char[m_capacity];
    memset(m_data, 0, m_capacity);
    memcpy(m_data, o.m_data, m_len);
    m_capacity = o.m_capacity;
    m_len = o.m_len;
}

IOBuffer& IOBuffer::operator=(const IOBuffer& o) {
    if (this != &o) {
        m_data = new char[m_capacity];
        memset(m_data, 0, m_capacity);
        memcpy(m_data, o.m_data, m_len);
        m_capacity = o.m_capacity;
        m_len = o.m_len;
    }
    return *this;
}

IOBuffer::IOBuffer(IOBuffer&& m): m_capacity(m.m_capacity), m_len(m.m_len), m_data(m.m_data) {
    m.m_data = nullptr;
    m.m_capacity = 0;
    m.m_len = 0;
}

IOBuffer::~IOBuffer() {
    if (m_data != nullptr) {
        delete m_data;
    }
    m_data = nullptr;
    m_capacity = 0;
    m_len = 0;
}


void IOBuffer::add(const char* buf, size_t len) {
    if (m_len + len > m_capacity) {
        resize(static_cast<size_t>((m_len + len) * 1.5));
    }
    memcpy(m_data + m_len, buf, len);
    m_len += len;
}

void IOBuffer::del(size_t len) {
    size_t del_len = m_len > len ? len : m_len;
    size_t new_len = m_len >= len ? m_len - len : 0;
    memmove(m_data, m_data + del_len, m_len - del_len);
    memset(m_data + new_len, 0, m_len - new_len);
    m_len = new_len;
}

void IOBuffer::del(size_t start_pos, size_t end_pos) {
    end_pos = end_pos > m_len ? m_len : end_pos;
    memmove(m_data + end_pos, m_data + start_pos, m_len - end_pos);
    memset(m_data + (m_len - (end_pos - start_pos)), 0, end_pos - start_pos);
    m_len -= (end_pos - start_pos);
}

void IOBuffer::resize(size_t new_capacity) {
    if (new_capacity == m_capacity) return;
    if (new_capacity == 0) {
        delete m_data;
        m_data = nullptr;
        m_capacity = 0;
        m_len = 0;
        return;
    }
    char* new_data = new char[new_capacity];
    memset(new_data, 0, new_capacity);
    if (m_data != nullptr) {
        memcpy(new_data, m_data, m_len);
        delete m_data;
    }
    m_capacity = new_capacity;
    m_data = new_data;
}

void IOBuffer::clear() {
    memset(m_data, 0, m_len);
    m_len = 0;
}
