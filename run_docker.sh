#!/bin/bash

# Check for XQuartz
if ! command -v xquartz &> /dev/null; then
    if ! [ -d "/Applications/Utilities/XQuartz.app" ]; then
        echo "⚠️  XQuartz is not installed/found!"
        echo "   You need XQuartz to see the game GUI from Docker on Mac."
        echo "   Install it: brew install --cask xquartz"
        echo "   Then restart your Mac (or log out/in)."
        echo "   And enable 'Allow connections from network clients' in XQuartz settings."
    fi
fi

# Allow local connections to X11
xhost + 127.0.0.1

# Build the Image
echo "🐳 Building Docker Image..."
docker build -t amphitude-linux .

# Run the Container
echo "🚀 Running Amphitude (Linux) in Docker..."
echo "   (Make sure XQuartz is running!)"

# Network: host (shares network stack) to allow UDP hole punching to work easily on localhost?
# Actually, --net=host on Mac Docker is limited.
# But for P2P test, default bridge is fine if we can reach outside.
# DISPLAY: host.docker.internal:0 points to Mac's X Server.

docker run -it --rm \
    -p 50001:50000/udp \
    -e DISPLAY=host.docker.internal:0 \
    -e SDL_RENDER_DRIVER=software \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    amphitude-linux
