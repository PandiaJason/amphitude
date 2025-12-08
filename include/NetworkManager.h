#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <SDL2/SDL_net.h>
#include <iostream>
#include <string>
#include <vector>
#include "StunClient.h"

// ... (Struct definitions Packet, NetPowerUp remain same) ...
// Except we need to make Packet serializable or just use it as is if it's POD.
// It is POD.

struct NetPowerUp {
    float x, y;
    char type[20];
};

struct Packet {
    Uint32 seqId; // For reliability (Simple ACK)
    int type; // 0=Handshake, 1=Input, 2=State, 3=Start, 4=Ping/Punch
    
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
    float startTimer; // Countdown Timer (-1 = off, >0 = counting)
};

class NetworkManager {
public:
    UDPsocket udpSocket = nullptr;
    UDPpacket* packet = nullptr;
    
    bool isHost = false;
    bool connected = false;

    // Reliable Packets Queue
    struct ReliablePacket {
        Packet p;
        Uint32 firstSentTime;
        Uint32 lastSentTime;
    };
    std::vector<ReliablePacket> reliableQueue;

    // Send a critical packet that MUST arrive (e.g. Start Game, Game Over)
    void sendReliable(Packet& p) {
        if (!hasPeer || !udpSocket) return;
        
        p.seqId = ++localSeqId;
        
        // Store for retransmission
        ReliablePacket rp;
        rp.p = p;
        rp.firstSentTime = SDL_GetTicks();
        rp.lastSentTime = 0; // Force immediate send
        reliableQueue.push_back(rp);
        
        // Actual send happens in update()
    }
    
    // Peer Address
    IPaddress peerIP;
    bool hasPeer = false;

    // Reliability
    Uint32 localSeqId = 0;
    Uint32 remoteSeqId = 0;
    
    // Discovery
    StunClient stun;
    std::string myPublicIP = "";
    int myPublicPort = 0;
    int myLocalPort = 0;

    bool init() {
        if (SDLNet_Init() < 0) return false;
        
        // Try to bind to a specific port range (50000 - 50100)
        // This allows us to KNOW our local port and display it for LAN/Localhost
        bool bound = false;
        for (int p = 50000; p < 50100; p++) {
            udpSocket = SDLNet_UDP_Open(p);
            if (udpSocket) {
                myLocalPort = p;
                bound = true;
                std::cout << "Bound to Local Port: " << myLocalPort << std::endl;
                break;
            }
        }
        
        if (!bound) {
             std::cerr << "UDP Open Failed: Could not bind to any port in range!" << std::endl;
             return false;
        }
        
        packet = SDLNet_AllocPacket(4096); // Allocated once
        return true;
    }
    
    void discoverPublicIP() {
        std::cout << "Discovering Public IP..." << std::endl;
        auto res = stun.getPublicAddress(udpSocket);
        if (res.success) {
            myPublicIP = res.publicIP;
            myPublicPort = res.publicPort;
            std::cout << "My Public Address: " << myPublicIP << ":" << myPublicPort << std::endl;
        } else {
            std::cerr << "STUN Failed. Using Localhost?" << std::endl;
        }
    }
    
    // "Host" in UDP just means "I am Player 1"
    void setAsHost() {
        isHost = true;
        discoverPublicIP();
    }
    
    // Set Peer Address manually (from Code Exchange)
    void setPeer(const std::string& ipStr, int port) {
        if (SDLNet_ResolveHost(&peerIP, ipStr.c_str(), port) == 0) {
            hasPeer = true;
            std::cout << "Peer Set to: " << ipStr << ":" << port << std::endl;
        } else {
            std::cerr << "Failed to resolve peer: " << ipStr << std::endl;
        }
    }

    Uint32 lastReceiveTime = 0;
    const Uint32 TIMEOUT_MS = 5000; // 5 Seconds Timeout

    // Renamed from updateReliability to update to reflect broader usage
    void update() {
        if (!hasPeer || !udpSocket) return;
        
        Uint32 now = SDL_GetTicks();
        
        // 1. Reliability Retransmissions
        for (auto it = reliableQueue.begin(); it != reliableQueue.end(); ) {
            if (now - it->lastSentTime > 500) {
                if (now - it->firstSentTime > 5000) {
                     std::cout << "Packet Timed Out: " << it->p.type << std::endl;
                     it = reliableQueue.erase(it);
                     continue;
                }
                
                // Retransmit
                memcpy(packet->data, &it->p, sizeof(Packet));
                packet->len = sizeof(Packet);
                packet->address = peerIP;
                SDLNet_UDP_Send(udpSocket, -1, packet);
                it->lastSentTime = now;
            }
            ++it;
        }
        
        // 2. Disconnect Detection (Heartbeat)
        if (connected && now - lastReceiveTime > TIMEOUT_MS) {
            std::cout << "Connection Timed Out! (No packets for " << TIMEOUT_MS << "ms)" << std::endl;
            connected = false;
            // Optionally reset peer? hasPeer = false; 
        }
    }

    void send(Packet& p) {
        if (!hasPeer || !udpSocket) return;
        
        p.seqId = ++localSeqId;
        
        memcpy(packet->data, &p, sizeof(Packet));
        packet->len = sizeof(Packet);
        packet->address = peerIP;
        
        SDLNet_UDP_Send(udpSocket, -1, packet);
    }
    
    // Send a "Hole Punch" packet (Empty or Ping)
    void sendPunch() {
        if (!hasPeer || !udpSocket) return;
        
        const char* msg = "PUNCH";
        memcpy(packet->data, msg, 6);
        packet->len = 6;
        packet->address = peerIP;
        
        SDLNet_UDP_Send(udpSocket, -1, packet);
    }
    
    // Send ACK for a received reliable packet
    void sendAck(Uint32 seqId) {
        if (!hasPeer || !udpSocket) return;
        
        // ACK Packet Structure: Type=99 (Special)
        // Or just re-use Packet struct and set Type=99
        Packet ackP;
        ackP.type = 99; // ACK
        ackP.seqId = seqId; // Echo back the ID
        
        memcpy(packet->data, &ackP, sizeof(Packet));
        packet->len = sizeof(Packet);
        packet->address = peerIP;
        SDLNet_UDP_Send(udpSocket, -1, packet);
    }

    void disconnect() {
        connected = false;
        hasPeer = false;
        isHost = false;
        // Don't close socket, we might reuse it? 
        // Actually, better to keep it open to maintain the port mapping?
        // But for clean restart, maybe close?
    }

    // Reliable Logic Helpers (moved to top)

    bool receive(Packet& p) {
        if (!udpSocket) return false;
        
        if (SDLNet_UDP_Recv(udpSocket, packet)) {
            if (hasPeer) {
                 if (packet->address.host != peerIP.host || packet->address.port != peerIP.port) {
                     // Address mismatch, arguably should ignore, but for now allow (NAT Hairpinning might change IP)
                 }
            }
            
            // Update Heartbeat
            lastReceiveTime = SDL_GetTicks();
            
            if (packet->len == 6 && strncmp((char*)packet->data, "PUNCH", 5) == 0) {
                if (!connected) std::cout << "Recv PUNCH from Peer!" << std::endl;
                
                // Auto-Latch: If we don't have a peer (we are waiting Host), adopt this sender!
                if (!hasPeer) {
                    peerIP = packet->address;
                    hasPeer = true;
                    // Log IP
                    Uint32 ip = SDL_SwapBE32(peerIP.host);
                    std::cout << "Auto-Latched Peer: " 
                              << ((ip>>24)&0xFF) << "." << ((ip>>16)&0xFF) << "." << ((ip>>8)&0xFF) << "." << (ip&0xFF)
                              << ":" << SDL_SwapBE16(peerIP.port) << std::endl;
                }
                
                connected = true; 
                return false; 
            }
            
            if (packet->len == sizeof(Packet)) {
                connected = true; 
                memcpy(&p, packet->data, sizeof(Packet));
                
                // ... (Ack Logic) ...
                if (p.type == 99) {
                     for (auto it = reliableQueue.begin(); it != reliableQueue.end(); ) {
                         if (it->p.seqId == p.seqId) {
                             it = reliableQueue.erase(it);
                         } else {
                             ++it;
                         }
                     }
                     return false; 
                }
                
                if (p.type == 3 || p.type == 4) {
                    sendAck(p.seqId);
                }

                if (p.seqId > remoteSeqId) {
                    remoteSeqId = p.seqId;
                    return true;
                } else {
                    return true;
                }
            }
        }
        return false;
    }
    
    void cleanup() {
        if (udpSocket) SDLNet_UDP_Close(udpSocket);
        if (packet) SDLNet_FreePacket(packet);
        SDLNet_Quit();
    }
};

#endif
