#include "router.h"

Router& Router::operator=(const Router& o) {
    for (auto it: o.m_path_func_map) {
        m_path_func_map.insert(it);
    }
}

Router::Router(const Router& o) {
    for (auto it: o.m_path_func_map) {
        m_path_func_map.insert(it);
    }
}

Router::Router(Router&& o) {
    m_path_func_map = std::move(o.m_path_func_map);
}

Router::~Router() {
    m_path_func_map.clear();
}

void Router::add_handle_func(const std::string& path, const std::string& method, handle_func_type func) {
    AutoLock lock(m_mutex);
    std::string _method = method;
    std::transform(_method.begin(), _method.end(), _method.begin(), ::tolower);
    m_path_func_map[path][_method] = func;
}

handle_func_type Router::get_handle_func(const std::string& path, const std::string& method) {
    AutoLock lock(m_mutex);
    auto it = m_path_func_map.find(path);
    if (it == m_path_func_map.end()) {
        return nullptr;
    }
    std::string _method = method;
    std::transform(_method.begin(), _method.end(), _method.begin(), ::tolower);
    auto it_func = it->second.find(_method);
    if (it_func == it->second.end()) {
        return nullptr;
    }
    return it_func->second;
}