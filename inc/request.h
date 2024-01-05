#pragma once

// #include "server.h"
#include <vector>
#include <map>
#include <string>

#include "io_buf.h"

typedef std::map<std::string, std::string> header_type;

class Request {
public:
    Request(const std::string& method, const std::string& uri, const std::string& proto, 
       const header_type& header, const std::vector<char>& payload):
        m_method(method),
        m_uri(uri),
        m_proto(proto),
        m_headers(header),
        m_payload(m_payload),
        m_status(Request::WAIT_REQUEST_HEAD)
        {}
    Request(): m_payload(IOBuffer(0)), m_status(Request::WAIT_REQUEST_HEAD)  {}
    Request(size_t payload_size): m_payload(payload_size), m_status(Request::WAIT_REQUEST_HEAD) {}

public:
    void set_method(const std::string& method) {  m_method = method; }
    void set_uri(const std::string& uri) {  m_uri = uri; }
    void set_proto(const std::string& proto) {  m_proto = proto; }
    void set_header(const header_type& headers);
    void add_header(const std::string& key, const std::string& val) { m_headers[key] = val; }
    void set_payload(const IOBuffer& payload) {  m_payload = payload; }
    void add_payload_data(const char* data, size_t len);
    bool compele_request() const {
        return !m_method.empty() && !m_uri.empty() && m_payload.capacity() == m_payload.len();
    }
    bool has_header(const std::string& key) {
        if (get_header(key) == "") return false;
        return true;
    }
    std::string get_header(const std::string& key);
    void reset();

public:
    friend class HttpParse;
enum ReqStatus {
    WAIT_REQUEST_HEAD = 1,
    WAIT_REQUEST_PAYLOAD_DATA = 2,
    WAIT_REQUEST_CHUNKEDD_PAYLOAD_DATA = 3,
    COMPLETE_REQUEST = 4,
};

public:
    const std::string method() const { return m_method; }
    const std::string uri() const { return m_uri; }
    const std::string proto() const { return m_proto; }
    const header_type& header() const { return m_headers; }
    const IOBuffer& payload() const { return m_payload; }
    const char* payload_ptr() { return m_payload.data(); }
    size_t payload_len() { return m_payload.len(); }
    Request::ReqStatus status() const { return m_status; }

private:
    std::string m_method;
    std::string m_uri;
    std::string m_proto;
    header_type m_headers;
    IOBuffer    m_payload;
    ReqStatus  m_status;
};
