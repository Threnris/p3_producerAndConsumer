# Media Upload Service - Producer-Consumer System

A distributed media upload service implementing producer-consumer pattern with gRPC communication, queue management, and web-based GUI.

## Features

### Core Features ✅

1. **Producer Threads** - Multiple producers reading from separate folders
2. **Consumer Threads** - Concurrent processing of uploaded videos
3. **gRPC Communication** - Efficient streaming protocol for file transfer
4. **Queue Management** - Leaky bucket design with configurable capacity
5. **Web-based GUI** - Real-time dashboard with video preview and playback

### Bonus Features ⭐

1. **Queue Status Notification** - Producers check queue status before uploading
2. **Duplicate Detection** - SHA-256 hash-based duplicate identification
3. **Video Compression** - (Placeholder for FFmpeg integration)
4. **10-Second Preview** - Hover over video cards to see preview
5. **Full Playback** - Click to watch complete videos

## Architecture

```
┌─────────────┐         gRPC          ┌─────────────┐
│  Producer 1 │────┐                ┌──│ Consumer 1  │
├─────────────┤    │                │  ├─────────────┤
│  Producer 2 │────┤   Network      ├──│ Consumer 2  │
├─────────────┤    │   Stream       │  ├─────────────┤
│  Producer 3 │────┴────────────────┴──│ Consumer 3  │
└─────────────┘                        └─────────────┘
                                              │
                                              ▼
                                       ┌─────────────┐
                                       │    Queue    │
                                       │  (Leaky)    │
                                       └─────────────┘
                                              │
                                              ▼
                                       ┌─────────────┐
                                       │   Storage   │
                                       │ + Metadata  │
                                       └─────────────┘
                                              │
                                              ▼
                                       ┌─────────────┐
                                       │   Web GUI   │
                                       │  (HTTP:8080)│
                                       └─────────────┘
```

## System Requirements

- C++20 compiler (GCC 10+ or Clang 12+)
- CMake 3.15+
- gRPC and Protocol Buffers
- OpenSSL (for SHA-256 hashing)
- Web browser (for GUI)

## Building the Project

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgrpc++-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    libssl-dev \
    pkg-config
```

### Build

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Executables will be in build/
# - consumer_server
# - producer_client
```

## Usage

### 1. Prepare Video Files

Create directory structure for producers:

```bash
mkdir -p video_files/producer_1
mkdir -p video_files/producer_2
mkdir -p video_files/producer_3

# Add video files (.mp4, .avi, .mov, etc.) to each folder
cp sample1.mp4 video_files/producer_1/
cp sample2.mp4 video_files/producer_2/
cp sample3.mp4 video_files/producer_3/
```

### 2. Start Consumer Server

```bash
./consumer_server -c 4 -q 10
```

**Options:**
- `-c <num>` - Number of consumer threads (default: 4)
- `-q <num>` - Queue capacity/max size (default: 10)
- `-p <port>` - gRPC port (default: 50051)
- `-w <port>` - Web GUI port (default: 8080)
- `-o <dir>` - Output directory (default: ./uploaded_videos)

**Example:**
```bash
./consumer_server -c 8 -q 20 -p 50051 -w 8080 -o /data/videos
```

### 3. Start Producer Clients

```bash
./producer_client -p 3
```

**Options:**
- `-p <num>` - Number of producer threads (required)
- `-s <addr>` - Server address (default: localhost:50051)
- `-i <dir>` - Base input directory (default: ./video_files)

**Example:**
```bash
./producer_client -p 5 -s 192.168.1.100:50051 -i /home/user/videos
```

### 4. Access Web GUI

Open browser and navigate to:
```
http://localhost:8080
```

## Web GUI Features

### Dashboard View

- **Statistics Cards**
  - Total Received
  - Total Processed
  - Total Dropped
  - Total Duplicates
  - Success Rate

- **Queue Status Bar**
  - Visual representation of queue utilization
  - Real-time updates
  - Color coding (green = available, red = full)

### Video Gallery

- **Grid Layout** - Responsive video card grid
- **Filtering** - All / Processed / Duplicates
- **Video Cards** show:
  - Filename
  - File size
  - Producer ID
  - Upload timestamp
  - Status badge

### Video Interactions

- **Hover** - Auto-play 10-second preview
- **Click** - Open full-screen player
- **ESC** - Close player

### Auto-Refresh

- Dashboard auto-refreshes every 5 seconds
- Manual refresh button available

## Input Parameters Summary

### Consumer Server (`-c`, `-q`)
```bash
./consumer_server -c <consumers> -q <queue_size>
```
- `c`: Number of consumer threads (1-100)
- `q`: Maximum queue length (1-1000)

Example: `./consumer_server -c 8 -q 15`

### Producer Client (`-p`)
```bash
./producer_client -p <producers>
```
- `p`: Number of producer threads (1-50)

Example: `./producer_client -p 5`

## Output

### Console Output

**Consumer Server:**
```
╔════════════════════════════════════════════════╗
║  Media Upload Service - Consumer Server       ║
╚════════════════════════════════════════════════╝

📊 Configuration:
  Consumer threads: 4
  Queue capacity:   10 (Leaky bucket)
  gRPC port:        50051
  Web GUI port:     8080
  Output directory: ./uploaded_videos

✨ Features enabled:
  ✓ Queue management (leaky bucket)
  ✓ Duplicate detection (SHA-256 hashing)
  ✓ Queue status API for producers
  ✓ Web-based GUI with video preview
  ✓ Real-time statistics

✓ Started 4 consumer workers
🚀 gRPC Server listening on 0.0.0.0:50051
🌐 Web GUI available at http://localhost:8080

✅ Server is ready to accept uploads!
```

**Producer Client:**
```
╔════════════════════════════════════════════════╗
║  Media Upload Service - Producer Client       ║
╚════════════════════════════════════════════════╝

 Media Upload Producer Client
Producers:       3
Server:          localhost:50051
Input directory: ./video_files

[PRODUCER-1] Found 2 video files in ./video_files/producer_1
[PRODUCER-2] Found 3 video files in ./video_files/producer_2
[PRODUCER-3] Found 1 video files in ./video_files/producer_3

All producer threads started. Press Ctrl+C to stop...
```

### Uploaded Files

Videos saved to: `<output_dir>/<video_id>_<filename>`

Example:
```
uploaded_videos/
├── VID_1_1702345678_456_sample1.mp4
├── VID_2_1702345690_789_sample2.mp4
└── thumbnails/
    ├── VID_1_1702345678_456.jpg
    └── VID_2_1702345690_789.jpg
```

## Implementation Details

### Queue Management (Leaky Bucket)

- Fixed capacity defined by `-q` parameter
- Videos beyond capacity are dropped
- Producers notified of queue status
- FIFO processing order

### Duplicate Detection

- SHA-256 hash calculated for each video
- Hash stored in set for O(1) lookup
- Duplicates rejected before processing
- Original file preserved, duplicate logged

### Video Preview

- JavaScript creates `<video>` element on hover
- Auto-plays for 10 seconds
- Muted playback
- Starts from beginning

### gRPC Streaming

- Chunk size: 64KB
- Progress updates every 10 chunks
- Metadata in first chunk
- `is_last` flag in final chunk

## API Endpoints

### gRPC (Port 50051)

1. **UploadVideo** - Stream video chunks
   ```protobuf
   rpc UploadVideo(stream VideoChunk) returns (UploadResponse)
   ```

2. **GetQueueStatus** - Check queue state
   ```protobuf
   rpc GetQueueStatus(QueueStatusRequest) returns (QueueStatusResponse)
   ```

3. **GetStatistics** - Retrieve server stats
   ```protobuf
   rpc GetStatistics(StatisticsRequest) returns (StatisticsResponse)
   ```

### HTTP (Port 8080)

1. **GET /api/statistics** - JSON stats
2. **GET /api/queue** - JSON queue status
3. **GET /api/videos** - JSON video list
4. **GET /** - Serve web GUI

## Bonus Features Explained

### 1. Queue Full Notification ✅

**Implementation:** Producers call `GetQueueStatus()` RPC before each upload.

```cpp
bool ProducerThread::checkQueueStatus() {
    ClientContext context;
    QueueStatusRequest request;
    QueueStatusResponse response;
    
    Status status = stub_->GetQueueStatus(&context, request, &response);
    
    if (status.ok() && response.is_full()) {
        // Wait and retry
        return false;
    }
    return true;
}
```

### 2. Duplicate Detection ✅

**Implementation:** SHA-256 hash calculated on consumer side.

```cpp
std::string ConsumerServer::calculateHash(const std::vector<char>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.data()), 
           data.size(), hash);
    return hexString(hash);
}
```

Hashes stored in `std::unordered_set` for fast lookup.

### 3. Video Compression

**Placeholder for FFmpeg integration:**

```bash
# Example FFmpeg command
ffmpeg -i input.mp4 -c:v libx264 -crf 23 -preset medium output.mp4
```

Can be integrated in `consumerWorker()` method.

## Troubleshooting

### Producer can't connect to server

**Error:** `Failed to connect to localhost:50051`

**Solution:** 
- Ensure consumer server is running
- Check firewall rules
- Verify port in use: `netstat -an | grep 50051`
- Use correct server address with `-s` flag

### Queue fills up too quickly

**Solution:**
- Increase queue size: `-q 20`
- Add more consumers: `-c 8`
- Reduce producer count: `-p 2`

### Web GUI not loading

**Solution:**
- Check web server port: `netstat -an | grep 8080`
- Verify `web/` directory exists in build folder
- Check browser console for errors
- Try different port: `-w 8081`

### Videos not appearing in GUI

**Solution:**
- Check output directory permissions
- Verify files exist: `ls uploaded_videos/`
- Check browser network tab
- Refresh page manually

## Performance Tuning

### For High Throughput

```bash
# More consumers, larger queue
./consumer_server -c 16 -q 50
```

### For Low Latency

```bash
# Fewer consumers, smaller queue
./consumer_server -c 2 -q 5
```

### For Many Producers

```bash
# Large queue, many consumers
./consumer_server -c 12 -q 100
```

## Testing

### Test with Small Files

```bash
# Create test videos (requires FFmpeg)
ffmpeg -f lavfi -i testsrc=duration=10:size=1280x720:rate=30 test1.mp4
ffmpeg -f lavfi -i testsrc=duration=10:size=1280x720:rate=30 test2.mp4
```

### Load Testing

```bash
# Many producers
./producer_client -p 10

# Monitor in another terminal
watch -n 1 'curl -s http://localhost:8080/api/statistics | jq .'
```

## Project Structure

```
MediaUploadService/
├── CMakeLists.txt
├── README.md
├── media_service.proto
├── include/
│   ├── producerClient.h
│   ├── producerThread.h
│   ├── consumerServer.h
│   └── webServer.h
├── producerMain.cpp
├── producerClient.cpp
├── producerThread_enhanced.cpp
├── serverMain.cpp
├── consumerServer.cpp
├── webServer.cpp
└── web/
    └── index.html
```

## License

Educational project for STDISCM coursework.

## Credits

Created for Distributed Computing course - Producer-Consumer exercise with network sockets and file I/O.
