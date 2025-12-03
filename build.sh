#!/bin/bash
g++ -std=c++17 src/main.cpp src/Game.cpp src/Player.cpp src/Utils.cpp -o game -Iinclude -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net
