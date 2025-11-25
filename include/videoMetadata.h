#pragma once

#include <string>
#include <ctime>

struct VideoMetadata {
    std::string video_id;
    std::string filename;
    std::string filepath;
    size_t size;
    std::string upload_time;
    int producer_id;

    VideoMetadata() : size(0), producer_id(0) {}
    
    VideoMetadata(const std::string& vid, const std::string& fname,
                  const std::string& fpath, size_t sz,
                  const std::string& utime, int pid)
        : video_id(vid), filename(fname), filepath(fpath),
          size(sz), upload_time(utime), producer_id(pid) {}
};