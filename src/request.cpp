#include <algorithm>

#include "request.h"

void Request::set_header(const header_type& headers) {  
    m_headers.clear();
    for (auto it: headers) {
        m_headers.insert(it);
    }
}

void Request::add_payload_data(const char* data, size_t len) {
    if (m_payload.len() + len > m_payload.capacity()) {
        m_payload.add(data, m_payload.capacity() - m_payload.len());
    } else {
        m_payload.add(data, len);
    }
}

std::string Request::get_header(const std::string& key) {
    std::string _key(key);
    std::transform(_key.begin(), _key.end(), _key.begin(), ::tolower);
    for (auto it: m_headers) {
        std::string t_key(it.first);
        std::transform(t_key.begin(), t_key.end(), t_key.begin(), ::tolower);
        if (t_key == _key) return it.second;
    }
    return "";
}

void Request::reset() {
    m_method.clear();
    m_proto.clear();
    m_uri.clear();
    m_headers.clear();
    m_payload.clear();
    m_payload.resize(IO_BUFFER_DAEFUALT_SIZE);
    m_status = WAIT_REQUEST_HEAD;
}