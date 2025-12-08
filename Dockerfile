# Use Debian Bullseye (Stable)
FROM debian:bullseye

# Install Dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libsdl2-net-dev \
    && rm -rf /var/lib/apt/lists/*

# Set Working Directory
WORKDIR /app

# Copy Source Code
COPY . .

# Build
RUN chmod +x build.sh && ./build.sh

# Run
CMD ["./amphitude"]
