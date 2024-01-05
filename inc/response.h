#pragma once

#include <map>
#include <sstream>
#include <algorithm>

#include "http_status_code.h"
#include "request.h"

class Request;

class Response {
public:
    Response(): m_status_code(StatusCode::OK), m_buf(IO_BUFFER_DAEFUALT_SIZE), m_req_ptr(nullptr) {}
    Response(const Response&) = delete;
    Response(Response&&) = delete;
    Response(Request* request): m_status_code(StatusCode::OK), m_buf(IO_BUFFER_DAEFUALT_SIZE), m_req_ptr(request) {}
    ~Response();

public:
    void write_body(const char* buf, size_t len);
    void add_header(std::string key, std::string value) {
        m_headers[key] = value;
    }
    void write_status_code(StatusCode status_code) {
        m_status_code = status_code;
    }

public:
    typedef std::map<std::string, std::string> header_map;

public:
    StatusCode status_code() const { return m_status_code; }
    const header_map header() const { return m_headers; }
    bool has_body() const { return m_buf.len() != 0; }
    const IOBuffer& body()const { return m_buf; }
    std::string head();

public:
    void reset();

private:
    Request*    m_req_ptr;
    StatusCode  m_status_code;
    IOBuffer    m_buf;
    header_map  m_headers;
};

