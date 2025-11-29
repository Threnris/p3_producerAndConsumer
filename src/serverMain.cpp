#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <grpcpp/grpcpp.h>
#include "include/consumerServer.h"
#include "include/webServer.h"

using grpc::Server;
using grpc::ServerBuilder;

std::unique_ptr<Server> grpc_server;
std::unique_ptr<ConsumerServer> consumer_service;
std::unique_ptr<WebServer> web_server;

void signalHandler(int signum) {
    std::cout << "\n\nShutting down gracefully..." << std::endl;
    
    if (consumer_service) {
        consumer_service->stop();
    }
    
    if (web_server) {
        web_server->stop();
    }
    
    if (grpc_server) {
        grpc_server->Shutdown();
    }
    
    exit(0);
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -c <consumers> -q <queue_size> [-p <port>] [-w <web_port>] [-o <output_dir>]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -c <consumers>    Number of consumer threads (default: 4)\n";
    std::cout << "  -q <queue_size>   Maximum queue size/capacity (default: 10)\n";
    std::cout << "  -p <port>         gRPC server port (default: 50051)\n";
    std::cout << "  -w <web_port>     Web GUI port (default: 8080)\n";
    std::cout << "  -o <output_dir>   Output directory for videos (default: ./uploaded_videos)\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " -c 4 -q 10\n";
    std::cout << "  " << program_name << " -c 8 -q 20 -p 50051 -w 8080\n";
}

int main(int argc, char** argv) {
    // Default parameters
    int num_consumers = 4;
    int max_queue_size = 10;
    int grpc_port = 50051;
    int web_port = 8080;
    std::string output_dir = "./uploaded_videos";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-c" && i + 1 < argc) {
            num_consumers = std::stoi(argv[++i]);
        } else if (arg == "-q" && i + 1 < argc) {
            max_queue_size = std::stoi(argv[++i]);
        } else if (arg == "-p" && i + 1 < argc) {
            grpc_port = std::stoi(argv[++i]);
        } else if (arg == "-w" && i + 1 < argc) {
            web_port = std::stoi(argv[++i]);
        } else if (arg == "-o" && i + 1 < argc) {
            output_dir = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate parameters
    if (num_consumers < 1 || num_consumers > 100) {
        std::cerr << "Error: Number of consumers must be between 1 and 100" << std::endl;
        return 1;
    }

    if (max_queue_size < 1 || max_queue_size > 1000) {
        std::cerr << "Error: Queue size must be between 1 and 1000" << std::endl;
        return 1;
    }

    // Set up signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Media Upload Service - Consumer Server       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n📊 Configuration:" << std::endl;
    std::cout << "  Consumer threads: " << num_consumers << std::endl;
    std::cout << "  Queue capacity:   " << max_queue_size << " (Leaky bucket)" << std::endl;
    std::cout << "  gRPC port:        " << grpc_port << std::endl;
    std::cout << "  Web GUI port:     " << web_port << std::endl;
    std::cout << "  Output directory: " << output_dir << std::endl;
    std::cout << "\n✨ Features enabled:" << std::endl;
    std::cout << "  ✓ Queue management (leaky bucket)" << std::endl;
    std::cout << "  ✓ Duplicate detection (SHA-256 hashing)" << std::endl;
    std::cout << "  ✓ Queue status API for producers" << std::endl;
    std::cout << "  ✓ Web-based GUI with video preview" << std::endl;
    std::cout << "  ✓ Real-time statistics" << std::endl;
    std::cout << std::endl;

    // Create consumer service
    consumer_service = std::make_unique<ConsumerServer>(
        num_consumers, max_queue_size, output_dir
    );

    // Start consumer workers
    consumer_service->start();

    // Build and start gRPC server
    std::string server_address = "0.0.0.0:" + std::to_string(grpc_port);
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(consumer_service.get());
    
    grpc_server = builder.BuildAndStart();
    std::cout << "🚀 gRPC Server listening on " << server_address << std::endl;

    // Start web server for GUI
    web_server = std::make_unique<WebServer>(web_port, consumer_service.get(), "./web");
    web_server->start();
    std::cout << "🌐 Web GUI available at http://localhost:" << web_port << std::endl;

    std::cout << "\n✅ Server is ready to accept uploads!" << std::endl;
    std::cout << "   Press Ctrl+C to stop the server\n" << std::endl;

    // Wait for server shutdown
    grpc_server->Wait();

    return 0;
}
