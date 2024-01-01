#pragma once

#include <thread>
#include <mutex>

// #include "server.h"
#include "router.h"
#include "conn.h"
#include "utils.h"

#define HTTP_SERVER_DAEFUALT_PORT   7789
#define HTTP_SERVER_DAEFUALT_IP     "127.0.0.1"
#define MAX_EPOLL_EVENT_NUM         1024

class HttpServer {
public:
    HttpServer(): m_epoll_fd(-1), m_listen_fd(-1), m_bind_ip(HTTP_SERVER_DAEFUALT_IP), m_bind_port(HTTP_SERVER_DAEFUALT_PORT), m_is_poll(false) {}
    HttpServer(const Router& router): 
        m_router(router), m_epoll_fd(-1), m_bind_ip(HTTP_SERVER_DAEFUALT_IP), m_bind_port(HTTP_SERVER_DAEFUALT_PORT), m_is_poll(false)   {}
    HttpServer(const std::string& ip, uint16_t port): 
        m_epoll_fd(-1), m_listen_fd(-1), m_bind_ip(ip), m_bind_port(port), m_is_poll(false)  {}
    HttpServer(const Router& router, const std::string& ip, uint16_t port): 
         m_router(router), m_epoll_fd(-1), m_listen_fd(-1), m_bind_ip(ip), m_bind_port(port), m_is_poll(false)  {}
    HttpServer(const HttpServer&) = delete;
    HttpServer(HttpServer&&) = delete;
    ~HttpServer();
    
public:
    int init();
    int poll();
    int stop();
    int poll_in_thread();

public:
    Router& router() { return m_router; }

public:
    void set_router(const Router& router) { m_router = router; }

public:
    

protected:
    int init_epoll();
    int init_listen_fd();
    
private:
class IsPoll {
public:
    IsPoll(bool& is_poll): m_is_poll(is_poll) {
        m_is_poll = true;
    }
    ~IsPoll() {
        m_is_poll = false;
    }
private:
    bool& m_is_poll;
};

private:
    bool is_poll();
    void set_poll();
    void unset_poll();
    
private:
    Router              m_router;
    int                 m_epoll_fd;
    int                 m_listen_fd;
    uint16_t            m_bind_port;
    std::string         m_bind_ip;
    ConnManage          m_conn_mgr;
    std::thread         m_thread_poll;
    bool                m_is_poll;
    std::mutex          m_mutex;
};