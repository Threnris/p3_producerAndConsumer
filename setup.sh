#!/bin/bash

echo "╔════════════════════════════════════════════════╗"
echo "║  Media Upload Service - Setup Script          ║"
echo "╚════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    print_warning "This script is designed for Linux systems"
fi

echo "Step 1: Creating directory structure..."

# Create necessary directories
mkdir -p build
mkdir -p uploaded_videos
mkdir -p uploaded_videos/thumbnails
mkdir -p video_files/producer_1
mkdir -p video_files/producer_2
mkdir -p video_files/producer_3
mkdir -p web

print_status "Directory structure created"

echo ""
echo "Step 2: Checking dependencies..."

# Check for required tools
MISSING_DEPS=0

if ! command -v cmake &> /dev/null; then
    print_error "CMake not found"
    MISSING_DEPS=1
else
    print_status "CMake found: $(cmake --version | head -n1)"
fi

if ! command -v g++ &> /dev/null; then
    print_error "g++ not found"
    MISSING_DEPS=1
else
    print_status "g++ found: $(g++ --version | head -n1)"
fi

if ! command -v protoc &> /dev/null; then
    print_error "protoc not found"
    MISSING_DEPS=1
else
    print_status "protoc found: $(protoc --version)"
fi

# Check for libraries
if ! pkg-config --exists grpc++ 2>/dev/null; then
    print_error "gRPC++ not found"
    MISSING_DEPS=1
else
    print_status "gRPC++ found"
fi

if ! pkg-config --exists openssl 2>/dev/null; then
    print_error "OpenSSL not found"
    MISSING_DEPS=1
else
    print_status "OpenSSL found"
fi

if [ $MISSING_DEPS -eq 1 ]; then
    echo ""
    print_error "Missing dependencies detected!"
    echo ""
    echo "To install dependencies on Ubuntu/Debian, run:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install -y build-essential cmake libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc libssl-dev pkg-config"
    echo ""
    exit 1
fi

print_status "All dependencies found"

echo ""
echo "Step 3: Generating sample test videos (optional)..."

if command -v ffmpeg &> /dev/null; then
    print_status "FFmpeg found, generating test videos..."
    
    # Generate small test videos if they don't exist
    if [ ! -f "video_files/producer_1/test1.mp4" ]; then
        ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 -f lavfi -i sine=frequency=1000:duration=5 \
               -pix_fmt yuv420p -c:v libx264 -c:a aac video_files/producer_1/test1.mp4 -y 2>/dev/null
        print_status "Generated test1.mp4 for producer 1"
    fi
    
    if [ ! -f "video_files/producer_2/test2.mp4" ]; then
        ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 -f lavfi -i sine=frequency=1500:duration=5 \
               -pix_fmt yuv420p -c:v libx264 -c:a aac video_files/producer_2/test2.mp4 -y 2>/dev/null
        print_status "Generated test2.mp4 for producer 2"
    fi
    
    if [ ! -f "video_files/producer_3/test3.mp4" ]; then
        ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 -f lavfi -i sine=frequency=2000:duration=5 \
               -pix_fmt yuv420p -c:v libx264 -c:a aac video_files/producer_3/test3.mp4 -y 2>/dev/null
        print_status "Generated test3.mp4 for producer 3"
    fi
else
    print_warning "FFmpeg not found - skipping test video generation"
    print_warning "Please add your own video files to video_files/producer_*/ directories"
fi

echo ""
echo "Step 4: Building the project..."

cd build

if cmake .. ; then
    print_status "CMake configuration successful"
else
    print_error "CMake configuration failed"
    exit 1
fi

if make -j$(nproc) ; then
    print_status "Build successful"
else
    print_error "Build failed"
    exit 1
fi

cd ..

echo ""
echo "╔════════════════════════════════════════════════╗"
echo "║  Setup Complete!                               ║"
echo "╚════════════════════════════════════════════════╝"
echo ""
echo "To run the system:"
echo ""
echo "1. Start the consumer server:"
echo "   cd build"
echo "   ./consumer_server -c 4 -q 10"
echo ""
echo "2. In another terminal, start producers:"
echo "   cd build"
echo "   ./producer_client -p 3"
echo ""
echo "3. Open web browser:"
echo "   http://localhost:8080"
echo ""
print_status "Ready to run!"
