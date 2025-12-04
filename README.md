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

    ```

### 🪟 Windows
1.  **Install Prerequisites**:
    *   **Visual Studio** (with "Desktop development with C++").
    *   **CMake** (from cmake.org).
    *   **SDL2 Libraries**: The easiest way is using **vcpkg**:
        ```powershell
        vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-net
        vcpkg integrate install
        ```
2.  **Build**:
    ```powershell
    mkdir build; cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake
    cmake --build .
    ```
3.  **Run**:
    ```powershell
    .\Debug\amphitude.exe
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

## 🌐 Online Play

Amphitude uses a **Signaling Server** to exchange connection details via simple codes.

1.  **Start the Server**:
    *   Open a terminal in the `build` folder.
    *   Run `./server_app`.
    *   Keep this terminal open!

2.  **Host a Game**:
    *   Run `./amphitude`.
    *   Press **H**.
    *   The game will generate a **Code** (e.g., `X7Z29A`).
    *   Share this code with your friend.

3.  **Join a Game**:
    *   Run `./amphitude`.
    *   Press **J**.
    *   Enter the **Code** and press **Enter**.

*Note: For play over the internet, both players must be able to reach the Signaling Server. Currently, it defaults to `localhost` for testing.*

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
