# amphitude

**amphitude** is a high-octane, **serverless P2P** 2D multiplayer platform fighter built from scratch with C++ and SDL2. It blends retro pixel-art aesthetics with modern, fast-paced combat mechanics.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey.svg)
![Network](https://img.shields.io/badge/network-Serverless%20P2P-green.svg)

---

## ✨ Features

### 🎮 Gameplay
*   **Intense PvP Combat**: Battle your friends in 1v1 duels.
*   **True P2P Multiplayer**: No servers, no accounts, no lag. Connect directly via UDP Hole Punching.
*   **Unique Characters**:
    *   **Xeno**: A Playable Character -> Male.
    *   **Zeni**: A Playable Character -> Female.
*   **Dynamic Physics**: Master double jumps, wall jumps, and momentum-based movement.
*   **Power-Ups**: Turn the tide with random spawns like **Health Packs**, **Dragon Suit** (fireball), and **Rhino Suit** (2x damage).
*   **Lobby System**: Countdown timer (3... 2... 1... GO!) ensures fair starts.

### 🛠️ Technical
*   **Custom Engine**: Built on top of SDL2 for maximum performance.
*   **UDP Networking**: Custom reliable UDP protocol with NAT Hole Punching (STUN).
*   **Serverless**: Zero backend infrastructure required.

---

## 🛠️ Installation & Build Guide

### ⚡ Quick Start
1.  **Install Dependencies** (see below).
2.  **Run Build Script**: `./build.sh`
3.  **Play**: `./amphitude`

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

---

## 🌐 How to Play Online (Covai <-> Chennai Protocol)

Amphitude uses **Serverless P2P** technology to connect players directly over the internet without a central server.

### 1. Host (Player A)
1.  Run `./amphitude` and press **H** (Host).
2.  You will see **Your Join Code** (e.g., `123.45.67.89:50000`).
3.  **Send this code** to your friend via WhatsApp/Telegram.
4.  **Wait** for your friend's code.

### 2. Client (Player B)
1.  Run `./amphitude` and press **J** (Join).
2.  You will see **Your Join Code**.
3.  **Send this code** to your friend.
4.  Enter the **Host's Code** and wait.

### 3. Connect
1.  Both players enter the **Friend's Code**.
2.  Both players press **ENTER**.
3.  The game will punch through firewalls and connect! 🥊

---

## 🕹️ Controls

| Action | Player 1 (Host/Local) | Player 2 (Client/Local) |
| :--- | :--- | :--- |
| **Move** | `W`, `A`, `S`, `D` | `Arrow Keys` |
| **Jump** | `W` (Double Jump) | `Up Arrow` (Double Jump) |
| **Attack** | `F` | `Enter` |
| **Pause** | `Esc` | `Esc` |

**Menu Shortcuts:**
*   **H**: Host Online Game
*   **J**: Join Online Game (Client)
*   **L**: Local 1v1 Game
*   **Enter / Space**: Toggle Ready (Lobby)
*   **Esc**: Quit / Back

---

## 📜 License

This project is open-source and available under the **GNU General Public License v2.0 (GPLv2)**.

**Copyright (c) 2025 amphitainments**
