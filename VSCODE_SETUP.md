# 💻 Media Upload Service - VS Code Setup (Windows)

## ✅ Yes, You Can Use VS Code!

Visual Studio Code works great for this project! Here's how to set it up.

---

## 📋 Prerequisites

### 1. Install Required Software

**VS Code:**
- Download: https://code.visualstudio.com/
- Install the following extensions:
  - C/C++ (Microsoft)
  - CMake Tools (Microsoft)
  - CMake (twxs)

**Build Tools:**
Choose ONE of these options:

#### Option A: MinGW-w64 (Lightweight, Recommended)
```powershell
# Install via chocolatey
choco install mingw

# Or download directly:
# https://github.com/niXman/mingw-builds-binaries/releases
# Extract to C:\mingw64
# Add C:\mingw64\bin to PATH
```

#### Option B: Visual Studio Build Tools (If you want full compatibility)
- Download: https://visualstudio.microsoft.com/downloads/
- Select: "Build Tools for Visual Studio 2022"
- Install: "C++ build tools"

**vcpkg (Package Manager):**
```powershell
cd C:\
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat

# Add to PATH
setx VCPKG_ROOT "C:\vcpkg"
```

**CMake:**
```powershell
# Via chocolatey
choco install cmake

# Or download from:
# https://cmake.org/download/
```

**Git:**
```powershell
choco install git
# Or: https://git-scm.com/download/win
```

---

## 🚀 Setup with VS Code

### Step 1: Extract Project

Extract `MediaUploadService_Windows.zip` to a folder, e.g.:
```
C:\Users\YourName\MediaUploadService
```

### Step 2: Open in VS Code

```powershell
cd C:\Users\YourName\MediaUploadService
code .
```

### Step 3: Install Dependencies via vcpkg

Open VS Code terminal (Ctrl+`) and run:

```powershell
# This takes 30-60 minutes on first run!
cd C:\vcpkg

.\vcpkg install grpc:x64-windows
.\vcpkg install protobuf:x64-windows
.\vcpkg install openssl:x64-windows

# Integrate with CMake
.\vcpkg integrate install
```

### Step 4: Configure CMake in VS Code

1. Press `Ctrl+Shift+P`
2. Type: "CMake: Configure"
3. Select: Your compiler (MinGW or MSVC)

VS Code will automatically:
- Detect vcpkg
- Configure the project
- Generate build files

### Step 5: Build the Project

**Method 1: Using VS Code UI**
- Press `F7` or
- Click "Build" in status bar
- Or: `Ctrl+Shift+P` → "CMake: Build"

**Method 2: Using Terminal**
```powershell
cd build
cmake --build . --config Release
```

Executables will be in: `build/Release/`

---

## 🎮 Running in VS Code

### Method 1: Using Integrated Terminal

**Terminal 1 (Server):**
```powershell
cd build/Release
./consumer_server.exe -c 4 -q 10
```

**Terminal 2 (Producer):**
```powershell
# Split terminal: Ctrl+Shift+5
cd build/Release
./producer_client.exe -p 3
```

**Browser:**
```
http://localhost:8080
```

### Method 2: Using VS Code Tasks

Create `.vscode/tasks.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Run Server",
            "type": "shell",
            "command": "${workspaceFolder}/build/Release/consumer_server.exe",
            "args": ["-c", "4", "-q", "10"],
            "problemMatcher": [],
            "group": "none"
        },
        {
            "label": "Run Producer",
            "type": "shell",
            "command": "${workspaceFolder}/build/Release/producer_client.exe",
            "args": ["-p", "3"],
            "problemMatcher": [],
            "group": "none"
        }
    ]
}
```

Run with: `Ctrl+Shift+P` → "Tasks: Run Task"

### Method 3: Using VS Code Launch Configuration

Create `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Release/consumer_server.exe",
            "args": ["-c", "4", "-q", "10"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/Release",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        },
        {
            "name": "Producer",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Release/producer_client.exe",
            "args": ["-p", "3"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/Release",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

Run with: `F5` or Debug panel

---

## 📁 VS Code Workspace Settings

Create `.vscode/settings.json`:
```json
{
    "cmake.configureSettings": {
        "CMAKE_TOOLCHAIN_FILE": "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
    },
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "files.associations": {
        "*.h": "cpp",
        "*.cpp": "cpp"
    }
}
```

---

## 🔧 Complete VS Code Configuration

Create all three files in `.vscode/` folder:

### 1. settings.json
```json
{
    "cmake.configureSettings": {
        "CMAKE_TOOLCHAIN_FILE": "C:/vcpkg/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_GENERATOR": "MinGW Makefiles"
    },
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "MinGW Makefiles",
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "terminal.integrated.defaultProfile.windows": "PowerShell"
}
```

### 2. tasks.json
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure",
            "type": "shell",
            "command": "cmake",
            "args": [
                "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake",
                "-G", "MinGW Makefiles",
                ".."
            ],
            "options": {
                "cwd": "${workspaceFolder}/build"
            },
            "problemMatcher": []
        },
        {
            "label": "CMake Build",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", ".", "--config", "Release"],
            "options": {
                "cwd": "${workspaceFolder}/build"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Run Server",
            "type": "shell",
            "command": "${workspaceFolder}/build/Release/consumer_server.exe -c 4 -q 10",
            "problemMatcher": []
        },
        {
            "label": "Run Producer",
            "type": "shell",
            "command": "${workspaceFolder}/build/Release/producer_client.exe -p 3",
            "problemMatcher": []
        }
    ]
}
```

### 3. launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Release/consumer_server.exe",
            "args": ["-c", "4", "-q", "10"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/Release",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe"
        }
    ]
}
```

---

## 🎯 Quick Commands in VS Code

| Action | Shortcut |
|--------|----------|
| Open terminal | `Ctrl + `` |
| Split terminal | `Ctrl + Shift + 5` |
| Command palette | `Ctrl + Shift + P` |
| Build | `F7` or `Ctrl + Shift + B` |
| Debug | `F5` |
| Find in files | `Ctrl + Shift + F` |

---

## 🐛 Troubleshooting in VS Code

### Issue: CMake not found

**Solution:**
```powershell
choco install cmake
# Restart VS Code
```

### Issue: Compiler not found

**Solution:**
```powershell
# For MinGW
where gcc
# Should show: C:\mingw64\bin\gcc.exe

# If not found, add to PATH:
# C:\mingw64\bin
```

### Issue: vcpkg packages not found

**Solution:**
```powershell
# In VS Code terminal
cd C:\vcpkg
.\vcpkg integrate install

# Then in VS Code: Ctrl+Shift+P → "CMake: Delete Cache and Reconfigure"
```

### Issue: IntelliSense not working

**Solution:**
1. `Ctrl+Shift+P` → "C/C++: Edit Configurations (UI)"
2. Set "Configuration provider" to "CMake Tools"
3. Set "Compiler path" to your gcc.exe location

### Issue: Cannot debug

**Solution:**
Install gdb:
```powershell
# Included with MinGW
# Verify with:
where gdb
# Should show: C:\mingw64\bin\gdb.exe
```

---

## 📂 Project Structure in VS Code

```
MediaUploadService/
├── .vscode/              ← VS Code configuration
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── build/                ← Build output
├── include/              ← Headers
├── src/                  ← Source files
├── web/                  ← Web GUI
├── video_files/          ← Input
└── CMakeLists.txt
```

---

## ⚡ Automated Setup Script for VS Code

Create `setup_vscode.bat`:

```batch
@echo off
echo Setting up for VS Code...

REM Create .vscode directory
if not exist ".vscode" mkdir .vscode

REM Create build directory
if not exist "build" mkdir build

REM Configure CMake
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"

REM Build
cmake --build . --config Release

cd ..

echo Setup complete! Open in VS Code with: code .
pause
```

Run it:
```powershell
.\setup_vscode.bat
```

---

## 🎨 Recommended VS Code Extensions

**Essential:**
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- CMake (twxs)

**Helpful:**
- GitLens
- Error Lens
- Better Comments
- Bracket Pair Colorizer
- Code Spell Checker

Install all at once:
```powershell
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake
```

---

## 🎓 VS Code Workflow

### Daily Development

1. **Open project:**
   ```powershell
   code C:\Users\YourName\MediaUploadService
   ```

2. **Build:** `Ctrl + Shift + B`

3. **Run in two terminals:**
   - Terminal 1: Server
   - Terminal 2: Producer

4. **Code changes:** Edit → Save → Auto-rebuild (if configured)

5. **Debug:** Set breakpoint → `F5`

---

## ✅ Advantages of VS Code

- ✅ Lightweight and fast
- ✅ Free and open source
- ✅ Great CMake integration
- ✅ Excellent Git support
- ✅ Customizable
- ✅ Cross-platform (same setup on Linux!)
- ✅ Modern UI
- ✅ Active extensions ecosystem

---

## 🎉 You're Ready!

With VS Code, you get:
- Full C++ development environment
- CMake integration
- Debugging support
- Git integration
- Terminal access
- Everything you need!

**VS Code is perfect for this project!** 💻🚀

---

## 📞 Quick Reference

**Install dependencies:**
```powershell
C:\vcpkg\vcpkg install grpc:x64-windows protobuf:x64-windows openssl:x64-windows
```

**Configure & Build:**
```powershell
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"
cmake --build . --config Release
```

**Run:**
```powershell
# Terminal 1
cd build/Release
./consumer_server.exe -c 4 -q 10

# Terminal 2
cd build/Release
./producer_client.exe -p 3
```

**Happy coding in VS Code!** 💻✨
