@echo off
echo ========================================================
echo   Media Upload Service - Windows Setup
echo ========================================================
echo.

REM Check if vcpkg is installed
where vcpkg >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [!] vcpkg not found in PATH
    echo.
    echo You need to install vcpkg first:
    echo 1. Download: https://github.com/microsoft/vcpkg
    echo 2. Install: bootstrap-vcpkg.bat
    echo 3. Add to PATH
    echo.
    pause
    exit /b 1
)

echo [*] vcpkg found!
echo.

REM Create directories
echo [*] Creating directories...
if not exist "build" mkdir build
if not exist "video_files\producer_1" mkdir video_files\producer_1
if not exist "video_files\producer_2" mkdir video_files\producer_2
if not exist "video_files\producer_3" mkdir video_files\producer_3
if not exist "uploaded_videos\thumbnails" mkdir uploaded_videos\thumbnails
echo [+] Directories created
echo.

REM Check for required packages
echo [*] Checking dependencies...
vcpkg list | findstr /C:"grpc" >nul
if %ERRORLEVEL% NEQ 0 (
    echo [!] gRPC not found. Installing dependencies...
    echo This may take 30-60 minutes...
    vcpkg install grpc:x64-windows
    vcpkg install protobuf:x64-windows
    vcpkg install openssl:x64-windows
) else (
    echo [+] gRPC found
)
echo.

REM Build project
echo [*] Building project...
cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -A x64
if %ERRORLEVEL% NEQ 0 (
    echo [!] CMake configuration failed
    cd ..
    pause
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [!] Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================================
echo   Setup Complete!
echo ========================================================
echo.
echo To run the system:
echo.
echo 1. Start server (in one terminal):
echo    cd build\Release
echo    consumer_server.exe -c 4 -q 10
echo.
echo 2. Start producers (in another terminal):
echo    cd build\Release
echo    producer_client.exe -p 3
echo.
echo 3. Open browser:
echo    http://localhost:8080
echo.
pause
