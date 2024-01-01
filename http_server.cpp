#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "http_server.h"

HttpServer::~HttpServer() {
    if (m_listen_fd != -1) close(m_listen_fd);
    if (m_epoll_fd != -1) close(m_epoll_fd);
}

int HttpServer::init() {
    int ret = init_listen_fd();
    if (ret != 0) return ret;
    ret = init_epoll();
    if (ret != 0) return ret;
    return 0;
}

int HttpServer::poll() {
    if (m_listen_fd == -1) {
        int ret = init();
        if (ret != 0) {
            std::cout << "init error" << std::endl;
            return ret;
        }
    }
    epoll_event events[MAX_EPOLL_EVENT_NUM];
    // 事件循环，接收到的事件会被放到 events 中
    bool stop_flag = false, err_flag = false;
    int ret = 0;
    std::cout << "start poll." << std::endl;
    while (true) {
        int n = epoll_wait(m_epoll_fd, events, MAX_EPOLL_EVENT_NUM, -1);
        if (n == -1) {
            std::cerr << "m_epoll_fd = " << m_epoll_fd << ", epoll_wait error: " << strerror(errno) << std::endl;
            err_flag = true;
            break;
        } 

        for (int i=0; i<n; ++i) {
            if (events[i].data.fd == m_listen_fd) {
                if (events[i].events & EPOLLHUP) {
                    std::cout << "listen stop..." << std::endl;
                    stop_flag = true;
                    break;
                } else if (events[i].events & EPOLLERR) {
                    std::cerr << "listen socket error" << std::endl;
                    err_flag = true;
                    break;
                } else {
                    sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int client_fd =
                    accept(m_listen_fd, (sockaddr*)&client_addr, &client_addr_len);
                    if (client_fd == -1) {
                        std::cerr << "accept error" << std::endl;
                        continue;
                    }
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                    std::string r_ip = inet_ntoa(client_addr.sin_addr);
                    uint16_t r_port = ntohs(client_addr.sin_port);
                    epoll_event event;
                    event.data.fd = client_fd;
                    event.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
                    ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                    if (ret == -1) {
                        std::cerr << "epoll_ctl error when add client: " << strerror(errno)
                                    << std::endl;
                            close(client_fd);
                            continue;
                    }
                    std::cout << "client ["<< client_fd << "] connected from ["
                        << r_ip << ":"
                        << r_port << "]" << std::endl;
                    m_conn_mgr.add_conn(client_fd, &m_router, m_conn_mgr, r_ip, r_port);
                }
            } else {
                int client_fd = events[i].data.fd;
                std::cout << "new event, fd = " << client_fd << std::endl;
                Conn* conn = m_conn_mgr.get_conn(client_fd);
                if (conn == nullptr) continue;
                if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR) {
                    m_conn_mgr.del_conn(conn);
                } else {
                    conn->read_data();
                }
            }
        }
        if (stop_flag || err_flag) break;
    }
    m_conn_mgr.clear();
    close(m_epoll_fd);
    m_listen_fd = -1;
    m_epoll_fd = -1;
    if (err_flag) return -1;
    return 0;
}

int HttpServer::stop() {
    if (m_listen_fd != -1) {
        close(m_listen_fd);
    }
    return 0;
}

int HttpServer::init_epoll() {
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd == -1) {
        std::cerr << "epoll_create error" << std::endl;
        if (m_listen_fd != -1) {
            close(m_listen_fd);
        }
        return -1;
    }
    std::cout << "init_epoll: fd = " << m_epoll_fd << std::endl;
    epoll_event event;
    event.data.fd = m_listen_fd;
    event.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
    int ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &event);
    if (ret == -1) {
        std::cerr << "epoll_ctl error when initialize: " << strerror(errno)
                << std::endl;
        if (m_listen_fd != -1) {
            close(m_listen_fd);
        }
        return -1;
    }
    return 0;
}

int HttpServer::init_listen_fd() {
    // init addr
    in_addr_t addr = inet_addr(m_bind_ip.c_str());
    if (addr == INADDR_NONE) {
        std::cerr << "invalid address" << std::endl;
        return -1;
    }
    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(m_bind_port);
    addr_in.sin_addr.s_addr = addr;

    // init m_socket_fd, do bind and listen
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd == -1) {
        std::cerr << "socket error" << std::endl;
        return -1;
    }
    int flags = fcntl(m_listen_fd, F_GETFL, 0);
    fcntl(m_listen_fd, F_SETFL, flags | O_NONBLOCK);
    int opt = 1;
    setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int ret;
    ret = bind(m_listen_fd, (sockaddr*)&addr_in, sizeof(addr_in));
    if (ret == -1) {
        std::cerr << "bind error: " << strerror(errno) << std::endl;
        return -1;
    }
    ret = listen(m_listen_fd, SOMAXCONN);
    if (ret == -1) {
        std::cerr << "listen error" << std::endl;
        return -1;
    }

    std::cout << "server is listening on " << m_bind_ip << ": "
                << m_bind_port << std::endl;
    return 0;
}
