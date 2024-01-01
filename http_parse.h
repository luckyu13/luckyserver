#pragma once

#include "io_buf.h"
#include "conn.h"
#include "request.h"

class Conn;
enum HttpParseResultCode {
    HttpParseOk = 0,
    HttpParseDataNotReady = 1,
    HttpParseBadData = 2,
    HttpParseExceptErr = 3,
};

class HttpParse {
public:
    static HttpParseResultCode Parse(Conn& conn,  Request& request);

private:
    static bool compelet_request_head(Conn& conn);
    static std::string get_line(IOBuffer& buf, size_t& pos);
};
