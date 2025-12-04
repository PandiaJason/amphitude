CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net

# Source files
SRC = src/main.cpp src/Game.cpp src/Player.cpp src/Utils.cpp
OBJ = $(SRC:.cpp=.o)

SERVER_SRC = server/main.cpp
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Targets
all: amphitude server_app

amphitude: $(OBJ)
	$(CXX) $(OBJ) -o amphitude $(LDFLAGS)

server_app: $(SERVER_OBJ)
	$(CXX) $(SERVER_OBJ) -o server_app $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o server/*.o amphitude server_app

.PHONY: all clean
