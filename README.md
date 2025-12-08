# amphitude

**amphitude** is a high-octane, **Serverless P2P** 2D multiplayer platform fighter built from scratch with C++ and SDL2. It blends retro pixel-art aesthetics with modern, fast-paced combat mechanics.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey.svg)
![Network](https://img.shields.io/badge/network-Serverless%20P2P-green.svg)

---

## ✨ Features

### 🎮 Gameplay
*   **Intense PvP Combat**: Battle your friends in 1v1 duels.
*   **True P2P Multiplayer**: No servers, no accounts, no lag. Connect directly via UDP Hole Punching.
*   **Unique Characters**: Xeno (Boy) & Zeni (Girl).
*   **Dynamic Physics**: Master double jumps, wall jumps, and momentum-based movement.
*   **Lobby System**: Countdown timer (3... 2... 1... GO!) ensures fair starts.

### 🛠️ Technical
*   **Custom Engine**: Built on top of SDL2 for maximum performance.
*   **UDP Networking**: Custom reliable UDP protocol with NAT Hole Punching (STUN).
*   **Zero Backend**: No servers required. Players connect directly.

---

## 🛠️ Installation & Build Guide

### ⚡ Quick Start (Mac/Linux)
1.  **Install Dependencies** (SDL2, SDL2_image, SDL2_ttf, SDL2_net).
2.  **Run Build Script**: `./build.sh`
3.  **Play**: `./amphitude`

---

## 🌐 How to Play Online (Covai <-> Chennai Protocol)

Amphitude connects players directly over the internet using **UDP Hole Punching**.

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

## 🐳 Docker Support (Linux Testing)

You can run the Linux version of the game inside Docker to verify cross-platform compatibility or simulate a network peer.

### 1. Build & Run
We provide a helper script to build the image and run it with X11 forwarding (GUI support).

**Prerequisites (macOS):**
*   **Docker Desktop** installed.
*   **XQuartz** installed (`brew install --cask xquartz`) and running.
    *   *Ensure "Allow connections from network clients" is checked in XQuartz settings.*

**Command:**
```bash
./run_docker.sh
```

### 2. Testing Connection (Mac vs Docker)
Since both run on the same physical router, you must use **NAT Hairpinning** (Green Codes) or Manual Localhost mapping.

*   **Mac**: `./amphitude` (Runs on Port 50000)
*   **Docker**: `./run_docker.sh` (Maps Internal 50000 -> Host 50001)

**To Connect:**
*   **Mac**: Enter `127.0.0.1:50001` (Connect to Docker)
*   **Docker**: Enter `host.docker.internal:50000` (Connect to Mac)

---

## 🕹️ Controls

| Action | Player 1 (Host/Local) | Player 2 (Client/Local) |
| :--- | :--- | :--- |
| **Move** | `W`, `A`, `S`, `D` | `Arrow Keys` |
| **Jump** | `W` (Double Jump) | `Up Arrow` (Double Jump) |
| **Attack** | `F` | `Enter` |
| **Ready** | `Space` / `Enter` | `Space` / `Enter` |
| **Pause** | `Esc` | `Esc` |

---

## 📜 License

This project is open-source and available under the **GNU General Public License v2.0 (GPLv2)**.

**Copyright (c) 2025 amphitainments**
