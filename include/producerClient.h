#ifndef PRODUCER_CLIENT_H
#define PRODUCER_CLIENT_H

#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "producerThread.h"

using grpc::Channel;

class ProducerClient {
public:
    ProducerClient(int num_producers, const std::string& base_input_dir,
                  const std::string& server_address);

    void start();
    void stop();
    void printStatistics();

private:
    int num_producers_;
    std::string base_input_dir_;
    std::string server_address_;
    
    std::shared_ptr<Channel> channel_;
    std::vector<std::unique_ptr<ProducerThread>> producers_;
    std::vector<std::thread> threads_;
};

#endif // PRODUCER_CLIENT_H
