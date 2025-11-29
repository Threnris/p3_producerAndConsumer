#include "include/consumerServer.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

namespace fs = std::filesystem;

ConsumerServer::ConsumerServer(int num_consumers, int max_queue_size, 
                               const std::string& output_dir)
    : num_consumers_(num_consumers), 
      max_queue_size_(max_queue_size),
      output_dir_(output_dir),
      running_(true),
      total_received_(0),
      total_dropped_(0),
      total_duplicates_(0) {
    
    // Create output directory if it doesn't exist
    if (!fs::exists(output_dir_)) {
        fs::create_directories(output_dir_);
    }
    
    std::cout << "Consumer Server initialized:" << std::endl;
    std::cout << "  Consumers: " << num_consumers_ << std::endl;
    std::cout << "  Queue size: " << max_queue_size_ << std::endl;
    std::cout << "  Output dir: " << output_dir_ << std::endl;
}

Status ConsumerServer::UploadVideo(ServerContext* context,
                                   ServerReader<VideoChunk>* reader,
                                   UploadResponse* response) {
    VideoChunk chunk;
    std::vector<char> video_data;
    std::string video_id;
    std::string filename;
    int producer_id = 0;
    size_t total_size = 0;
    int chunks_received = 0;

    // Read all chunks
    while (reader->Read(&chunk)) {
        if (video_id.empty()) {
            video_id = chunk.video_id();
            filename = chunk.filename();
            producer_id = chunk.producer_id();
            total_size = chunk.total_size();
            
            std::cout << "\n[CONSUMER] Receiving video: " << filename 
                      << " from Producer-" << producer_id << std::endl;
        }

        video_data.insert(video_data.end(), 
                         chunk.data().begin(), 
                         chunk.data().end());
        chunks_received++;

        if (chunk.is_last()) {
            std::cout << "[CONSUMER] Received final chunk #" << chunks_received 
                      << " for " << filename << std::endl;
            break;
        }
    }

    if (video_data.empty()) {
        response->set_success(false);
        response->set_message("No data received");
        return Status::OK;
    }

    // Calculate hash for duplicate detection
    std::string file_hash = calculateHash(video_data);
    
    // Check for duplicates
    {
        std::lock_guard<std::mutex> lock(hash_mutex_);
        if (uploaded_hashes_.find(file_hash) != uploaded_hashes_.end()) {
            total_duplicates_++;
            std::cout << "[CONSUMER] ⚠️  Duplicate detected: " << filename 
                      << " (hash: " << file_hash.substr(0, 8) << "...)" << std::endl;
            response->set_success(false);
            response->set_message("Duplicate file detected");
            
            // Log duplicate info
            VideoMetadata duplicate_meta;
            duplicate_meta.video_id = video_id;
            duplicate_meta.filename = filename;
            duplicate_meta.producer_id = producer_id;
            duplicate_meta.file_hash = file_hash;
            duplicate_meta.is_duplicate = true;
            
            std::lock_guard<std::mutex> meta_lock(metadata_mutex_);
            video_metadata_.push_back(duplicate_meta);
            
            return Status::OK;
        }
    }

    // Check queue capacity (leaky bucket)
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (upload_queue_.size() >= static_cast<size_t>(max_queue_size_)) {
            total_dropped_++;
            std::cout << "[CONSUMER] ❌ Queue full! Dropping: " << filename 
                      << " (queue: " << upload_queue_.size() << "/" 
                      << max_queue_size_ << ")" << std::endl;
            response->set_success(false);
            response->set_message("Queue full - video dropped");
            return Status::OK;
        }

        // Add to queue
        UploadTask task;
        task.video_id = video_id;
        task.filename = filename;
        task.producer_id = producer_id;
        task.data = std::move(video_data);
        task.file_hash = file_hash;
        task.total_size = total_size;

        upload_queue_.push(std::move(task));
        total_received_++;
        
        std::cout << "[CONSUMER] ✓ Queued: " << filename 
                  << " (queue: " << upload_queue_.size() << "/" 
                  << max_queue_size_ << ")" << std::endl;
    }
    
    queue_cv_.notify_one();

    response->set_success(true);
    response->set_message("Video queued for processing");
    return Status::OK;
}

Status ConsumerServer::GetQueueStatus(ServerContext* context,
                                     const QueueStatusRequest* request,
                                     QueueStatusResponse* response) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    response->set_current_size(upload_queue_.size());
    response->set_max_size(max_queue_size_);
    response->set_is_full(upload_queue_.size() >= static_cast<size_t>(max_queue_size_));
    response->set_available_slots(max_queue_size_ - upload_queue_.size());
    
    return Status::OK;
}

Status ConsumerServer::GetStatistics(ServerContext* context,
                                    const StatisticsRequest* request,
                                    StatisticsResponse* response) {
    std::lock_guard<std::mutex> lock(metadata_mutex_);
    
    response->set_total_received(total_received_);
    response->set_total_processed(processed_videos_.size());
    response->set_total_dropped(total_dropped_);
    response->set_total_duplicates(total_duplicates_);
    response->set_queue_size(upload_queue_.size());
    
    return Status::OK;
}

std::string ConsumerServer::calculateHash(const std::vector<char>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.data()), 
           data.size(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void ConsumerServer::consumerWorker(int consumer_id) {
    std::cout << "[CONSUMER-" << consumer_id << "] Worker started" << std::endl;

    while (running_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        queue_cv_.wait(lock, [this] {
            return !upload_queue_.empty() || !running_;
        });

        if (!running_ && upload_queue_.empty()) {
            break;
        }

        if (upload_queue_.empty()) {
            continue;
        }

        UploadTask task = std::move(upload_queue_.front());
        upload_queue_.pop();
        lock.unlock();

        // Process the video
        std::cout << "\n[CONSUMER-" << consumer_id << "] Processing: " 
                  << task.filename << std::endl;

        auto start_time = std::chrono::steady_clock::now();
        
        // Save video file
        std::string output_path = output_dir_ + "/" + task.video_id + "_" + task.filename;
        std::ofstream output_file(output_path, std::ios::binary);
        
        if (output_file) {
            output_file.write(task.data.data(), task.data.size());
            output_file.close();
            
            auto end_time = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time).count();

            std::cout << "[CONSUMER-" << consumer_id << "] ✓ Saved: " 
                      << output_path << " (" << duration << "ms)" << std::endl;

            // Store metadata
            VideoMetadata meta;
            meta.video_id = task.video_id;
            meta.filename = task.filename;
            meta.producer_id = task.producer_id;
            meta.file_path = output_path;
            meta.file_size = task.total_size;
            meta.file_hash = task.file_hash;
            meta.upload_time = std::chrono::system_clock::now();
            meta.is_duplicate = false;
            meta.consumer_id = consumer_id;

            {
                std::lock_guard<std::mutex> meta_lock(metadata_mutex_);
                video_metadata_.push_back(meta);
                processed_videos_[task.video_id] = output_path;
                uploaded_hashes_.insert(task.file_hash);
            }

            // Generate thumbnail (placeholder - would use FFmpeg in real implementation)
            generateThumbnail(output_path, task.video_id);
            
        } else {
            std::cerr << "[CONSUMER-" << consumer_id << "] ❌ Failed to save: " 
                      << output_path << std::endl;
        }

        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[CONSUMER-" << consumer_id << "] Worker stopped" << std::endl;
}

void ConsumerServer::generateThumbnail(const std::string& video_path, 
                                      const std::string& video_id) {
    // Placeholder for thumbnail generation
    // In real implementation, use FFmpeg:
    // ffmpeg -i video_path -ss 00:00:01 -vframes 1 output_thumbnail.jpg
    
    std::string thumb_dir = output_dir_ + "/thumbnails";
    if (!fs::exists(thumb_dir)) {
        fs::create_directories(thumb_dir);
    }
    
    // For now, just log that we would generate it
    std::cout << "[THUMBNAIL] Would generate thumbnail for: " << video_id << std::endl;
}

void ConsumerServer::start() {
    // Start consumer worker threads
    for (int i = 0; i < num_consumers_; i++) {
        consumer_threads_.emplace_back([this, i]() {
            consumerWorker(i + 1);
        });
    }

    std::cout << "\n✓ Started " << num_consumers_ << " consumer workers" << std::endl;
}

void ConsumerServer::stop() {
    std::cout << "\nStopping consumer server..." << std::endl;
    
    running_ = false;
    queue_cv_.notify_all();

    for (auto& thread : consumer_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    printStatistics();
}

void ConsumerServer::printStatistics() {
    std::cout << "\n=== Final Statistics ===" << std::endl;
    std::cout << "Total received:   " << total_received_ << std::endl;
    std::cout << "Total processed:  " << processed_videos_.size() << std::endl;
    std::cout << "Total dropped:    " << total_dropped_ << std::endl;
    std::cout << "Total duplicates: " << total_duplicates_ << std::endl;
    std::cout << "Success rate:     " << std::fixed << std::setprecision(2)
              << (total_received_ > 0 ? 
                  (processed_videos_.size() * 100.0 / total_received_) : 0)
              << "%" << std::endl;
}

std::vector<VideoMetadata> ConsumerServer::getVideoMetadata() {
    std::lock_guard<std::mutex> lock(metadata_mutex_);
    return video_metadata_;
}
