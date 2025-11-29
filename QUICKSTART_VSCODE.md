# 🚀 QUICK START - VS Code on Windows

## For VS Code Users - Start Here!

### ⚡ 5-Minute Setup

#### 1. Install Software (One Time)

**VS Code:**
```
Download: https://code.visualstudio.com/
Install extensions: C/C++, CMake Tools
```

**MinGW (Compiler):**
```powershell
choco install mingw
# Or download: https://github.com/niXman/mingw-builds-binaries/releases
# Extract to C:\mingw64, add C:\mingw64\bin to PATH
```

**vcpkg (Package Manager):**
```powershell
cd C:\
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
setx VCPKG_ROOT "C:\vcpkg"
```

#### 2. Install Dependencies (30-60 mins first time)

```powershell
cd C:\vcpkg
.\vcpkg install grpc:x64-windows protobuf:x64-windows openssl:x64-windows
```

#### 3. Open Project in VS Code

```powershell
cd C:\Users\YourName\MediaUploadService
code .
```

#### 4. Build

Press `Ctrl+Shift+B` in VS Code

Or in terminal:
```powershell
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"
cmake --build . --config Release
```

#### 5. Run

**Terminal 1 (Ctrl+`):**
```powershell
cd build/Release
./consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Split: Ctrl+Shift+5):**
```powershell
cd build/Release
./producer_client.exe -p 3
```

**Browser:**
```
http://localhost:8080
```

---

## 📚 Full VS Code Guide

See **VSCODE_SETUP.md** for:
- Complete configuration
- Debugging setup
- Tasks and launch configs
- Troubleshooting
- Extensions

---

## 🎯 VS Code vs Visual Studio

**Use VS Code if you want:**
- ✅ Lightweight editor
- ✅ Fast startup
- ✅ Customizable
- ✅ Free and open source
- ✅ Same setup works on Linux

**Use Visual Studio if you want:**
- Full-featured IDE
- Visual debugger
- GUI CMake configuration
- Official Microsoft support

**Both work great for this project!**

---

## ✨ Why VS Code is Perfect for This

- CMake integration works flawlessly
- Built-in terminal for running server/client
- Great C++ IntelliSense
- Debugging with gdb
- Git integration
- Lightweight and fast

---

**Next:** Read `VSCODE_SETUP.md` for complete guide! 💻
