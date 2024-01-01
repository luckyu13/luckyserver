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

void Router::add_handle_func(const std::string& path, handle_func_type func) {
    m_path_func_map[path] = func;
}

handle_func_type Router::get_handle_func(const std::string& path) {
    auto it = m_path_func_map.find(path);
    if (it == m_path_func_map.end()) {
        return nullptr;
    }
    return it->second;
}