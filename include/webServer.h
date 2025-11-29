#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <thread>
#include "consumerServer.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    typedef int SOCKET;
#endif

class WebServer {
public:
    WebServer(int port, ConsumerServer* consumer_server, 
              const std::string& web_root);
    ~WebServer();

    void start();
    void stop();

private:
    void run();
    void handleRequest(SOCKET client_fd);
    void handleApiRequest(SOCKET client_fd, const std::string& method, 
                         const std::string& path);
    void handleFileRequest(SOCKET client_fd, const std::string& path);
    void sendResponse(SOCKET client_fd, int status_code, 
                     const std::string& content_type,
                     const std::string& body);

    std::string getStatisticsJson();
    std::string getQueueStatusJson();
    std::string getVideosJson();

    int port_;
    ConsumerServer* consumer_server_;
    std::string web_root_;
    bool running_;
    std::thread server_thread_;
};

#endif // WEB_SERVER_H