#include "include/producerThread.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <grpcpp/grpcpp.h>
#include "media_service.grpc.pb.h"
    
namespace fs = std::filesystem;

ProducerThread::ProducerThread(int id, const std::string& input_dir,
                              std::shared_ptr<Channel> channel)
    : producer_id_(id), input_dir_(input_dir), channel_(channel),
      stub_(mediaupload::MediaUploadService::NewStub(channel)),
      running_(true), uploaded_count_(0), failed_count_(0) {}

std::string ProducerThread::generateVideoId() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999);
    
    return "VID_" + std::to_string(producer_id_) + "_" + 
           std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

bool ProducerThread::isVideoFile(const std::string& filename) {
    std::string lower = filename;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    const std::vector<std::string> extensions = {
        ".mp4", ".avi", ".mov", ".mkv", ".flv", ".wmv", ".webm", ".m4v"
    };
    
    for (const auto& ext : extensions) {
        if (lower.size() >= ext.size() &&
            lower.substr(lower.size() - ext.size()) == ext) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ProducerThread::getVideoFiles() {
    std::vector<std::string> files;
    
    if (!fs::exists(input_dir_)) {
        std::cerr << "[PRODUCER-" << producer_id_ << "] Directory not found: " 
                  << input_dir_ << std::endl;
        return files;
    }

    for (const auto& entry : fs::directory_iterator(input_dir_)) {
        if (entry.is_regular_file() && isVideoFile(entry.path().filename().string())) {
            files.push_back(entry.path().string());
        }
    }
    
    std::cout << "[PRODUCER-" << producer_id_ << "] Found " << files.size() 
              << " video files in " << input_dir_ << std::endl;
    return files;
}

std::string ProducerThread::formatFileSize(size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double dsize = static_cast<double>(size);
    
    while (dsize >= 1024.0 && unit < 3) {
        dsize /= 1024.0;
        unit++;
    }
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f %s", dsize, units[unit]);
    return std::string(buffer);
}

// BONUS FEATURE #1: Producer can check if queue is full
bool ProducerThread::checkQueueStatus() {
    ClientContext context;
    mediaupload::QueueStatusRequest request;
    mediaupload::QueueStatusResponse response;
    
    Status status = stub_->GetQueueStatus(&context, request, &response);
    
    if (status.ok()) {
        std::cout << "[PRODUCER-" << producer_id_ << "] Queue status: " 
                  << response.current_size() << "/" << response.max_size();
        
        if (response.is_full()) {
            std::cout << " ⚠️  FULL!" << std::endl;
            return false;
        }
        std::cout << " (" << response.available_slots() << " slots available)" << std::endl;
        return true;
    }
    
    return true; // Assume queue is available if check fails
}

bool ProducerThread::uploadVideo(const std::string& filepath) {
    // BONUS FEATURE #1: Check queue before uploading
    if (!checkQueueStatus()) {
        std::cout << "[PRODUCER-" << producer_id_ << "] Queue is full, waiting 2s..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        failed_count_++;
        return false;
    }

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[PRODUCER-" << producer_id_ << "] Failed to open: " 
                  << filepath << std::endl;
        return false;
    }

    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string filename = fs::path(filepath).filename().string();
    std::string video_id = generateVideoId();

    std::cout << "\n[PRODUCER-" << producer_id_ << "] Starting upload: " 
              << filename << " (" << formatFileSize(file_size) << ")" << std::endl;
    std::cout << "[PRODUCER-" << producer_id_ << "] Video ID: " << video_id << std::endl;

    ClientContext context;
    mediaupload::UploadResponse response;
    std::unique_ptr<ClientWriter<mediaupload::VideoChunk>> writer(
        stub_->UploadVideo(&context, &response));

    std::vector<char> buffer(CHUNK_SIZE);
    int chunk_number = 0;
    size_t total_sent = 0;

    while (file.read(buffer.data(), CHUNK_SIZE) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        
        mediaupload::VideoChunk chunk;
        chunk.set_video_id(video_id);
        chunk.set_filename(filename);
        chunk.set_data(buffer.data(), bytes_read);
        chunk.set_chunk_number(chunk_number++);
        chunk.set_producer_id(producer_id_);
        chunk.set_total_size(file_size);
        
        if (file.peek() == EOF) {
            chunk.set_is_last(true);
        } else {
            chunk.set_is_last(false);
        }

        if (!writer->Write(chunk)) {
            std::cerr << "[PRODUCER-" << producer_id_ << "] Failed to write chunk " 
                      << chunk_number << std::endl;
            return false;
        }

        total_sent += bytes_read;
        
        if (chunk_number % 10 == 0) {
            int progress = static_cast<int>((total_sent * 100) / file_size);
            std::cout << "[PRODUCER-" << producer_id_ << "] Progress: " 
                      << progress << "% (" << formatFileSize(total_sent) 
                      << "/" << formatFileSize(file_size) << ")" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    writer->WritesDone();
    Status status = writer->Finish();

    if (status.ok() && response.success()) {
        std::cout << "[PRODUCER-" << producer_id_ << "] ✓ Uploaded: " 
                  << filename << std::endl;
        std::cout << "[PRODUCER-" << producer_id_ << "] Server: " 
                  << response.message() << std::endl;
        uploaded_count_++;
        return true;
    } else {
        std::cerr << "[PRODUCER-" << producer_id_ << "] ❌ Upload failed: " 
                  << filename << std::endl;
        if (!status.ok()) {
            std::cerr << "[PRODUCER-" << producer_id_ << "] gRPC error: " 
                      << status.error_message() << std::endl;
        } else {
            std::cerr << "[PRODUCER-" << producer_id_ << "] Server error: " 
                      << response.message() << std::endl;
        }
        failed_count_++;
        return false;
    }
}

void ProducerThread::run() {
    std::cout << "[PRODUCER-" << producer_id_ << "] Thread started" << std::endl;
    std::cout << "[PRODUCER-" << producer_id_ << "] Input directory: " 
              << input_dir_ << std::endl;

    auto video_files = getVideoFiles();
    
    if (video_files.empty()) {
        std::cerr << "[PRODUCER-" << producer_id_ 
                  << "] No video files found. Exiting." << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2000, 5000);

    size_t file_index = 0;
    while (running_ && file_index < video_files.size()) {
        const std::string& video_file = video_files[file_index];
        
        std::cout << "\n[PRODUCER-" << producer_id_ << "] Preparing file " 
                  << (file_index + 1) << "/" << video_files.size() << std::endl;
        
        uploadVideo(video_file);
        
        file_index++;
        
        if (file_index < video_files.size() && running_) {
            int wait_time = dis(gen);
            std::cout << "[PRODUCER-" << producer_id_ << "] Waiting " 
                      << wait_time << "ms..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
        }
    }

    std::cout << "\n[PRODUCER-" << producer_id_ << "] Thread finished" << std::endl;
    std::cout << "[PRODUCER-" << producer_id_ << "] Statistics:" << std::endl;
    std::cout << "  - Successful: " << uploaded_count_ << std::endl;
    std::cout << "  - Failed: " << failed_count_ << std::endl;
}

void ProducerThread::stop() {
    running_ = false;
}
