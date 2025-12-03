#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SDL2/SDL_net.h>
#include <string>
#include <iostream>
#include "../include/UPnPMapper.h"



struct NetPowerUp {
    float x, y;
    char type[20];
};

struct Packet {
    int type; // 0=Handshake, 1=Input, 2=State, 3=Start
    // Input Data
    bool keys[5]; // Left, Right, Jump, Down, Attack
    
    // State Data (Host -> Client)
    float gameTime;
    
    float p1X, p1Y, p1VX, p1VY;
    int p1HP;
    char p1Power[20];
    int p1PowerTimer;
    int p1Invincible;
    int p1AttackCooldown;
    bool p1FacingLeft;

    float p2X, p2Y, p2VX, p2VY;
    int p2HP;
    char p2Power[20];
    int p2PowerTimer;
    int p2Invincible;
    int p2AttackCooldown;
    bool p2FacingLeft;

    int numPowerUps;
    NetPowerUp powerUps[5];

    int numProjectiles;
    struct NetProjectile {
        float x, y, vx, vy;
        int owner; // 0 or 1
        char type[10];
    } projectiles[10];

    int gameState; // 0=Menu, 1=Name, 2=Playing, 3=Paused, 4=GameOver
    int winnerId; // 0=None, 1=P1, 2=P2



    int p1Char, p2Char;
    char p1Name[20];
    char p2Name[20];
    bool p1Ready;
    bool p2Ready;
};

class NetworkManager {
public:
    TCPsocket server = nullptr;
    TCPsocket client = nullptr;
    SDLNet_SocketSet socketSet = nullptr; // Socket Set for non-blocking
    bool isHost = false;
    bool connected = false;

    UPnPMapper upnp;

    bool init() {
        if (SDLNet_Init() < 0) {
            std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
            return false;
        }
        socketSet = SDLNet_AllocSocketSet(1); // We only talk to one peer (Client or Host)
        
        // Init UPnP
        std::cout << "Initializing UPnP..." << std::endl;
        if (upnp.init()) {
            std::cout << "UPnP Initialized. External IP: " << upnp.getExternalIP() << std::endl;
        } else {
            std::cout << "UPnP Initialization Failed (Manual Port Forwarding required)." << std::endl;
        }
        
        return true;
    }

    bool hostGame(int port) {
        // Try to map port
        upnp.addPortMapping(port);
        
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, NULL, port) < 0) return false;
        server = SDLNet_TCP_Open(&ip);
        if (!server) return false;
        isHost = true;
        std::cout << "Hosting on port " << port << "..." << std::endl;
        return true;
    }
    
    // ... joinGame ...

    // ... acceptClient ...
    
    // ... send/receive ...

    void disconnect() {
        if (isHost) {
            upnp.removePortMapping(12345); // Assuming default port
        }
        
        if (server) {
            SDLNet_TCP_Close(server);
            server = nullptr;
        }
        if (client) {
            if (socketSet) SDLNet_TCP_DelSocket(socketSet, client);
            SDLNet_TCP_Close(client);
            client = nullptr;
        }
        // Do NOT free socketSet here, as we reuse the NetworkManager instance.
        // It is freed in cleanup().
        
        connected = false;
        isHost = false;
    }

    bool joinGame(const std::string& host, int port) {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, host.c_str(), port) < 0) return false;
        client = SDLNet_TCP_Open(&ip);
        if (!client) return false;
        
        SDLNet_TCP_AddSocket(socketSet, client); // Add to set
        
        isHost = false;
        connected = true;
        std::cout << "Connected to " << host << ":" << port << std::endl;
        return true;
    }

    void acceptClient() {
        if (isHost && !connected) {
            client = SDLNet_TCP_Accept(server);
            if (client) {
                SDLNet_TCP_AddSocket(socketSet, client); // Add to set
                connected = true;
                std::cout << "Client connected!" << std::endl;
            }
        }
    }

    void send(const Packet& p) {
        if (connected && client) {
            SDLNet_TCP_Send(client, &p, sizeof(Packet));
        }
    }

    bool receive(Packet& p) {
        if (connected && client) {
            // Check if data is available (Non-blocking)
            if (SDLNet_CheckSockets(socketSet, 0) > 0) {
                if (SDLNet_SocketReady(client)) {
                    int received = SDLNet_TCP_Recv(client, &p, sizeof(Packet));
                    if (received > 0) {
                        return true;
                    } else if (received == 0) {
                        // Connection closed
                        connected = false;
                        SDLNet_TCP_DelSocket(socketSet, client);
                        SDLNet_TCP_Close(client);
                        client = nullptr;
                        std::cout << "Disconnected." << std::endl;
                    }
                }
            }
        }
        return false;
    }



    // Signaling Server Integration
    std::string requestHostCode(int gamePort) {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, "127.0.0.1", 8080) < 0) return "";
        TCPsocket sock = SDLNet_TCP_Open(&ip);
        if (!sock) return "";

        std::string msg = "HOST " + std::to_string(gamePort);
        SDLNet_TCP_Send(sock, msg.c_str(), msg.length() + 1);

        char buffer[1024];
        int len = SDLNet_TCP_Recv(sock, buffer, 1024);
        SDLNet_TCP_Close(sock);

        if (len > 0) {
            buffer[len] = '\0';
            std::string response(buffer);
            if (response.rfind("CODE", 0) == 0) {
                return response.substr(5);
            }
        }
        return "";
    }

    std::string resolveJoinCode(const std::string& code) {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, "127.0.0.1", 8080) < 0) return "";
        TCPsocket sock = SDLNet_TCP_Open(&ip);
        if (!sock) return "";

        std::string msg = "JOIN " + code;
        SDLNet_TCP_Send(sock, msg.c_str(), msg.length() + 1);

        char buffer[1024];
        int len = SDLNet_TCP_Recv(sock, buffer, 1024);
        SDLNet_TCP_Close(sock);

        if (len > 0) {
            buffer[len] = '\0';
            std::string response(buffer);
            if (response.rfind("ADDR", 0) == 0) {
                return response.substr(5); // Returns "IP PORT"
            }
        }
        return "";
    }

    void cleanup() {
        disconnect();
        if (socketSet) {
            SDLNet_FreeSocketSet(socketSet);
            socketSet = nullptr;
        }
        SDLNet_Quit();
    }
};

#endif
