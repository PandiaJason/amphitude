#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <SDL2/SDL_net.h>
#include <iostream>
#include <string>
#include <vector>




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
    std::string serverIP = "127.0.0.1"; // Default to localhost

    void setSignalingServer(const std::string& ip) {
        serverIP = ip;
    }

    bool init() {
        if (SDLNet_Init() < 0) {
            std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
            return false;
        }
        socketSet = SDLNet_AllocSocketSet(1); // We only talk to one peer (Client or Host)
        

        
        return true;
    }

    bool hostGame(int port) {

        
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
                        // Connection closed by peer
                        std::cout << "Connection closed by peer." << std::endl;
                        connected = false;
                    } else {
                        // Error (-1)
                        // Previously we ignored this, so we'll log it but stay connected to avoid dropping active games on temp errors.
                        std::cerr << "SDLNet_TCP_Recv Error: " << SDLNet_GetError() << std::endl;
                        // connected = false; // Don't disconnect on error for now, just log
                    }
                }
            }
        }
        return false;
    }



    // Signaling Server Integration
    std::string requestHostCode(int gamePort) {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, serverIP.c_str(), 8080) < 0) return "";
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
        if (SDLNet_ResolveHost(&ip, serverIP.c_str(), 8080) < 0) return "";
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
                std::string addr = response.substr(5); // "IP PORT"
                // Localhost Swap Logic for VPNs
                // If the Host is on the same machine as the Signaling Server (common in VPN setup),
                // the Signaling Server sees "127.0.0.1".
                // But the Client needs to connect to the Signaling Server's VPN IP.
                if (addr.rfind("127.0.0.1", 0) == 0 || addr.rfind("localhost", 0) == 0) {
                    size_t space = addr.find(' ');
                    if (space != std::string::npos) {
                        return serverIP + addr.substr(space);
                    }
                }
                return addr;
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
