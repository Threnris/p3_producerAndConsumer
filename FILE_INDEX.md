# 📁 Media Upload Service - Project Files Index

## 📖 Start Here

1. **IMPLEMENTATION_SUMMARY.md** - Overview of all features and changes
2. **QUICK_START.md** - Get running in minutes
3. **README.md** - Complete documentation

## 🏗️ Source Code Files

### Server (Consumer) Side

| File | Purpose | Lines |
|------|---------|-------|
| `serverMain.cpp` | Server application entry point | ~150 |
| `consumerServer.cpp` | Queue management & video processing | ~300 |
| `consumerServer.h` | Consumer server header | ~80 |
| `webServer.cpp` | HTTP server for GUI | ~200 |
| `webServer.h` | Web server header | ~40 |

### Client (Producer) Side

| File | Purpose | Lines |
|------|---------|-------|
| `producerMain.cpp` | Producer application entry point | ~90 |
| `producerClient.cpp` | Producer orchestration | ~80 |
| `producerClient.h` | Producer client header | ~30 |
| `producerThread_enhanced.cpp` | Producer thread with queue checking | ~250 |
| `producerThread.h` | Producer thread header | ~40 |
| `producerThread.cpp` | Original producer thread (reference) | ~220 |

### Protocol & Communication

| File | Purpose | Lines |
|------|---------|-------|
| `media_service.proto` | gRPC service definitions | ~90 |

### Web Interface

| File | Purpose | Lines |
|------|---------|-------|
| `web/index.html` | Complete web GUI | ~600 |

### Build & Setup

| File | Purpose | Lines |
|------|---------|-------|
| `CMakeLists.txt` | Build configuration | ~75 |
| `setup.sh` | Automated setup script | ~140 |

### Documentation

| File | Purpose | Pages |
|------|---------|-------|
| `README.md` | Complete user guide | ~20 |
| `QUICK_START.md` | Quick reference | ~8 |
| `IMPLEMENTATION_SUMMARY.md` | Feature overview | ~15 |

## 📊 File Organization

```
MediaUploadService/
│
├── 📘 Documentation
│   ├── README.md                    # Complete guide
│   ├── QUICK_START.md              # Quick reference
│   └── IMPLEMENTATION_SUMMARY.md   # Feature overview
│
├── 🏗️ Build Configuration
│   ├── CMakeLists.txt              # Build system
│   └── setup.sh                    # Setup script
│
├── 📡 Protocol Definition
│   └── media_service.proto         # gRPC definitions
│
├── 💻 Server Components
│   ├── serverMain.cpp              # Server entry point
│   ├── consumerServer.cpp          # Core server logic
│   ├── consumerServer.h            # Server header
│   ├── webServer.cpp               # HTTP server
│   └── webServer.h                 # HTTP header
│
├── 📤 Producer Components
│   ├── producerMain.cpp            # Producer entry point
│   ├── producerClient.cpp          # Client orchestration
│   ├── producerClient.h            # Client header
│   ├── producerThread_enhanced.cpp # Enhanced thread
│   ├── producerThread.cpp          # Original thread
│   └── producerThread.h            # Thread header
│
├── 🌐 Web Interface
│   └── web/
│       └── index.html              # GUI dashboard
│
└── 📂 Headers
    └── include/
        ├── consumerServer.h
        ├── producerClient.h
        ├── producerThread.h
        └── webServer.h
```

## 🎯 What to Read First

### If you want to understand the system:
1. Start with `IMPLEMENTATION_SUMMARY.md`
2. Read the architecture section in `README.md`
3. Review `media_service.proto` for gRPC messages

### If you want to run it immediately:
1. Read `QUICK_START.md`
2. Run `./setup.sh`
3. Follow the 3-step guide

### If you want to modify the code:
1. Read `consumerServer.cpp` for server logic
2. Read `producerThread_enhanced.cpp` for producer logic
3. Read `web/index.html` for GUI

## 🔑 Key Features by File

### Queue Management
- **Implemented in:** `consumerServer.cpp`
- **Methods:** `UploadVideo()`, queue operations
- **Lines:** 90-150

### Duplicate Detection
- **Implemented in:** `consumerServer.cpp`
- **Methods:** `calculateHash()`, hash checking
- **Lines:** 80-120

### Queue Status Check (Bonus)
- **Implemented in:** `producerThread_enhanced.cpp`
- **Methods:** `checkQueueStatus()`
- **Lines:** 95-115

### Web GUI
- **Implemented in:** `web/index.html`
- **Features:** Dashboard, preview, playback
- **Lines:** 1-600

## 📋 Compilation Dependencies

### Required by Each Component

**consumerServer.cpp:**
- gRPC++
- Protocol Buffers
- OpenSSL (for SHA-256)
- C++ standard library (threads, queue, mutex)

**producerThread_enhanced.cpp:**
- gRPC++
- Protocol Buffers
- C++ filesystem
- C++ standard library (threads, random)

**webServer.cpp:**
- POSIX sockets
- C++ threads
- C++ standard library (fstream, sstream)

## 🔧 Modifying the System

### To change queue behavior:
→ Edit `consumerServer.cpp`, `UploadVideo()` method

### To change upload logic:
→ Edit `producerThread_enhanced.cpp`, `uploadVideo()` method

### To change GUI appearance:
→ Edit `web/index.html`, `<style>` section

### To add new gRPC methods:
→ Edit `media_service.proto`, rebuild with protoc

### To add new statistics:
→ Edit `consumerServer.h` (add fields), `consumerServer.cpp` (update logic), `web/index.html` (display)

## 📦 Build Artifacts

After running `./setup.sh` and building:

```
build/
├── consumer_server          # Server executable
├── producer_client          # Producer executable
├── media_service.pb.h       # Generated protobuf header
├── media_service.pb.cc      # Generated protobuf source
├── media_service.grpc.pb.h  # Generated gRPC header
├── media_service.grpc.pb.cc # Generated gRPC source
└── web/                     # Copied web files
    └── index.html
```

## 🧪 Testing Files

### Test with these:
```
video_files/
├── producer_1/test1.mp4  # Generated by setup.sh
├── producer_2/test2.mp4  # Generated by setup.sh
└── producer_3/test3.mp4  # Generated by setup.sh
```

### Output goes here:
```
uploaded_videos/
├── VID_*.mp4             # Uploaded videos
└── thumbnails/           # Video thumbnails
```

## 🔍 Quick File Search

**Looking for...**

- **Command-line parsing?** → `serverMain.cpp` & `producerMain.cpp`
- **Queue operations?** → `consumerServer.cpp` (lines 45-180)
- **File streaming?** → `producerThread_enhanced.cpp` (lines 120-200)
- **Web API?** → `webServer.cpp` (lines 80-150)
- **Statistics?** → `consumerServer.cpp` (lines 220-250)
- **GUI layout?** → `web/index.html` (lines 1-350)
- **JavaScript logic?** → `web/index.html` (lines 500-600)
- **Build configuration?** → `CMakeLists.txt`
- **Setup automation?** → `setup.sh`

## 📞 Support Reference

**Build issues?** → Check `CMakeLists.txt` and `setup.sh`
**Runtime errors?** → Check `serverMain.cpp` and `producerMain.cpp`
**Network issues?** → Check `media_service.proto` and gRPC code
**GUI issues?** → Check `web/index.html` and `webServer.cpp`
**Logic issues?** → Check `consumerServer.cpp` and `producerThread_enhanced.cpp`

---

## 📊 File Statistics

- **Total Source Files:** 15
- **Total Lines of Code:** ~2,500
- **Total Documentation:** ~2,000 lines
- **Languages:** C++, Protocol Buffers, HTML/CSS/JavaScript, Bash
- **Dependencies:** gRPC, Protobuf, OpenSSL, C++20 standard library

---

**All files are in `/mnt/user-data/outputs/`**
