#include <iostream>
#include <string>
#include <csignal>
#include "include/producerClient.h"

std::unique_ptr<ProducerClient> producer_client;

void signalHandler(int signum) {
    std::cout << "\n\nShutting down producers..." << std::endl;
    
    if (producer_client) {
        producer_client->stop();
    }
    
    exit(0);
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -p <producers> [-s <server>] [-i <input_dir>]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -p <producers>    Number of producer threads (required)\n";
    std::cout << "  -s <server>       Server address (default: localhost:50051)\n";
    std::cout << "  -i <input_dir>    Base input directory (default: ./video_files)\n";
    std::cout << "\nInput Directory Structure:\n";
    std::cout << "  The base directory should contain subdirectories for each producer:\n";
    std::cout << "    <input_dir>/producer_1/\n";
    std::cout << "    <input_dir>/producer_2/\n";
    std::cout << "    ...\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " -p 3\n";
    std::cout << "  " << program_name << " -p 5 -s 192.168.1.100:50051 -i /path/to/videos\n";
}

int main(int argc, char** argv) {
    int num_producers = 0;
    std::string server_address = "localhost:50051";
    std::string base_input_dir = "./video_files";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-p" && i + 1 < argc) {
            num_producers = std::stoi(argv[++i]);
        } else if (arg == "-s" && i + 1 < argc) {
            server_address = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            base_input_dir = argv[++i];
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
    if (num_producers < 1) {
        std::cerr << "Error: Number of producers must be specified and at least 1" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    if (num_producers > 50) {
        std::cerr << "Error: Number of producers cannot exceed 50" << std::endl;
        return 1;
    }

    // Set up signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Media Upload Service - Producer Client       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // Create and start producer client
    producer_client = std::make_unique<ProducerClient>(
        num_producers, base_input_dir, server_address
    );

    producer_client->start();

    return 0;
}
