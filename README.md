# amphitude

**amphitude** is a high-octane, 2D multiplayer platform fighter built from scratch with C++ and SDL2. It blends retro pixel-art aesthetics with modern, fast-paced combat mechanics.

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

## 🛠️ Installation & Build Guide

### ⚡ Quick Start (All Platforms)
1.  **Install Dependencies** (see below).
2.  **Build**:
    ```bash
    ./build.sh
    ```
3.  **Run**:
    ```bash
    ./server_app  # Terminal 1
    ./amphitude   # Terminal 2
    ```

### 📦 Dependency Installation

#### 🍎 macOS
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_net
```

#### 🐧 Linux (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev
```

#### 🪟 Windows (MinGW)
Install **SDL2** libraries via **pacman** in MSYS2 or MinGW.

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
*   **S**: Set Server IP (For VPN/LAN)
*   **Esc**: Quit / Back

---

## 🌐 Online Play Guide

Amphitude uses a **Signaling Server** to connect players. You can play on a **Local Network (LAN)** or over the **Internet** using a free VPN like ZeroTier.

### 🏠 Option 1: Local Network (LAN) / Same WiFi
If you and your friend are on the same WiFi:
1.  **Run Server**: `./server_app` on one computer.
2.  **Host**: Run `./amphitude`, press **S**, enter the **Local IP** of the server computer (e.g., `192.168.1.5`), then press **H**.
3.  **Join**: Run `./amphitude`, press **S**, enter the same **Local IP**, then press **J** and enter the code.

### 🌍 Option 2: Internet Play (ZeroTier VPN)
We recommend **ZeroTier** for free, easy online play anywhere in the world.

#### 1. Setup ZeroTier 🛡️
1.  **Create a Network**: Go to [ZeroTier Central](https://my.zerotier.com/), create an account, and create a "Network". Copy the **Network ID**.
2.  **Install App**: Download and install the **ZeroTier One** app on both computers.
3.  **Join Network**:
    *   **Option A (App)**: Open the app, click "Join Network", and paste the **Network ID**.
    *   **Option B (Terminal)**:
        ```bash
        sudo zerotier-cli join <NETWORK_ID>
        ```
4.  **Authorize Members** (Crucial Step!):
    *   **Find Your Device ID**:
        *   **App**: Click the ZeroTier icon in the menu bar.
        *   **Terminal**: Run `sudo zerotier-cli info` (The 10-character code is your ID).
    *   Go back to the ZeroTier website (Network settings).
    *   Scroll down to the "Members" section.
    *   **Check the box** under "Auth?" for your device ID.
    *   Wait a moment until you see a **Managed IP** (e.g., `10.147.x.x`).

#### 2. Play the Game 🎮
1.  **Host**:
    *   Run `./server_app` and `./amphitude`.
    *   In the Game Menu, press **S** and type your **ZeroTier Managed IP**.
    *   Press **H** to Host.
    *   Share the **Secret Code** with your friend.
2.  **Client**:
    *   Run `./amphitude`.
    *   Press **S** and type the **Host's ZeroTier Managed IP**.
    *   Press **J** and enter the Secret Code.

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

This project is open-source and available under the **GNU General Public License v2.0 (GPLv2)**.

**Copyright (c) 2025 Amphitainments**

Feel free to use, modify, and distribute it under the terms of the license.
