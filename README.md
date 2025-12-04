# amphitude 

**amphitude** is an open-source, 2D multiplayer platformer battle game built with C++ and SDL2. It features real-time online multiplayer, pixel-art graphics, and fast-paced combat.

![License](https://img.shields.io/badge/license-GPLv2-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey.svg)

## 🎮 Features
- **Online Multiplayer**: Host and Join games over TCP/IP.
- **Real-Time Sync**: Full state synchronization (movement, attacks, projectiles, power-ups).
- **Lobby System**: Character selection, name input, and ready system.
- **Combat**: Melee attacks, fireballs, and special power-ups (Dragon, Rhino).
- **Robust Networking**: Non-blocking sockets with disconnect handling and forfeit systems.

## 🛠️ Installation & Build Guide

### Prerequisites
You need a C++ compiler (`clang` or `g++`) and the **SDL2** development libraries.

#### macOS (Homebrew)
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_net
```

#### Linux (Debian/Ubuntu)
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev
```

### Building the Game (CMake)
We use **CMake** for cross-platform builds.

**Prerequisites:**
-   CMake (3.14+)
-   SDL2, SDL2_image, SDL2_ttf, SDL2_net (Development libraries)

**Build Steps:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Creating Installers 📦
You can generate an installer for your OS (macOS .dmg, Windows .exe, Linux .deb) by running:
```bash
cd build
cpack
```

### Building the Signaling Server
The server is currently built separately:
```bash
cd server
./build.sh
```

### Running the Game
1.  **Start the Server** (in one terminal):
    ```bash
    cd server
    ./server_app
    ```
2.  **Start the Game** (in another terminal):
    ```bash
    ./game
    ```

## 🕹️ How to Play

2.  Host runs `server_app`.
3.  Game connects using the VPN IP.
3.  **Lobby**:
    - Press **T** to type your name.
    - Press **1** (P1) or **2** (P2) to switch characters.
    - Press **Enter** to toggle **Ready**.
    - Press **Escape** to disconnect and return to the main menu.
    - The game starts when both players are Ready.
4.  **Game Over**:
    - Press **Space** or **Escape** to return to the main menu.

## 👨‍💻 Developer Documentation

### Project Structure
```
amphitude/
├── src/                # Source files (.cpp)
│   ├── Game.cpp        # Main game loop, state management, rendering
│   ├── Player.cpp      # Player physics, collision, input
│   ├── Utils.cpp       # Helper functions
│   └── main.cpp        # Entry point
├── include/            # Header files (.h)
│   ├── Game.h          # Game class definition
│   ├── NetworkManager.h# Networking logic (Sockets, Packets)
│   ├── Player.h        # Player class definition
│   └── Constants.h     # Global constants (Physics, Screen size)
├── assets/             # Game assets (Images, Fonts)
└── build.sh            # Compilation script
```

### Key Systems

#### Networking (`NetworkManager.h`)
The game uses a **Host-Client** architecture.
- **Host**: Authoritative. Calculates physics, spawns power-ups, manages game state. Sends full state snapshots to Client.
- **Client**: Sends input to Host. Receives state snapshots and renders them.
- **Packets**: Defined in `NetworkManager.h`. Types include `Input` (1), `State` (2), and `Start` (3).
- **Sync**: The `Game::update` loop drains the socket buffer every frame to ensure the latest state is applied, minimizing latency.

#### Game Loop (`Game.cpp`)
Standard game loop:
1.  **Handle Events**: Input processing, network connection handling.
2.  **Update**: Physics integration, collision detection, network sync.
3.  **Render**: Drawing sprites and UI to the screen.

### Contributing
We welcome contributions! Please follow these steps:
1.  Fork the repository.
2.  Create a feature branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

## 📄 License
This project is licensed under the **GNU General Public License v2.0** - see the [LICENSE](LICENSE) file for details.
