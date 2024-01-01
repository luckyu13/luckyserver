#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "request.h"
#include "response.h"

class Response;
class Request;

typedef void(*handle_func_type)(Request& request, Response& response);

class Router {
public:
    Router() {};
    Router(const Router& o);
    Router(Router&& o);
    ~Router();

public:
    Router& operator=(const Router& o);

public:
    void add_handle_func(const std::string& path, handle_func_type func);
    handle_func_type get_handle_func(const std::string& path);

public:
    typedef std::map<std::string, handle_func_type> router_map_type;

private:
    router_map_type     m_path_func_map;
};