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

### Building the Game
Clone the repository and run the build script:

```bash
git clone https://github.com/PandiaJason/amphitude.git
cd amphitude
./build.sh
```

This will compile the source code and generate an executable named `game`.

### Running the Game
```bash
./game
```

## 🕹️ How to Play

### Controls
| Action | Player 1 (Host/Local) | Player 2 (Client/Local) |
| :--- | :--- | :--- |
| **Move** | W, A, S, D | Arrow Keys |
| **Jump** | W | Up Arrow |
| **Attack** | F | Enter |
| **Pause** | Escape | Escape |
| **Forfeit** | Y (in Pause Menu) | Y (in Pause Menu) |

### Online Multiplayer
1.  **Host**: Select **Host (H)** in the main menu. Wait for a player to join.
    - Press **Escape** to cancel and return to the main menu.
2.  **Join**: Select **Join (J)**. Enter the Host's IP address (default `127.0.0.1` for local testing) and press Enter.
    - Press **Escape** to cancel and return to the main menu.
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
