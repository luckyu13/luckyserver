#include <iostream>
#include <string>
#include <thread>

#include "request.h"
#include "response.h"
#include "http_server.h"

void SayHello(Request& request, Response& response) {
    std::cout << "************SayHello***************" << std::endl;
    char buf[] = "hello world";
    response.write_head(StatusCode::OK);
    response.add_header("Content-type", "text/html");
    response.write_body(buf, sizeof(buf));
}

void PostJsonString(Request& request, Response& response) {
    std::cout << "\n\n" << std::endl;
    std::cout << "method = " << request.method() << "\n"
        << "uri = " << request.uri() << "\n"
        << "proto = " << request.proto() << "\nheaders:\n";
    for (auto it: request.header()) {
        std::cout << it.first << ": " << it.second << "\n";
    }
    if (request.has_header("Content-Length")) {
        std::string payload = std::string(request.payload_ptr(), request.payload_len());
        std::cout << "recv payload: \n" << payload << std::endl;
        response.write_body(payload.c_str(), payload.length());
        response.add_header("Content-Type", "text/plain");
    }
    response.write_head(StatusCode::OK);
}

int main(int argc, char ** argv) {
    HttpServer server;
    server.router().add_handle_func("/say_hello", SayHello);
    server.router().add_handle_func("/postdata", PostJsonString);
    server.init();
    std::thread thread([&server]() {
        server.poll();
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "stop ....." << std::endl;
    server.stop();
    thread.join();
    server.poll_in_thread();
    std::cout << "second = " << server.poll() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "stop ....." << std::endl;
    server.stop();
    
}