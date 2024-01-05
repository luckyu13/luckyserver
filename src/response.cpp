#include <string.h>
#include <iostream>

#include "response.h"

Response::~Response() {}

void Response::write_body(const char* buf, size_t len) {
    if (len == 0) return;
    m_buf.add(buf, len);
}

std::string Response::head() {
    std::stringstream s;
    // write status line
    if (!m_req_ptr || m_req_ptr->proto() == "") {
        s << "HTTP/1.1 ";
    } else {
        s << m_req_ptr->proto() << " ";
    }
    s << std::to_string(m_status_code) << " " << status_message(m_status_code) << "\r\n";
    if (m_buf.len() > 0) {
        bool content_type_flag = false, content_length_flag = false;
        for (auto it: m_headers) {
            std::string key = it.first;
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            if (key == "content-type") {
                content_type_flag = true;
            } else if (key == "content-length") {
                content_length_flag = true;
            }
        } 
        if (!content_type_flag) {
            m_headers["Content-Type"] = "application/octet-stream";
        }
        if (!content_length_flag) {
            m_headers["Content-Length"] = std::to_string(m_buf.len());
        }
    }
    // connection: keep-alive
    if (m_req_ptr->get_header("connection") == "keep-alive") {
        m_headers["Connection"] = "keep-alive";
    }

    // write headers
    for (auto it: m_headers) {
        s << it.first << ": " << it.second << "\r\n";
    }
    // head over
    s << "\r\n";
    return s.str();
}

void Response::reset() {
    m_status_code = StatusCode::OK;
    m_headers.clear();
    m_buf.clear();
    m_buf.resize(IO_BUFFER_DAEFUALT_SIZE);
}
