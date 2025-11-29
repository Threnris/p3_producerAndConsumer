#include "include/webServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define closesocket close
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

WebServer::WebServer(int port, ConsumerServer* consumer_server, 
                     const std::string& web_root)
    : port_(port), consumer_server_(consumer_server), 
      web_root_(web_root), running_(false) {
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }
#endif
}

WebServer::~WebServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void WebServer::start() {
    running_ = true;
    server_thread_ = std::thread([this]() { run(); });
    std::cout << "Web server started on http://localhost:" << port_ << std::endl;
}

void WebServer::stop() {
    running_ = false;
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

void WebServer::run() {
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_fd);
        return;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_fd);
        return;
    }

    while (running_) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            continue;
        }

        // Handle request in new thread
        std::thread([this, client_fd]() {
            handleRequest(client_fd);
            closesocket(client_fd);
        }).detach();
    }

    closesocket(server_fd);
}

void WebServer::handleRequest(SOCKET client_fd) {
    char buffer[4096] = {0};
    recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    std::string request(buffer);
    std::istringstream request_stream(request);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    std::cout << "[WEB] " << method << " " << path << std::endl;

    if (path.find("/api/") == 0) {
        handleApiRequest(client_fd, method, path);
    } else {
        handleFileRequest(client_fd, path);
    }
}

void WebServer::handleApiRequest(SOCKET client_fd, const std::string& method, 
                                const std::string& path) {
    std::string response_body;
    std::string content_type = "application/json";

    if (path == "/api/statistics") {
        response_body = getStatisticsJson();
    } else if (path == "/api/queue") {
        response_body = getQueueStatusJson();
    } else if (path == "/api/videos") {
        response_body = getVideosJson();
    } else {
        response_body = R"({"error": "Not found"})";
        sendResponse(client_fd, 404, content_type, response_body);
        return;
    }

    sendResponse(client_fd, 200, content_type, response_body);
}

void WebServer::handleFileRequest(SOCKET client_fd, const std::string& path) {
    std::string file_path = web_root_;
    if (path == "/") {
        file_path += "/index.html";
    } else {
        file_path += path;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::string not_found = "<html><body><h1>404 Not Found</h1></body></html>";
        sendResponse(client_fd, 404, "text/html", not_found);
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::string content_type = "text/html";
    if (file_path.ends_with(".css")) content_type = "text/css";
    else if (file_path.ends_with(".js")) content_type = "application/javascript";
    else if (file_path.ends_with(".mp4")) content_type = "video/mp4";
    else if (file_path.ends_with(".jpg")) content_type = "image/jpeg";
    else if (file_path.ends_with(".png")) content_type = "image/png";

    sendResponse(client_fd, 200, content_type, content);
}

void WebServer::sendResponse(SOCKET client_fd, int status_code, 
                            const std::string& content_type,
                            const std::string& body) {
    std::string status_text = (status_code == 200) ? "OK" : "Not Found";
    
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    std::string response_str = response.str();
    send(client_fd, response_str.c_str(), (int)response_str.length(), 0);
}

std::string WebServer::getStatisticsJson() {
    // Mock implementation - replace with actual gRPC call
    std::ostringstream json;
    json << "{"
         << "\"total_received\": 0,"
         << "\"total_processed\": 0,"
         << "\"total_dropped\": 0,"
         << "\"total_duplicates\": 0"
         << "}";
    return json.str();
}

std::string WebServer::getQueueStatusJson() {
    // Mock implementation - replace with actual gRPC call
    std::ostringstream json;
    json << "{"
         << "\"current_size\": 0,"
         << "\"max_size\": 10,"
         << "\"is_full\": false,"
         << "\"available_slots\": 10"
         << "}";
    return json.str();
}

std::string WebServer::getVideosJson() {
    auto metadata = consumer_server_->getVideoMetadata();
    
    std::ostringstream json;
    json << "[";
    
    for (size_t i = 0; i < metadata.size(); i++) {
        const auto& video = metadata[i];
        auto timestamp = std::chrono::system_clock::to_time_t(video.upload_time);
        
        json << "{"
             << "\"video_id\":\"" << video.video_id << "\","
             << "\"filename\":\"" << video.filename << "\","
             << "\"file_path\":\"" << video.file_path << "\","
             << "\"producer_id\":" << video.producer_id << ","
             << "\"file_size\":" << video.file_size << ","
             << "\"upload_timestamp\":" << timestamp << ","
             << "\"is_duplicate\":" << (video.is_duplicate ? "true" : "false")
             << "}";
        
        if (i < metadata.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}
