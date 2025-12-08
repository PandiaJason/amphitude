#!/bin/bash

# Detect OS
OS="$(uname -s)"
echo "🖥️  Detected System: $OS"

# Default Compiler & Flags
CXX="g++"
CXXFLAGS="-std=c++17 -Iinclude"
LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net"
OUTPUT_EXT=""

# OS Specific Configuration
if [ "$OS" == "Darwin" ]; then
    # macOS
    echo "🍏 Configuring for macOS..."
    # Attempt to find Homebrew paths if standard link fails? 
    # Usually standard -lSDL2 works if brew is in path, but let's be safe.
    if [ -d "/opt/homebrew/include" ]; then
        CXXFLAGS="$CXXFLAGS -I/opt/homebrew/include"
        LIBS="-L/opt/homebrew/lib $LIBS"
    fi

elif [ "$OS" == "Linux" ]; then
    # Linux
    echo "🐧 Configuring for Linux..."
    # Use pkg-config if available for robust path detection
    if command -v pkg-config &> /dev/null; then
        CXXFLAGS="$CXXFLAGS $(pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_net)"
        LIBS="$(pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_net)"
    else
        echo "⚠️  pkg-config not found. Using default flags."
    fi

elif [[ "$OS" == MINGW* ]] || [[ "$OS" == CYGWIN* ]] || [[ "$OS" == MSYS* ]]; then
    # Windows (Git Bash / MSYS2)
    echo "🪟 Configuring for Windows (MinGW)..."
    LIBS="-lmingw32 -lSDL2main $LIBS"
    OUTPUT_EXT=".exe"
else
    echo "⚠️  Unknown OS: $OS. Trying generic build..."
fi

# Function to build a target
build_target() {
    NAME=$1
    SRC=$2
    echo "🔨 Building $NAME..."
    $CXX $CXXFLAGS $SRC -o $NAME$OUTPUT_EXT $LIBS
    
    if [ $? -eq 0 ]; then
        echo "✅ $NAME built successfully!"
    else
        echo "❌ Failed to build $NAME."
        echo "   Tip: Ensure you have installed SDL2 dependencies."
        if [ "$OS" == "Darwin" ]; then
            echo "   Run: brew install sdl2 sdl2_image sdl2_ttf sdl2_net"
        elif [ "$OS" == "Linux" ]; then
            echo "   Run: sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev"
        fi
        exit 1
    fi
}

# Build Game
build_target "amphitude" "src/*.cpp"

echo ""
echo "🎉 Build Complete!"
echo "👉 Run Game:   ./amphitude$OUTPUT_EXT"
