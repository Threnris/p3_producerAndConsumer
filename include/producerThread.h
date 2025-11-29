#ifndef PRODUCER_THREAD_H
#define PRODUCER_THREAD_H

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "media_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;

class ProducerThread {
public:
    ProducerThread(int id, const std::string& input_dir,
                  std::shared_ptr<Channel> channel);

    void run();
    void stop();
    
    int getUploadedCount() const { return uploaded_count_; }
    int getFailedCount() const { return failed_count_; }

private:
    static constexpr size_t CHUNK_SIZE = 64 * 1024; // 64KB chunks

    std::string generateVideoId();
    bool isVideoFile(const std::string& filename);
    std::vector<std::string> getVideoFiles();
    std::string formatFileSize(size_t size);
    bool uploadVideo(const std::string& filepath);
    bool checkQueueStatus();  // BONUS: Check if server queue is full

    int producer_id_;
    std::string input_dir_;
    std::shared_ptr<Channel> channel_;
    std::unique_ptr<mediaupload::MediaUploadService::Stub> stub_;
    
    std::atomic<bool> running_;
    std::atomic<int> uploaded_count_;
    std::atomic<int> failed_count_;
};

#endif // PRODUCER_THREAD_H
