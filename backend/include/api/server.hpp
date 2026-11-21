#pragma once

#include <string>

namespace recongraph::api {

class Server {
public:
    explicit Server(int port = 8080);
    ~Server() = default;

    // Non-copyable
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void start();
    void stop();

    int port() const { return port_; }

private:
    int port_;
    bool running_ = false;

    void setup_routes();
};

} // namespace recongraph::api
