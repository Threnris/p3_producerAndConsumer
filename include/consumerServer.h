#ifndef CONSUMER_SERVER_H
#define CONSUMER_SERVER_H

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "media_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using mediaupload::MediaUploadService;
using mediaupload::VideoChunk;
using mediaupload::UploadResponse;
using mediaupload::QueueStatusRequest;
using mediaupload::QueueStatusResponse;
using mediaupload::StatisticsRequest;
using mediaupload::StatisticsResponse;

struct UploadTask {
    std::string video_id;
    std::string filename;
    int producer_id;
    std::vector<char> data;
    std::string file_hash;
    size_t total_size;
};

struct VideoMetadata {
    std::string video_id;
    std::string filename;
    std::string file_path;
    std::string file_hash;
    int producer_id;
    int consumer_id;
    size_t file_size;
    std::chrono::system_clock::time_point upload_time;
    bool is_duplicate;
};

class ConsumerServer final : public MediaUploadService::Service {
public:
    ConsumerServer(int num_consumers, int max_queue_size, 
                   const std::string& output_dir);

    // gRPC service methods
    Status UploadVideo(ServerContext* context,
                      ServerReader<VideoChunk>* reader,
                      UploadResponse* response) override;

    Status GetQueueStatus(ServerContext* context,
                         const QueueStatusRequest* request,
                         QueueStatusResponse* response) override;

    Status GetStatistics(ServerContext* context,
                        const StatisticsRequest* request,
                        StatisticsResponse* response) override;

    void start();
    void stop();
    void printStatistics();
    std::vector<VideoMetadata> getVideoMetadata();

private:
    void consumerWorker(int consumer_id);
    void generateThumbnail(const std::string& video_path, 
                          const std::string& video_id);
    std::string calculateHash(const std::vector<char>& data);

    int num_consumers_;
    int max_queue_size_;
    std::string output_dir_;

    std::queue<UploadTask> upload_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    std::vector<std::thread> consumer_threads_;
    bool running_;

    // Metadata tracking
    std::vector<VideoMetadata> video_metadata_;
    std::unordered_map<std::string, std::string> processed_videos_;
    std::unordered_set<std::string> uploaded_hashes_;
    std::mutex metadata_mutex_;
    std::mutex hash_mutex_;

    // Statistics
    int total_received_;
    int total_dropped_;
    int total_duplicates_;
};

#endif // CONSUMER_SERVER_H
