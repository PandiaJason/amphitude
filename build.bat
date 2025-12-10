@echo off
echo 🪟 Amphitude Windows Build Script
echo =================================

REM Check for g++
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ g++ compiler not found in PATH!
    echo    Please install MinGW-w64 and add bin folder to PATH.
    pause
    exit /b 1
)

echo 🔨 Building amphitude.exe...

REM Default Compiler Flags
REM We assume headers/libs are in standard search path OR environment variables
REM You might need to add -I"C:\SDL2\include" -L"C:\SDL2\lib" if not in standard path.

g++ -std=c++17 -Iinclude src/Game.cpp src/StunClient.cpp src/Utils.cpp src/Player.cpp src/main.cpp -o amphitude.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net

if %errorlevel% equ 0 (
    echo ✅ Build Successful!
    echo 👉 Run: amphitude.exe
) else (
    echo ❌ Build Failed.
    echo    Tip: Ensure SDL2 development libraries are installed and linked correctly.
    echo    You may need to edit this script to add include/lib paths: e.g., -I"C:\path\to\include"
)

pause
