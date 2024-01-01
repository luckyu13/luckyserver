#include <unistd.h>
#include <iostream>
#include <string.h>
#include <string>

#include "conn.h"
#include "utils.h"

Conn::Conn(int socket_fd, Router* router, ConnManage& mgr): 
    m_socket_fd(socket_fd), m_router(router), m_conn_mgr(mgr) {
#ifdef DEBUG
    std::cout << "New Conn socket = " << m_socket_fd << std::endl;
#endif
}

Conn::Conn(int socket_fd, Router* router, const std::string r_ip, uint16_t r_port, ConnManage& mgr):
    m_socket_fd(socket_fd), m_router(router), m_remote_ip(r_ip), m_remote_port(r_port), m_conn_mgr(mgr) {
#ifdef DEBUG
    std::cout << "New Conn socket = " << m_socket_fd << " addres: [" << m_remote_ip << ": " << m_remote_port << "]" << std::endl;
#endif
}

Conn::~Conn() {
    if (m_socket_fd > 0) {
        close(m_socket_fd);
    }
#ifdef DEBUG
    std::cout << "~Conn socket = " << m_socket_fd << " addres: [" << m_remote_ip << ": " << m_remote_port << "]" << std::endl;
#endif
}

ssize_t Conn::write(const char* buf, size_t len) {
    // m_send_buf.add(buf, len);
    ssize_t w_len = ::write(m_socket_fd, buf, len);
    return w_len;
}

ssize_t Conn::read_data() {
    char buf[1024];
    memset(buf, 0, 1024);
    size_t total_len = 0;
    ssize_t r_len = 0;
    std::cout << "read_data from socket fd" << std::endl;
    while (( r_len = read(m_socket_fd, buf, 1024)) > 0) {
        std::cout << "read_data: " << r_len << std::endl;
        total_len += r_len;
        m_recv_buf.add(buf, r_len);
        memset(buf, 0, 1024);
    }
    if (r_len == 0) {
        std::cout << "remote close." << std::endl;
        m_conn_mgr.del_conn(m_socket_fd);
        return 0;
    }
    // Todo process this: return -1 
    std::cout << "process readed data." << std::endl;
    process_http_recv_data();
    if (r_len == -1 && total_len == 0) return -1;
    return total_len;
}

void Conn::process_http_recv_data() {
    AutoLock lock(m_mutex);
    Request request;
    HttpParseResultCode parse_code = HttpParse::Parse(*this, request);
    switch (parse_code)
    {
    case HttpParseOk: {
        handle_func_type handle_func = m_router->get_handle_func(request.uri());
        if (!handle_func) {
            std::cout << "uri: [" << request.uri() << "] not resgister" << std::endl;
            return;
        }
        Response resp(*this, &request);
        handle_func(request, resp);
        break;
    }
    case HttpParseBadData: {
        std::cout << "bad recv data" << std::endl;
        close(m_socket_fd);
        break;
    }
    case HttpParseDataNotReady: {
        std::cout << "data not ready" << std::endl;
        break;
    }
    case HttpParseExceptErr: {
        std::cout << "except error" << std::endl;
        close(m_socket_fd);
        break;
    }
    default: {
        std::cout << "undifine code" << std::endl;
        break;
    }
    }
    if (parse_code == HttpParseOk && request.get_header("connection") != "keep-alive") {
        std::cout << "connect is not keep-alive, colse socket_fd ..." << std::endl;
        m_conn_mgr.del_conn(m_socket_fd);
    }
}

void ConnManage::add_conn(int client_fd, Router* router, ConnManage& mgr, const std::string& r_ip, uint16_t r_port) {
    auto pre = get_conn(client_fd);
    if (pre != nullptr) {
        delete pre;
        pre = nullptr;
    }
    Conn* conn = new_conn(client_fd, router, mgr, r_ip, r_port);
    m_conn_map[client_fd] = conn;
}

void ConnManage::del_conn(int client_fd) {
    auto conn = get_conn(client_fd);
    if (conn != nullptr) {
        delete conn;
        m_conn_map.erase(client_fd);
    }
}

void ConnManage::del_conn(Conn* conn) { 
    del_conn(conn->m_socket_fd); 
}

Conn* ConnManage::get_conn(int client_fd) {
    auto it = m_conn_map.find(client_fd);
    if (it == m_conn_map.end()) return nullptr;
    return it->second;
}

void ConnManage::clear() {
    for (auto it: m_conn_map) {
        delete it.second;
        m_conn_map.erase(it.first);
    }
}


Conn* ConnManage::new_conn(int client_fd, Router* router,  ConnManage& mgr, const std::string& r_ip, uint16_t r_port) {
    Conn *conn = new Conn(client_fd, router, r_ip, r_port, mgr);
    return conn;
}

ConnManage::~ConnManage() {
    clear();
 }