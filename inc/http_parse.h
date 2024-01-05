#pragma once

#include "io_buf.h"
#include "conn.h"
#include "request.h"
#include "utils.h"

class Conn;
enum HttpParseResultCode {
    HttpParseOk = 0,
    HttpParseDataNotReady = 1,
    HttpParseBadData = 2,
    HttpParseExceptErr = 3,
    HttpBadRequest = 4,
};

class HttpParse {
public:
    static HttpParseResultCode parse(Conn& conn,  Request& request);

private:
    static bool compelet_request_head(Conn& conn);
    static std::string get_line(IOBuffer& buf, size_t& pos);
    static HttpParseResultCode parse_payload(Conn& conn, Request& request);
    static HttpParseResultCode read_payload(Conn& conn, Request& request);
    static HttpParseResultCode read_chunked_payload(Conn& conn, Request& request);
};
