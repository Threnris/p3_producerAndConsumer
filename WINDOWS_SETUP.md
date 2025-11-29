# 🪟 Media Upload Service - Windows Setup Guide

## 📋 Prerequisites

### Required Software

1. **Visual Studio 2019 or 2022** (Community Edition is free)
   - Download: https://visualstudio.microsoft.com/downloads/
   - During installation, select:
     - ✅ Desktop development with C++
     - ✅ C++ CMake tools for Windows

2. **vcpkg** (Package Manager)
   - Install vcpkg:
   ```powershell
   # In PowerShell (as Administrator)
   cd C:\
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

3. **Add vcpkg to PATH**
   - Open System Environment Variables
   - Add `C:\vcpkg` to PATH
   - Restart terminal

### Optional
- **Git for Windows** (if you don't have it)
- **FFmpeg** (for test video generation)

---

## 🚀 Quick Setup (Automated)

### Method 1: Using the Setup Script

1. **Extract the zip** to `C:\MediaUploadService\`

2. **Open PowerShell** (or CMD) as Administrator

3. **Navigate to project:**
   ```powershell
   cd C:\MediaUploadService
   ```

4. **Run setup:**
   ```cmd
   setup.bat
   ```

This will:
- ✅ Check for vcpkg
- ✅ Install gRPC, Protobuf, OpenSSL
- ✅ Create directories
- ✅ Build the project

⚠️ **First-time installation may take 30-60 minutes** as vcpkg compiles gRPC and dependencies.

---

## 🔧 Manual Setup (If Automated Fails)

### Step 1: Install Dependencies via vcpkg

```powershell
# Open PowerShell as Administrator
cd C:\vcpkg

# Install required packages (this takes time!)
.\vcpkg install grpc:x64-windows
.\vcpkg install protobuf:x64-windows
.\vcpkg install openssl:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install
```

### Step 2: Set Environment Variable

```powershell
# Add to system environment variables
$env:VCPKG_ROOT = "C:\vcpkg"
```

Or permanently:
- Right-click "This PC" → Properties → Advanced System Settings
- Environment Variables → System Variables → New
- Variable name: `VCPKG_ROOT`
- Variable value: `C:\vcpkg`

### Step 3: Create Directories

```powershell
cd C:\MediaUploadService

# Create directories
New-Item -ItemType Directory -Force -Path video_files\producer_1
New-Item -ItemType Directory -Force -Path video_files\producer_2
New-Item -ItemType Directory -Force -Path video_files\producer_3
New-Item -ItemType Directory -Force -Path uploaded_videos\thumbnails
```

### Step 4: Build with CMake

```powershell
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64

# Build
cmake --build . --config Release
```

Executables will be in: `build\Release\`

---

## 🎮 Running the System

### Option 1: Using Command Prompt

**Terminal 1 (Server):**
```cmd
cd C:\MediaUploadService\build\Release
consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Producers):**
```cmd
cd C:\MediaUploadService\build\Release
producer_client.exe -p 3
```

**Browser:**
```
http://localhost:8080
```

### Option 2: Using PowerShell

**Terminal 1 (Server):**
```powershell
cd C:\MediaUploadService\build\Release
.\consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Producers):**
```powershell
cd C:\MediaUploadService\build\Release
.\producer_client.exe -p 3
```

### Option 3: Using Visual Studio

1. Open `build\MediaUploadService.sln` in Visual Studio
2. Right-click `consumer_server` → Set as Startup Project
3. Right-click → Properties → Debugging → Command Arguments: `-c 4 -q 10`
4. Press F5 to run

Repeat for `producer_client` in another VS instance.

---

## 📹 Adding Test Videos

### Manual Method

1. **Copy video files** to the producer folders:
   ```cmd
   copy C:\path\to\your\video1.mp4 C:\MediaUploadService\video_files\producer_1\
   copy C:\path\to\your\video2.mp4 C:\MediaUploadService\video_files\producer_2\
   copy C:\path\to\your\video3.mp4 C:\MediaUploadService\video_files\producer_3\
   ```

### Using FFmpeg (if installed)

```powershell
# Install FFmpeg via chocolatey
choco install ffmpeg

# Generate test videos
ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 ^
       -pix_fmt yuv420p video_files\producer_1\test1.mp4

ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 ^
       -pix_fmt yuv420p video_files\producer_2\test2.mp4

ffmpeg -f lavfi -i testsrc=duration=5:size=640x480:rate=30 ^
       -pix_fmt yuv420p video_files\producer_3\test3.mp4
```

---

## 🔥 Firewall Configuration

Windows Firewall may block the ports. Allow them:

```powershell
# Open PowerShell as Administrator

# Allow gRPC port (50051)
New-NetFirewallRule -DisplayName "Media Upload gRPC" -Direction Inbound -Protocol TCP -LocalPort 50051 -Action Allow

# Allow Web GUI port (8080)
New-NetFirewallRule -DisplayName "Media Upload Web" -Direction Inbound -Protocol TCP -LocalPort 8080 -Action Allow
```

Or manually:
1. Windows Defender Firewall → Advanced Settings
2. Inbound Rules → New Rule
3. Port → TCP → 50051, 8080
4. Allow the connection

---

## 🐛 Troubleshooting

### Issue: vcpkg not found

**Solution:**
```powershell
# Check if vcpkg is in PATH
where.exe vcpkg

# If not found, add to PATH manually or use full path in CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
```

### Issue: gRPC not found during build

**Solution:**
```powershell
# Reinstall packages
cd C:\vcpkg
.\vcpkg remove grpc:x64-windows
.\vcpkg install grpc:x64-windows --recurse
.\vcpkg integrate install
```

### Issue: Missing DLLs when running

**Solution:**
```powershell
# Copy DLLs from vcpkg to build directory
copy C:\vcpkg\installed\x64-windows\bin\*.dll C:\MediaUploadService\build\Release\
```

Or add vcpkg bin to PATH:
```powershell
$env:PATH += ";C:\vcpkg\installed\x64-windows\bin"
```

### Issue: Port already in use

**Solution:**
```powershell
# Check what's using the port
netstat -ano | findstr :50051
netstat -ano | findstr :8080

# Kill the process (replace PID with actual process ID)
taskkill /PID <PID> /F

# Or use different ports
consumer_server.exe -c 4 -q 10 -p 50052 -w 8081
```

### Issue: Can't connect to server

**Solution:**
1. Check firewall (see Firewall Configuration above)
2. Check if server is running:
   ```powershell
   Get-Process consumer_server
   ```
3. Verify server address:
   ```powershell
   # On producer machine
   producer_client.exe -p 3 -s <SERVER_IP>:50051
   ```

### Issue: Web GUI not loading

**Solution:**
1. Check if `web` folder exists in `build\Release\`
2. Manually copy if missing:
   ```powershell
   xcopy /E /I web build\Release\web
   ```
3. Try different browser
4. Check browser console (F12) for errors

---

## 💻 Using Different Machines (Windows to Windows)

### On Server Machine (e.g., 192.168.1.100):

1. **Run server:**
   ```cmd
   consumer_server.exe -c 8 -q 20
   ```

2. **Configure firewall** (see Firewall Configuration above)

3. **Find IP address:**
   ```cmd
   ipconfig
   ```
   Look for IPv4 Address

### On Producer Machine(s):

```cmd
producer_client.exe -p 5 -s 192.168.1.100:50051
```

### Access Web GUI:

From any machine on the network:
```
http://192.168.1.100:8080
```

---

## 📊 Command Reference

### Server (consumer_server.exe)

```cmd
consumer_server.exe -c <consumers> -q <queue_size> [options]

Options:
  -c NUM    Consumer threads (1-100)
  -q NUM    Queue capacity (1-1000)
  -p PORT   gRPC port (default: 50051)
  -w PORT   Web port (default: 8080)
  -o DIR    Output directory (default: .\uploaded_videos)
```

### Producer (producer_client.exe)

```cmd
producer_client.exe -p <producers> [options]

Options:
  -p NUM    Producer threads (1-50) [REQUIRED]
  -s ADDR   Server address (default: localhost:50051)
  -i DIR    Input directory (default: .\video_files)
```

---

## 🎓 Visual Studio Tips

### Debugging

1. Set breakpoints in Visual Studio
2. Right-click project → Properties → Debugging
3. Set Command Arguments: `-c 4 -q 10`
4. Press F5 to debug

### Building in Visual Studio

1. Open `build\MediaUploadService.sln`
2. Build → Build Solution (Ctrl+Shift+B)
3. Executables in `build\Release\` or `build\Debug\`

### Running Multiple Instances

1. Tools → Options → Projects and Solutions → General
2. Uncheck "Allow only one instance of the Visual Studio"
3. Open solution twice
4. Run server in one, producer in another

---

## 📁 Expected Directory Structure

```
C:\MediaUploadService\
├── build\
│   └── Release\
│       ├── consumer_server.exe     ← Server executable
│       ├── producer_client.exe     ← Producer executable
│       ├── web\
│       │   └── index.html
│       └── *.dll                   ← gRPC/Protobuf DLLs
│
├── video_files\
│   ├── producer_1\
│   │   └── video1.mp4
│   ├── producer_2\
│   │   └── video2.mp4
│   └── producer_3\
│       └── video3.mp4
│
├── uploaded_videos\
│   └── thumbnails\
│
├── include\                        ← Header files
├── src\                            ← Source files
├── web\                            ← Web GUI
├── CMakeLists.txt
└── setup.bat
```

---

## ✅ Quick Checklist

Before running:
- [ ] Visual Studio installed with C++ tools
- [ ] vcpkg installed and in PATH
- [ ] Dependencies installed via vcpkg
- [ ] Project built successfully
- [ ] Video files added to producer folders
- [ ] Firewall configured
- [ ] Both executables in `build\Release\`

---

## 🆘 Still Having Issues?

1. **Check build output** for specific errors
2. **Verify all dependencies** installed via vcpkg
3. **Try rebuilding** from scratch:
   ```powershell
   Remove-Item -Recurse -Force build
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
   cmake --build . --config Release
   ```
4. **Check documentation** in README.md
5. **Review error messages** carefully

---

## 🎉 You're Ready!

Once setup is complete, you can run the system and access the web GUI at `http://localhost:8080`!

**Windows-specific notes:**
- Use `\` instead of `/` in paths
- Use `.exe` when running executables
- Run terminals as Administrator if permission issues occur
- DLLs must be in same directory as executables

**Happy coding on Windows! 🪟🚀**
