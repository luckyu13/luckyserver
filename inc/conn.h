#pragma once
// #ifndef __HTTP_REQUEST_H__
// #define __HTTP_REQUEST_H__

// #include "server.h"
#include <mutex>
#include <string>

#include "router.h"
#include "http_parse.h"
#include "request.h"
#include "response.h"

class Router;
class Conn;

class ConnManage {
public:
    ~ConnManage();
public:
    void add_conn(int client_fd, Router* router, ConnManage& mgr, const std::string& r_ip, uint16_t r_port);
    void del_conn(int client_fd);
    void del_conn(Conn* conn);
    Conn* get_conn(int client_fd);
    void clear();

protected:
    Conn* new_conn(int client_fd, Router* router,  ConnManage& mgr, const std::string& r_ip, uint16_t r_port);

private: 
    typedef std::map<int, Conn*> conn_mgr_type;
    conn_mgr_type       m_conn_map;
};

class Conn {
public:
    Conn() = delete;
    Conn(int socket_fd, Router* router, ConnManage& mgr);
    Conn(int socket_fd, Router* router, const std::string r_ip, uint16_t r_port, ConnManage& mgr);
    Conn(const Conn& o) = delete;
    Conn(Conn&&) = delete;
    ~Conn();

public:
    ssize_t write(const char* buf, size_t len);
    ssize_t read_data();

public:
    friend class HttpParse;
    friend class ConnManage;

protected:
    void process_http_recv_data();

private:
    int         m_socket_fd;
    Router*     m_router;
    IOBuffer    m_recv_buf;
    std::mutex  m_mutex;
    std::string m_remote_ip;
    uint16_t    m_remote_port;
    ConnManage& m_conn_mgr;
};


// #endif // __HTTP_REQUEST_H__