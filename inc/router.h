#pragma once

#include <string>
#include <map>
#include <algorithm>
#include <mutex>
#include <functional>

#include "request.h"
#include "response.h"
#include "utils.h"

class Response;
class Request;

using handle_func_type = std::function<void(Request&, Response&)>;
// typedef void(*handle_func_type)(Request& request, Response& response);

class Router {
public:
    Router() {};
    Router(const Router& o);
    Router(Router&& o);
    ~Router();

public:
    Router& operator=(const Router& o);

public:
    void add_handle_func(const std::string& path, const std::string& method, handle_func_type func);
    handle_func_type get_handle_func(const std::string& path, const std::string& method);

public:
    typedef std::map<std::string, std::map<std::string, handle_func_type> > router_map_type;

private:
    router_map_type     m_path_func_map;
    std::mutex          m_mutex;
};