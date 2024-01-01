#pragma once

#include <map>
#include <sstream>
#include <algorithm>

#include "http_status_code.h"
#include "request.h"
#include "conn.h"

class Conn;
class Request;

class Response {
public:
    Response() = delete;
    Response(const Response&) = delete;
    Response(Response&&) = delete;
    Response(Conn& conn, Request* request): m_conn(conn), m_status_code(StatusCode::OK), m_buf(RespBody{nullptr, 0}), m_req_ptr(request) {}
    ~Response();

public:
    void write_body(const char* buf, size_t len);

    void add_header(std::string key, std::string value) {
        m_headers[key] = value;
    }

    void write_head(StatusCode status_code) {
        m_status_code = status_code;
    }

protected:
    void send_response();
    
public:
    typedef std::map<std::string, std::string> header_map;

private:
    struct RespBody {
        char*   ptr;
        size_t  len;
    };

private:
    Conn&       m_conn;
    Request*    m_req_ptr;
    StatusCode  m_status_code;
    RespBody    m_buf;
    header_map  m_headers;
};

