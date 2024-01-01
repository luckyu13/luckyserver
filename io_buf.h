#pragma once

#include <string>

#define IO_BUFFER_DAEFUALT_SIZE 1024*2

class IOBuffer {
public:
    IOBuffer();
    IOBuffer(size_t capacity);
    IOBuffer(const IOBuffer& o);
    IOBuffer(IOBuffer&& m);
    ~IOBuffer();

public:
    void add(const char* buf, size_t len);
    void del(size_t len);
    void del(size_t start_pos, size_t end_pos);

public:
    IOBuffer& operator=(const IOBuffer& o);
    
public:
    const char* data() { return m_data; }
    size_t capacity() const { return m_capacity; }
    size_t len() const { return m_len; }

protected:
    void resize(size_t new_capacity);

private:
    char* m_data;
    size_t m_capacity;
    size_t m_len;
};