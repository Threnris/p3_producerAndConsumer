# 🚀 MediaUploadService - Quick Setup Guide

## 📦 Extract the Zip

```bash
unzip MediaUploadService.zip
cd MediaUploadService
```

## 📁 What's Inside

```
MediaUploadService/
├── 📘 Documentation
│   ├── START_HERE.md              ⭐ READ THIS FIRST
│   ├── QUICK_START.md             Quick reference
│   ├── README.md                   Full documentation
│   ├── IMPLEMENTATION_SUMMARY.md   Feature details
│   └── FILE_INDEX.md              File reference
│
├── 🔧 Build & Setup
│   ├── CMakeLists.txt             Build configuration
│   └── setup.sh                   Automated setup script
│
├── 📡 Protocol
│   └── media_service.proto        gRPC definitions
│
├── 📂 Headers (include/)
│   ├── consumerServer.h
│   ├── producerClient.h
│   ├── producerThread.h
│   └── webServer.h
│
├── 💻 Source Code (src/)
│   ├── serverMain.cpp             Server entry point
│   ├── consumerServer.cpp         Consumer logic
│   ├── webServer.cpp              Web GUI server
│   ├── producerMain.cpp           Producer entry point
│   ├── producerClient.cpp         Producer orchestration
│   ├── producerThread_enhanced.cpp Producer thread (use this)
│   └── producerThread.cpp         Original thread (reference)
│
├── 🌐 Web Interface (web/)
│   └── index.html                 Dashboard GUI
│
├── 📹 Input Videos (video_files/)
│   ├── producer_1/                Place videos here
│   ├── producer_2/                Place videos here
│   └── producer_3/                Place videos here
│
└── 📤 Output (uploaded_videos/)
    └── thumbnails/                Generated thumbnails
```

## ⚡ Quick Start (3 Steps)

### Step 1: Run Setup
```bash
chmod +x setup.sh
./setup.sh
```

This will:
- ✅ Check dependencies
- ✅ Create directories
- ✅ Generate test videos (if FFmpeg available)
- ✅ Build the project

### Step 2: Start Server (Terminal 1)
```bash
cd build
./consumer_server -c 4 -q 10
```

### Step 3: Start Producers (Terminal 2)
```bash
cd build
./producer_client -p 3
```

### Step 4: Open Browser
```
http://localhost:8080
```

## 📋 Prerequisites

### Ubuntu/Debian
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

### Optional (for test videos)
```bash
sudo apt-get install -y ffmpeg
```

## 🎯 What It Does

1. **Producers** read videos from `video_files/producer_X/`
2. **Check queue status** before uploading (Bonus Feature #1)
3. **Stream videos** to server via gRPC
4. **Server detects duplicates** using SHA-256 (Bonus Feature #2)
5. **Queue manages** uploads with leaky bucket (parameter q)
6. **Consumers process** videos concurrently (parameter c)
7. **Web GUI shows** real-time stats and video gallery
8. **Preview on hover** (10 seconds)
9. **Play on click** (full video)

## 📊 Parameters

### Server (Consumer)
```bash
./consumer_server -c <consumers> -q <queue_size>

-c  Number of consumer threads (1-100)
-q  Queue capacity (1-1000)
-p  gRPC port (default: 50051)
-w  Web port (default: 8080)
-o  Output directory (default: ./uploaded_videos)
```

### Producer (Client)
```bash
./producer_client -p <producers>

-p  Number of producer threads (1-50) [REQUIRED]
-s  Server address (default: localhost:50051)
-i  Input directory (default: ./video_files)
```

## 🎬 Example Usage

### Standard Test
```bash
# Terminal 1
./consumer_server -c 4 -q 10

# Terminal 2
./producer_client -p 3
```

### High Load Test
```bash
# Terminal 1
./consumer_server -c 16 -q 50

# Terminal 2
./producer_client -p 10
```

### Remote Server
```bash
# Server (192.168.1.100)
./consumer_server -c 8 -q 20

# Producer (different machine)
./producer_client -p 5 -s 192.168.1.100:50051
```

## ✨ Features Implemented

- ✅ Producer threads (p parameter)
- ✅ Consumer threads (c parameter)
- ✅ Queue management (q parameter)
- ✅ Separate input folders per producer
- ✅ gRPC network communication
- ✅ Web-based GUI
- ✅ 10-second video preview
- ✅ Full video playback
- ✅ BONUS: Queue full notification
- ✅ BONUS: Duplicate detection
- 🟡 BONUS: Video compression (placeholder)

## 🔧 Troubleshooting

### Setup fails
```bash
# Check dependencies
cmake --version
g++ --version
protoc --version
pkg-config --exists grpc++
```

### Build fails
```bash
# Clean and rebuild
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Can't connect
```bash
# Check if server is running
ps aux | grep consumer_server

# Check ports
netstat -tuln | grep 50051
netstat -tuln | grep 8080
```

## 📚 Documentation

- **START_HERE.md** - Project overview
- **QUICK_START.md** - Command reference
- **README.md** - Complete documentation
- **IMPLEMENTATION_SUMMARY.md** - Technical details
- **FILE_INDEX.md** - File reference

## 🎓 Assignment Requirements

✅ **All core requirements met:**
- p, c, q parameters
- Separate folders per producer
- gRPC communication between VMs
- GUI with video preview and playback

✅ **Bonus features:**
- Queue status notification to producers
- Duplicate detection via hashing

## 🆘 Need Help?

1. Read **START_HERE.md** for overview
2. Check **QUICK_START.md** for commands
3. Review **README.md** for details
4. Check console error messages
5. Verify directory structure matches above

## 🎉 You're Ready!

Everything is configured and ready to run. Just follow the 3 steps above!

**Happy uploading! 🚀**
