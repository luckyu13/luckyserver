#include <iostream>

#include "http_parse.h"
#include "utils.h"

static bool parse_request_line(const std::string& line, Request& request) {
    std::string tmp;
    int idx = 0;
    for (; idx < line.size(); ++idx) {
        if (line[idx] == ' ') {
            break;
        }
        tmp.push_back(line[idx]);
    }
    request.set_method(tmp);
    idx++; tmp.clear();
    for (; idx < line.size(); idx++) {
        if (line[idx] == ' ') {
            break;
        }
        tmp.push_back(line[idx]);
    }
    request.set_uri(tmp);
    tmp.clear(); idx++;
    request.set_proto(std::string(line.begin() + idx, line.end()));
    if (request.method().length() == 0 || request.uri().length() == 0) {
        return false;
    }
    return true;
}

static std::string trim(const std::string& line, size_t start_pos, size_t end_ch_pos) {
    while (start_pos < end_ch_pos) {
        if (line[start_pos] == ' ' || line[start_pos] == '\r' || line[start_pos] == '\n') {
            start_pos++;
        } else if (line[end_ch_pos] == ' ' || line[end_ch_pos] == '\r' || line[end_ch_pos] == '\n') {
            end_ch_pos--;
        } else {
            break;
        }
    }
    if (start_pos > end_ch_pos) return "";
    return std::string(line.begin() + start_pos, line.begin() + end_ch_pos + 1);
}

static bool parse_header_line(const std::string& line, Request& request) {
    size_t colon_pos = 0;
    for (; colon_pos < line.length(); ++colon_pos) {
        if (line[colon_pos] == ':') break;
    } 
    std::string key = trim(line, 0, colon_pos - 1);
    std::string val = trim(line, colon_pos+1, line.length() -1);
    std::cout << key << ": " << val << std::endl;
    if (key.empty() || val.empty()) {
        return false;
    }
    request.add_header(key, val);
    return true;
}

HttpParseResultCode HttpParse::parse_payload(Conn& conn, Request& request) {
    std::string chunk = request.get_header("transfer-encoding");
    std::string payload_len_str = request.get_header("content-length");
    if (chunk.length() != 0 && payload_len_str.length() != 0) return HttpBadRequest;
    if (chunk.length() == 0 && payload_len_str.length() == 0) {
        conn.m_recv_buf.del(conn.m_recv_buf.len());
        request.m_status = Request::ReqStatus::COMPLETE_REQUEST;
        return HttpParseOk;
    }
    // Content-Length
    if (payload_len_str != "") {
        request.m_status = Request::ReqStatus::WAIT_REQUEST_PAYLOAD_DATA;
        return read_payload(conn, request);
    } else {
        request.m_status = Request::ReqStatus::WAIT_REQUEST_CHUNKEDD_PAYLOAD_DATA;
        return read_chunked_payload(conn, request);
    }
}

HttpParseResultCode HttpParse::read_payload(Conn& conn, Request& request) {
    std::string payload_len_str = request.get_header("content-length");
    if (payload_len_str.length() == 0) {
        return HttpParseExceptErr;
    }
    size_t payload_len = std::stoul(request.get_header("content-length"));
    if (payload_len > conn.m_recv_buf.len()) {
        request.m_status = Request::ReqStatus::WAIT_REQUEST_PAYLOAD_DATA;
        return HttpParseDataNotReady;
    }
    request.set_payload(IOBuffer(payload_len));
    request.add_payload_data(conn.m_recv_buf.data(), payload_len);
    conn.m_recv_buf.del(conn.m_recv_buf.len());
    request.m_status = Request::ReqStatus::COMPLETE_REQUEST;
    return HttpParseOk;
}

HttpParseResultCode HttpParse::read_chunked_payload(Conn& conn, Request& request) {
    std::string chunk = request.get_header("transfer-encoding");
    if (chunk.length() == 0) return HttpParseExceptErr;
    if (!str_eq(chunk, "chunked")) return HttpBadRequest;
    size_t pos = 0;
    size_t chuned_data_len = INT16_MAX;
    std::string chuned_data_len_str;
    while (true) {
        chuned_data_len_str = get_line(conn.m_recv_buf, pos);
        std::cout << "chuned_data_len_str = " << chuned_data_len_str << std::endl;
        if (chuned_data_len_str.length() == 0) break;
        chuned_data_len = std::stoul(chuned_data_len_str, 0, 16);
        if (chuned_data_len == 0) {
            conn.m_recv_buf.del(conn.m_recv_buf.len());
            request.m_status = Request::ReqStatus::COMPLETE_REQUEST;
            return HttpParseOk;
            break;
        }
         if (chuned_data_len > conn.m_recv_buf.len()) {
            conn.m_recv_buf.del(pos);
            request.m_status = Request::ReqStatus::WAIT_REQUEST_CHUNKEDD_PAYLOAD_DATA;
            return HttpParseDataNotReady;
         } else {
            request.add_payload_data(conn.m_recv_buf.data() + pos, chuned_data_len);
            pos += chuned_data_len + 2;
         }
    }
    conn.m_recv_buf.del(pos);
    request.m_status = Request::ReqStatus::WAIT_REQUEST_CHUNKEDD_PAYLOAD_DATA;
    return HttpParseDataNotReady;
}

HttpParseResultCode HttpParse::parse(Conn& conn, Request& request) {
#ifdef DEBUG
    std::cout << "parse..." << std::endl;
#endif
    switch (request.m_status) {
    case Request::ReqStatus::WAIT_REQUEST_HEAD: {
        // parse request head
        conn.m_request.reset();
        if (!compelet_request_head(conn)) {
            return HttpParseDataNotReady;
        }
#ifdef DEBUG
        std::cout << "compele request head" << std::endl;
#endif
        size_t pos = 0;
        std::string request_line = get_line(conn.m_recv_buf, pos);
#ifdef DEBUG
        std::cout << "request_line = " << request_line << std::endl;
#endif
        // GET / HTTP/1.1
        if (!parse_request_line(request_line, request)) {
            return HttpParseBadData;
        }
#ifdef DEBUG
        std::cout << "method=" << request.method() << '\n'
            << "uri=" << request.uri() << '\n'
            << "proto=" << request.proto() << std::endl;
#endif
        // Accept-Language: zh-cn
        // Accept-Encoding: gzip, deflate
        // 
        std::string header_line;
        while (true) {
            header_line = get_line(conn.m_recv_buf, pos);
            if (header_line.length() == 0) {
                break;
            }
            if (!parse_header_line(header_line, request)) {
                return HttpParseBadData;
            }
        }
        conn.m_recv_buf.del(pos);
        return parse_payload(conn, request);
    }
    case Request::ReqStatus::WAIT_REQUEST_CHUNKEDD_PAYLOAD_DATA: {
        return read_chunked_payload(conn, request);
    }
    case Request::WAIT_REQUEST_PAYLOAD_DATA: {
        return read_payload(conn, request);
    }
    default:
        return HttpParseExceptErr;
    }
}

bool HttpParse::compelet_request_head(Conn& conn) {
    std::cout << "compelet_request_head" << std::endl;
    auto& r_buf = conn.m_recv_buf;
    if (r_buf.len() < 4) return false;
    for (int i=3; i<r_buf.len(); ++i) {
        if (r_buf.data()[i-3] == '\r' && 
            r_buf.data()[i-2] == '\n' && 
            r_buf.data()[i-1] == '\r' && 
            r_buf.data()[i] == '\n') {
            return true;
        }
    }
    return false;
}

std::string HttpParse::get_line(IOBuffer& buf, size_t& pos) {
    size_t p = pos;
    for (p=pos; p < buf.len() - 1; ++p) {
        if (buf.data()[p] == '\r' && buf.data()[p+1] == '\n') {
            break;
        }
    }
    if (p == buf.len() - 1) {
        return "";
    }
    while (pos < p) {
        if (buf.data()[pos] < 32 || buf.data()[pos] == 127) {
            pos++;
        } else {
            break;
        }
    }

    size_t pr = p-1;
    while (pr > pos) {
        if (buf.data()[pr] < 32 || buf.data()[pr] == 127) {
            pr--;
        } else {
            break;
        }
    }

    std::string line(buf.data() + pos, pr - pos + 1);
    pos = p + 2;
    return trim(line, 0, line.length() -1);
}