#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <algorithm>

// Room Structure
struct Room {
    std::string ip;
    int port;
    time_t timestamp;
};

// Global Room Store
std::map<std::string, Room> rooms;

// Helper to generate random code
std::string generateCode() {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string code = "";
    for (int i = 0; i < 6; ++i) {
        code += charset[rand() % (sizeof(charset) - 1)];
    }
    return code;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    if (SDL_Init(0) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet_Init Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, 8080) < 0) {
        std::cerr << "ResolveHost Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    TCPsocket server = SDLNet_TCP_Open(&ip);
    if (!server) {
        std::cerr << "TCP_Open Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    std::cout << "Signaling Server running on port 8080..." << std::endl;

    SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(100); // Max 100 clients for now
    SDLNet_TCP_AddSocket(socketSet, server);

    std::vector<TCPsocket> clients;

    bool running = true;
    for (; running; ) {
        int numReady = SDLNet_CheckSockets(socketSet, 10); // 10ms timeout

        if (numReady > 0) {
            // Check for new connection
            if (SDLNet_SocketReady(server)) {
                TCPsocket client = SDLNet_TCP_Accept(server);
                if (client) {
                    SDLNet_TCP_AddSocket(socketSet, client);
                    clients.push_back(client);
                    // std::cout << "Client connected" << std::endl;
                }
            }

            // Check clients
            for (auto it = clients.begin(); it != clients.end(); ) {
                TCPsocket client = *it;
                if (SDLNet_SocketReady(client)) {
                    char buffer[1024];
                    int len = SDLNet_TCP_Recv(client, buffer, 1024);
                    if (len > 0) {
                        buffer[len] = '\0';
                        std::string msg(buffer);
                        // Protocol:
                        // HOST <PORT> -> CODE <CODE>
                        // JOIN <CODE> -> ADDR <IP> <PORT>
                        
                        if (msg.rfind("HOST", 0) == 0) {
                            int port = std::stoi(msg.substr(5));
                            std::string code = generateCode();
                            
                            IPaddress* remoteIP = SDLNet_TCP_GetPeerAddress(client);
                            Uint32 ipVal = SDL_SwapBE32(remoteIP->host);
                            std::string ipStr = std::to_string((ipVal >> 24) & 0xFF) + "." + 
                                                std::to_string((ipVal >> 16) & 0xFF) + "." + 
                                                std::to_string((ipVal >> 8) & 0xFF) + "." + 
                                                std::to_string(ipVal & 0xFF);
                            
                            rooms[code] = {ipStr, port, time(0)};
                            std::cout << "Room Created: " << code << " -> " << ipStr << ":" << port << std::endl;
                            
                            std::string response = "CODE " + code;
                            SDLNet_TCP_Send(client, response.c_str(), response.length() + 1);
                        }
                        else if (msg.rfind("JOIN", 0) == 0) {
                            std::string code = msg.substr(5);
                            // Trim newline if present
                            code.erase(std::remove(code.begin(), code.end(), '\n'), code.end());
                            code.erase(std::remove(code.begin(), code.end(), '\r'), code.end());
                            
                            if (rooms.count(code)) {
                                Room r = rooms[code];
                                // Check Expiry (3 mins = 180s)
                                if (time(0) - r.timestamp > 180) {
                                    std::string response = "ERR EXPIRED";
                                    SDLNet_TCP_Send(client, response.c_str(), response.length() + 1);
                                    rooms.erase(code);
                                } else {
                                    std::string response = "ADDR " + r.ip + " " + std::to_string(r.port);
                                    SDLNet_TCP_Send(client, response.c_str(), response.length() + 1);
                                }
                            } else {
                                std::string response = "ERR INVALID";
                                SDLNet_TCP_Send(client, response.c_str(), response.length() + 1);
                            }
                        }
                    } else {
                        // Disconnected
                        SDLNet_TCP_DelSocket(socketSet, client);
                        SDLNet_TCP_Close(client);
                        it = clients.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
        
        // Cleanup Old Rooms
        // (Ideally do this less frequently)
        for (auto it = rooms.begin(); it != rooms.end(); ) {
            if (time(0) - it->second.timestamp > 3600) { // 1 hour hard cleanup
                it = rooms.erase(it);
            } else {
                ++it;
            }
        }
    }

    SDLNet_FreeSocketSet(socketSet);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
