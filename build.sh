#!/bin/bash
g++ -std=c++17 -o game src/main.cpp src/Game.cpp src/Player.cpp src/Utils.cpp src/UPnPMapper.cpp -Iinclude -Ivendor/miniupnp/miniupnpc/include -I/opt/homebrew/include -L/opt/homebrew/lib $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_net vendor/miniupnp/miniupnpc/build/libminiupnpc.a
