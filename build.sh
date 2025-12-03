#!/bin/bash
g++ -std=c++17 -o game src/main.cpp src/Game.cpp src/Player.cpp src/Utils.cpp -Iinclude -I/opt/homebrew/include -L/opt/homebrew/lib $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_net
