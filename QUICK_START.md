# Quick Start Guide - Media Upload Service

## 🚀 Getting Started in 3 Steps

### 1️⃣ Setup & Build
```bash
./setup.sh
```

### 2️⃣ Start Server
```bash
cd build
./consumer_server -c 4 -q 10
```

### 3️⃣ Start Producers
```bash
# In another terminal
cd build
./producer_client -p 3
```

### 4️⃣ View Dashboard
Open browser: **http://localhost:8080**

---

## 📋 Command Reference

### Consumer Server
```bash
./consumer_server -c <consumers> -q <queue_size> [options]

Options:
  -c <num>   Number of consumer threads (default: 4)
  -q <num>   Queue capacity (default: 10)
  -p <port>  gRPC port (default: 50051)
  -w <port>  Web port (default: 8080)
  -o <dir>   Output directory (default: ./uploaded_videos)
```

**Examples:**
```bash
# Standard setup
./consumer_server -c 4 -q 10

# High performance
./consumer_server -c 16 -q 50 -p 50051 -w 8080

# Custom output
./consumer_server -c 8 -q 20 -o /data/videos
```

### Producer Client
```bash
./producer_client -p <producers> [options]

Options:
  -p <num>   Number of producer threads (REQUIRED)
  -s <addr>  Server address (default: localhost:50051)
  -i <dir>   Input directory (default: ./video_files)
```

**Examples:**
```bash
# Standard setup
./producer_client -p 3

# Remote server
./producer_client -p 5 -s 192.168.1.100:50051

# Custom input directory
./producer_client -p 4 -i /home/user/my_videos
```

---

## 📁 Directory Structure

### Input (Producer Side)
```
video_files/
├── producer_1/
│   ├── video1.mp4
│   └── video2.mp4
├── producer_2/
│   ├── video3.mp4
│   └── video4.mp4
└── producer_3/
    └── video5.mp4
```

### Output (Consumer Side)
```
uploaded_videos/
├── VID_1_1234567890_123_video1.mp4
├── VID_2_1234567891_456_video3.mp4
└── thumbnails/
    ├── VID_1_1234567890_123.jpg
    └── VID_2_1234567891_456.jpg
```

---

## 🎯 Common Scenarios

### Test with 3 Producers, 4 Consumers, Queue Size 10
```bash
# Terminal 1 - Server
./consumer_server -c 4 -q 10

# Terminal 2 - Producers
./producer_client -p 3
```

### High Load Test (10 Producers, 8 Consumers, Queue Size 30)
```bash
# Terminal 1 - Server
./consumer_server -c 8 -q 30

# Terminal 2 - Producers
./producer_client -p 10
```

### Multiple Machines
```bash
# Machine 1 (Server: 192.168.1.100)
./consumer_server -c 8 -q 20

# Machine 2 (Producer)
./producer_client -p 5 -s 192.168.1.100:50051
```

---

## 🌐 Web GUI Features

### Dashboard (http://localhost:8080)

**Statistics:**
- Total Received
- Total Processed  
- Total Dropped
- Total Duplicates
- Success Rate

**Queue Status:**
- Visual bar showing queue utilization
- Current size / Max size
- Color changes when full

**Video Gallery:**
- Grid layout of all uploaded videos
- Filter: All / Processed / Duplicates
- Each card shows: filename, size, producer ID, timestamp

**Interactions:**
- **Hover** → 10-second preview
- **Click** → Full playback
- **ESC** → Close player

---

## ✅ Feature Checklist

### Core Requirements
- [x] **P** - Number of producer threads
- [x] **C** - Number of consumer threads  
- [x] **Q** - Max queue length (leaky bucket)
- [x] **GUI** - Web-based interface
- [x] **Preview** - 10-second hover preview
- [x] **Playback** - Full video on click

### Bonus Features
- [x] **Queue Full Notification** - Producers check before upload
- [x] **Duplicate Detection** - SHA-256 hash comparison
- [ ] **Video Compression** - Placeholder for FFmpeg

---

## 🔧 Troubleshooting

### Server won't start
```bash
# Check if port is in use
netstat -tuln | grep 50051
netstat -tuln | grep 8080

# Kill existing process
pkill consumer_server

# Try different ports
./consumer_server -c 4 -q 10 -p 50052 -w 8081
```

### Producers can't connect
```bash
# Check server is running
ps aux | grep consumer_server

# Test connection
telnet localhost 50051

# Check firewall
sudo ufw status
sudo ufw allow 50051
```

### No videos in GUI
```bash
# Check output directory
ls -la uploaded_videos/

# Check permissions
chmod 755 uploaded_videos/

# Verify files were uploaded
tail -f consumer_server.log
```

### Web GUI not loading
```bash
# Check web directory exists
ls -la build/web/

# Test web server
curl http://localhost:8080/api/statistics

# Try manual refresh
Ctrl+F5 in browser
```

---

## 📊 Monitoring

### Real-time Statistics (Terminal)
```bash
# Watch queue status
watch -n 1 'curl -s http://localhost:8080/api/queue | jq .'

# Watch statistics
watch -n 1 'curl -s http://localhost:8080/api/statistics | jq .'

# Watch videos
watch -n 1 'curl -s http://localhost:8080/api/videos | jq length'
```

### Log Analysis
```bash
# Producer logs
./producer_client -p 3 2>&1 | tee producer.log

# Consumer logs
./consumer_server -c 4 -q 10 2>&1 | tee consumer.log

# Count successful uploads
grep "✓ Uploaded" producer.log | wc -l

# Count dropped videos
grep "Queue full" consumer.log | wc -l

# Count duplicates
grep "Duplicate detected" consumer.log | wc -l
```

---

## 🎓 Understanding the System

### Producer Behavior
1. Scans `video_files/producer_N/` for video files
2. **Checks queue status** (bonus feature)
3. Generates unique video ID
4. Streams video in 64KB chunks
5. Reports progress every 10 chunks
6. Waits 2-5 seconds between uploads

### Consumer Behavior
1. Receives video stream via gRPC
2. **Checks for duplicates** (SHA-256 hash)
3. Checks queue capacity (leaky bucket)
4. Queues video for processing
5. Consumer threads process queue
6. Saves to disk with metadata
7. Generates thumbnail (placeholder)

### Queue Management (Leaky Bucket)
- Fixed capacity (q parameter)
- Videos beyond capacity are **dropped**
- FIFO processing
- Producers get queue status
- Real-time monitoring via GUI

---

## 📈 Performance Tips

### For Maximum Throughput
```bash
# More consumers + larger queue
./consumer_server -c 16 -q 100

# More producers
./producer_client -p 10
```

### For Low Latency
```bash
# Fewer consumers + smaller queue
./consumer_server -c 2 -q 5

# Fewer producers
./producer_client -p 2
```

### For Stability
```bash
# Balanced setup
./consumer_server -c 4 -q 10
./producer_client -p 3
```

---

## 🧪 Testing Scenarios

### Test 1: Normal Operation
- 3 producers, 4 consumers, queue size 10
- Should process all videos successfully

### Test 2: Queue Overflow
- 10 producers, 2 consumers, queue size 5
- Should drop videos when queue is full

### Test 3: Duplicate Detection
- Upload same video twice
- Second upload should be rejected

### Test 4: High Concurrency
- 20 producers, 16 consumers, queue size 50
- Should handle high load

---

## 📞 Support

For issues or questions:
1. Check logs in terminal
2. Verify directory structure
3. Check firewall settings
4. Review README.md for detailed info

---

**Happy uploading! 🎬**
