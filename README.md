# Amphitude ⚔️

**Amphitude** is a high-octane, 2D multiplayer platform fighter built from scratch with C++ and SDL2. It blends retro pixel-art aesthetics with modern, fast-paced combat mechanics.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey.svg)

---

## ✨ Features

### 🎮 Gameplay
*   **Intense PvP Combat**: Battle your friends in 1v1 duels.
*   **Local & Online**: Play on the same keyboard or connect over the internet.
*   **Unique Characters**:
    *   **Xeno**: A Playable Character -> Male.
    *   **Zeni**: A Playable Character -> Female.
*   **Dynamic Physics**: Master double jumps, wall jumps, and momentum-based movement.
*   **Power-Ups**: Turn the tide with random spawns like **Health Packs**, **Dragon Suit which can fire**, and **Rhino Suit which can 2x damage**.

### 🛠️ Technical
*   **Custom Engine**: Built on top of SDL2 for maximum performance and control.
*   **Authoritative Networking**: Host-Client architecture ensures fair play and synchronization.
*   **Cross-Platform**: Runs natively on **macOS** and **Linux** (Debian/Ubuntu/Arch).

---

## 🚀 Installation Guide

### 🍎 macOS
1.  **Install Dependencies** (via [Homebrew](https://brew.sh/)):
    ```bash
    brew install cmake sdl2 sdl2_image sdl2_ttf sdl2_net
    ```
2.  **Clone & Build**:
    ```bash
    git clone https://github.com/PandiaJason/amphitude.git
    cd amphitude
    mkdir build && cd build
    cmake ..
    make
    ```
3.  **Run**:
    ```bash
    ./amphitude
    ```

### 🐧 Linux (Debian/Ubuntu/Mint)
1.  **Install Dependencies**:
    ```bash
    sudo apt update
    sudo apt install build-essential cmake git libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev
    ```
2.  **Clone & Build**:
    ```bash
    git clone https://github.com/PandiaJason/amphitude.git
    cd amphitude
    mkdir build && cd build
    cmake ..
    make
    ```
3.  **Run**:
    ```bash
    ./amphitude
    ```

### 🐧 Linux (Arch/Manjaro)
1.  **Install Dependencies**:
    ```bash
    sudo pacman -S base-devel cmake git sdl2 sdl2_image sdl2_ttf sdl2_net
    ```
2.  **Clone & Build**:
    ```bash
    git clone https://github.com/PandiaJason/amphitude.git
    cd amphitude
    mkdir build && cd build
    cmake ..
    make
    ```
3.  **Run**:
    ```bash
    ./amphitude
    ```

---

## 🕹️ Controls

| Action | Player 1 (Host/Local) | Player 2 (Client/Local) |
| :--- | :--- | :--- |
| **Move** | `W`, `A`, `S`, `D` | `Arrow Keys` |
| **Jump** | `W` (Double Jump) | `Up Arrow` (Double Jump) |
| **Attack** | `Space` | `Enter` |
| **Ability** | `E` | `/` |
| **Pause** | `Esc` | `Esc` |

**Menu Shortcuts:**
*   **H**: Host Online Game
*   **J**: Join Online Game
*   **L**: Start Local Game
*   **Esc**: Quit / Back

---

## 🌐 Online Play (Manual Port Forwarding)

Amphitude uses a direct peer-to-peer connection. To host a game online:

1.  **Port Forwarding**: Log into your router and forward **Port 12345 (TCP)** to your computer's local IP address.
2.  **Host**: Launch the game and press **H**.
3.  **Share IP**: Google "what is my ip" and send that Public IP to your friend.
4.  **Join**: Your friend launches the game, presses **J**, enters your Public IP, and presses **Enter**.

*Note: If you cannot port forward, you can use a VPN service like Hamachi or ZeroTier to create a virtual LAN.*

---

## 🤝 Contributing

We welcome contributions from the community!

1.  **Fork** the repository.
2.  Create a **Feature Branch** (`git checkout -b feature/NewAbility`).
3.  **Commit** your changes.
4.  **Push** to the branch.
5.  Open a **Pull Request**.

---

## 📜 License

This project is open-source and available under the **MIT License**. Feel free to use, modify, and distribute it.
