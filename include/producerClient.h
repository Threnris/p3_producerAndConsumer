#pragma once
#include "producerThread.h"
#include <vector>
#include <thread>
#include <memory>

class ProducerClient {
private:
    int num_producers_;
    std::string base_input_dir_;
    std::string server_address_;
    std::shared_ptr<Channel> channel_;
    
    std::vector<std::unique_ptr<ProducerThread>> producers_;
    std::vector<std::thread> threads_;

public:
    ProducerClient(int num_producers, const std::string& base_input_dir,
                  const std::string& server_address);
    
    void start();
    void stop();
    void printStatistics();
};