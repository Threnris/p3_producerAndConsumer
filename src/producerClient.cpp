#include "include/producerClient.h"
#include <iostream>
#include <csignal>

ProducerClient::ProducerClient(int num_producers, const std::string& base_input_dir,
                              const std::string& server_address)
    : num_producers_(num_producers), base_input_dir_(base_input_dir),
      server_address_(server_address) {
    
    channel_ = grpc::CreateChannel(server_address_, 
                                   grpc::InsecureChannelCredentials());
}

void ProducerClient::start() {
    std::cout << " Media Upload Producer Client" << std::endl;
    std::cout << "Producers:       " << num_producers_ << std::endl;
    std::cout << "Server:          " << server_address_ << std::endl;
    std::cout << "Input directory: " << base_input_dir_ << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < num_producers_; i++) {
        std::string input_dir = base_input_dir_ + "/producer_" + std::to_string(i + 1);
        
        auto producer = std::make_unique<ProducerThread>(i + 1, input_dir, channel_);
        auto* producer_ptr = producer.get();
        producers_.push_back(std::move(producer));
        
        threads_.emplace_back([producer_ptr]() { producer_ptr->run(); });
    }

    std::cout << "All producer threads started. Press Ctrl+C to stop...\n" << std::endl;

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    printStatistics();
}

void ProducerClient::stop() {
    for (auto& producer : producers_) {
        producer->stop();
    }
}

void ProducerClient::printStatistics() {
    std::cout << "Final Stats" << std::endl;
    
    
    int total_uploaded = 0;
    int total_failed = 0;
    
    for (const auto& producer : producers_) {
        total_uploaded += producer->getUploadedCount();
        total_failed += producer->getFailedCount();
    }
    
    std::cout << "Total uploaded:  " << total_uploaded << std::endl;
    std::cout << "Total failed:    " << total_failed << std::endl;
    
    if (total_uploaded + total_failed > 0) {
        double success_rate = (total_uploaded * 100.0) / (total_uploaded + total_failed);
        std::cout << "Success rate:    " << std::fixed << std::setprecision(2) 
                  << success_rate << "%" << std::endl;
    }
    
    
}

