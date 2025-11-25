#pragma once

#include <grpcpp/grpcpp.h>
#include "media_service.grpc.pb.h"
#include <string>
#include <atomic>
#include <memory>
#include <vector>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;

class ProducerThread {
private:
    int producer_id_;
    std::string input_dir_;
    std::shared_ptr<Channel> channel_;
    std::unique_ptr<mediaupload::MediaUploadService::Stub> stub_;
    std::atomic<bool> running_;
    std::atomic<int> uploaded_count_;
    std::atomic<int> failed_count_;
    
    static const size_t CHUNK_SIZE = 256 * 1024; // 256KB

    std::string generateVideoId();
    std::vector<std::string> getVideoFiles();
    bool isVideoFile(const std::string& filename);
    bool uploadVideo(const std::string& filepath);
    std::string formatFileSize(size_t size);

public:
    ProducerThread(int id, const std::string& input_dir,std::shared_ptr<Channel> channel);
    void run();
    void stop();
    int getUploadedCount() const { return uploaded_count_; }
    int getFailedCount() const { return failed_count_; }
};