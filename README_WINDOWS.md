# 🪟 Media Upload Service - WINDOWS VERSION

## ⚠️ IMPORTANT - READ THIS FIRST

This is the **Windows version** of the Media Upload Service. 

**Start with:** `WINDOWS_SETUP.md`

---

## 📦 What's Different from Linux Version

✅ **Windows Sockets (Winsock)** - Uses Windows network APIs  
✅ **Visual Studio Support** - Build with VS 2019/2022  
✅ **vcpkg Integration** - Easy dependency management  
✅ **Batch Scripts** - `.bat` files for Windows  
✅ **Path Separators** - Uses `\` instead of `/`  

---

## 🚀 Quick Start (3 Steps)

### 1. Install Prerequisites

- **Visual Studio 2019/2022** with C++ Desktop Development
- **vcpkg** package manager
- Run: `setup.bat`

### 2. Start Server (CMD or PowerShell)

```cmd
cd build\Release
consumer_server.exe -c 4 -q 10
```

### 3. Start Producers (Another Terminal)

```cmd
cd build\Release
producer_client.exe -p 3
```

### 4. Open Browser

```
http://localhost:8080
```

---

## 📚 Documentation Files

| File | Description |
|------|-------------|
| **WINDOWS_SETUP.md** | ⭐ **START HERE** - Complete Windows setup guide |
| START_HERE.md | Project overview (cross-platform) |
| QUICK_START.md | Command reference |
| README.md | Full documentation |
| IMPLEMENTATION_SUMMARY.md | Technical details |

---

## 🔧 Requirements

### Required
- Windows 10/11
- **Choose ONE:**
  - **Visual Studio Code** (Recommended - Lightweight) ⭐
    - See: `VSCODE_SETUP.md`
  - **Visual Studio 2019/2022** (Full IDE)
    - Desktop development with C++
    - C++ CMake tools
- vcpkg (C:\vcpkg)
- 10 GB free space (for dependencies)

### Optional
- Git for Windows
- FFmpeg (for test videos)

---

## 📁 Project Structure

```
MediaUploadService\
├── build\                    # Build directory (after building)
│   └── Release\
│       ├── consumer_server.exe
│       ├── producer_client.exe
│       └── web\
│           └── index.html
│
├── include\                  # Header files
├── src\                      # Source code
├── web\                      # Web GUI
├── video_files\              # Input videos
│   ├── producer_1\
│   ├── producer_2\
│   └── producer_3\
│
├── uploaded_videos\          # Output
│   └── thumbnails\
│
├── CMakeLists.txt            # Build configuration (Windows-compatible)
├── setup.bat                 # Windows setup script
└── WINDOWS_SETUP.md          # Windows installation guide
```

---

## ⚡ First Time Setup

```cmd
REM 1. Install vcpkg
cd C:\
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat

REM 2. Add to PATH
setx VCPKG_ROOT "C:\vcpkg"

REM 3. Run setup
cd C:\MediaUploadService
setup.bat
```

---

## 🎯 Running on Windows

### Using Command Prompt

**Terminal 1 (Server):**
```cmd
cd C:\MediaUploadService\build\Release
consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Client):**
```cmd
cd C:\MediaUploadService\build\Release
producer_client.exe -p 3
```

### Using PowerShell

**Terminal 1 (Server):**
```powershell
cd C:\MediaUploadService\build\Release
.\consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Client):**
```powershell
cd C:\MediaUploadService\build\Release
.\producer_client.exe -p 3
```

---

## 🔥 Windows Firewall

Allow ports 50051 and 8080:

```powershell
# Run as Administrator
New-NetFirewallRule -DisplayName "Media Upload gRPC" -Direction Inbound -Protocol TCP -LocalPort 50051 -Action Allow
New-NetFirewallRule -DisplayName "Media Upload Web" -Direction Inbound -Protocol TCP -LocalPort 8080 -Action Allow
```

---

## 🐛 Common Windows Issues

### Issue: vcpkg not found

**Solution:**
```cmd
where vcpkg
REM If not found, add C:\vcpkg to PATH
```

### Issue: Missing DLLs

**Solution:**
```cmd
copy C:\vcpkg\installed\x64-windows\bin\*.dll build\Release\
```

### Issue: Port in use

**Solution:**
```cmd
netstat -ano | findstr :50051
taskkill /PID <PID> /F
```

See `WINDOWS_SETUP.md` for complete troubleshooting guide.

---

## ✨ All Features Work on Windows

- ✅ Producer threads
- ✅ Consumer threads  
- ✅ Queue management
- ✅ gRPC streaming
- ✅ Web GUI
- ✅ Video preview
- ✅ Video playback
- ✅ Duplicate detection
- ✅ Queue status check

---

## 📞 Need Help?

1. Read **WINDOWS_SETUP.md** for detailed setup
2. Check build output for errors
3. Verify vcpkg installed correctly
4. Ensure firewall allows ports
5. Try rebuilding from scratch

---

## 🎓 For Your Assignment

This Windows version includes everything for your STDISCM assignment:

- **Parameter p** - Producer threads
- **Parameter c** - Consumer threads
- **Parameter q** - Queue capacity
- **Separate folders** - Per producer
- **Network communication** - gRPC
- **GUI** - Web dashboard
- **Video preview** - 10 seconds
- **Video playback** - Full length
- **BONUS:** Queue notification + Duplicate detection

---

**Perfect for Windows development and deployment!** 🪟🚀

For detailed setup instructions, see **WINDOWS_SETUP.md**
