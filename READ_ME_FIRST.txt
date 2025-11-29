╔════════════════════════════════════════════════════════════╗
║                                                            ║
║       MEDIA UPLOAD SERVICE - Producer-Consumer System     ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝

📖 START HERE: EXTRACT_AND_RUN.md

This contains everything you need for your STDISCM assignment!

═══════════════════════════════════════════════════════════

QUICK START (3 STEPS):
════════════════════════

1. Run setup:
   $ ./setup.sh

2. Start server (Terminal 1):
   $ cd build
   $ ./consumer_server -c 4 -q 10

3. Start producers (Terminal 2):
   $ cd build
   $ ./producer_client -p 3

4. Open browser:
   http://localhost:8080

═══════════════════════════════════════════════════════════

FEATURES INCLUDED:
═══════════════════

✅ Producer threads (p parameter)
✅ Consumer threads (c parameter)
✅ Queue management with leaky bucket (q parameter)
✅ Each producer reads from separate folder
✅ gRPC communication for network streaming
✅ Web-based GUI dashboard
✅ 10-second video preview on hover
✅ Full video playback on click
✅ BONUS: Queue full notification to producers
✅ BONUS: Duplicate detection via SHA-256 hashing

═══════════════════════════════════════════════════════════

DOCUMENTATION:
═══════════════

📘 EXTRACT_AND_RUN.md       ⭐ Read this first!
📘 START_HERE.md            Complete overview
📘 QUICK_START.md           Command reference
📘 README.md                Full documentation
📘 IMPLEMENTATION_SUMMARY.md Technical details
📘 FILE_INDEX.md            File organization

═══════════════════════════════════════════════════════════

WHAT'S INSIDE:
═══════════════

📂 include/           Header files (.h)
📂 src/               Source code (.cpp)
📂 web/               Web GUI (HTML/JS)
📂 video_files/       Input videos (add your videos here!)
📂 uploaded_videos/   Output directory
📄 media_service.proto gRPC definitions
📄 CMakeLists.txt     Build configuration
📄 setup.sh           Automated setup

═══════════════════════════════════════════════════════════

SYSTEM REQUIREMENTS:
═══════════════════════

- Ubuntu/Debian Linux
- C++20 compiler (GCC 10+)
- CMake 3.15+
- gRPC and Protocol Buffers
- OpenSSL

Install with:
$ sudo apt-get install build-essential cmake libgrpc++-dev \
  libprotobuf-dev protobuf-compiler-grpc libssl-dev pkg-config

═══════════════════════════════════════════════════════════

PARAMETERS:
════════════

Server (Consumer):
  -c <num>  Consumer threads (required)
  -q <num>  Queue capacity (required)
  -p <port> gRPC port (default: 50051)
  -w <port> Web port (default: 8080)

Producer (Client):
  -p <num>  Producer threads (required)
  -s <addr> Server address (default: localhost:50051)

═══════════════════════════════════════════════════════════

NEED HELP?
═══════════

1. Check EXTRACT_AND_RUN.md for setup
2. Read START_HERE.md for overview
3. See QUICK_START.md for commands
4. Review README.md for full docs
5. Check console error messages

═══════════════════════════════════════════════════════════

Happy coding! 🚀

This is a complete, working implementation ready for your
distributed computing assignment!

═══════════════════════════════════════════════════════════
