#!/bin/bash
g++ -o server_app main.cpp $(sdl2-config --cflags --libs) -lSDL2_net
