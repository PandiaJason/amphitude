#include "StunClient.h"
#include <iostream>
#include <cstring>
#include <ctime>

StunClient::StunClient() {
    // No internal socket
}

StunClient::~StunClient() {
    // Nothing to close
}

StunClient::StunResult StunClient::getPublicAddress(UDPsocket socket, const std::string& stunServer, int port) {
    if (!socket) return {false, "", 0};

    // Resolve STUN Server IP
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, stunServer.c_str(), port) < 0) {
        std::cerr << "STUN Resolve Failed: " << SDLNet_GetError() << std::endl;
        return {false, "", 0};
    }

    // Send Binding Request
    // STUN Header (20 bytes):
    // Type: 0x0001 (Binding Request)
    // Length: 0x0000
    // Magic Cookie: 0x2112A442
    // Transaction ID: 12 random bytes
    
    Uint8 request[20];
    memset(request, 0, 20);
    
    // Type 0x0001
    request[0] = 0x00; request[1] = 0x01;
    // Length 0x0000 (no attributes)
    request[2] = 0x00; request[3] = 0x00;
    // Magic Cookie
    request[4] = 0x21; request[5] = 0x12; request[6] = 0xA4; request[7] = 0x42;
    
    // Random Transaction ID
    for(int i=8; i<20; ++i) request[i] = rand() % 255;

    UDPpacket* packet = SDLNet_AllocPacket(512);
    memcpy(packet->data, request, 20);
    packet->len = 20;
    packet->address = ip;

    bool found = false;
    StunResult result = {false, "", 0};

    // Retry loop
    for(int i=0; i<3 && !found; ++i) {
        if (SDLNet_UDP_Send(socket, -1, packet) == 0) continue;
        
        // Wait for response (Timeout 1 sec)
        // Wait for response (Timeout 1 sec)
        Uint32 start = SDL_GetTicks();
        bool responseReceived = false;
        
        for (; SDL_GetTicks() - start < 1000; ) { // 1 sec timeout
            if (SDLNet_UDP_Recv(socket, packet)) {
                // Check if it's a valid STUN Binding Response (0x0101)
                if (packet->len >= 20 && packet->data[0] == 0x01 && packet->data[1] == 0x01) {
                    responseReceived = true;
                    break; // Found it!
                }
                // If not valid, just ignore and keep listening (continue loop)
            }
            SDL_Delay(10);
        }

        if (!responseReceived) {
             // Timeout or no valid response
             continue; // Try next server
        }

        // Parse Response
        result = parseResponse(packet->data, packet->len);
        found = true;

    }
    
    SDLNet_FreePacket(packet);
    return result;
}

StunClient::StunResult StunClient::parseResponse(const Uint8* data, int len) {
    // Parse Attributes
    int offset = 20; // Skip Header
    
    for(;offset < len;) {
        if (offset + 4 > len) break;
        
        Uint16 type = (data[offset] << 8) | data[offset+1];
        Uint16 attrLen = (data[offset+2] << 8) | data[offset+3];
        offset += 4;
        
        if (type == 0x0020) { // XOR-MAPPED-ADDRESS
            if (offset + attrLen > len) break;
            
            // Family (1 byte), Port (2 bytes), Address (4 bytes)
            // Skip 1 byte (reserved)
            // Family is at offset+1
            Uint16 xorPort = (data[offset+2] << 8) | data[offset+3];
            Uint32 xorAddr = (data[offset+4] << 24) | (data[offset+5] << 16) | (data[offset+6] << 8) | data[offset+7];
            
            // XOR with Magic Cookie
            int port = xorPort ^ 0x2112;
            Uint32 cookie = 0x2112A442;
            Uint32 addr = xorAddr ^ cookie;
            
            std::string ipStr = std::to_string((addr >> 24) & 0xFF) + "." +
                                std::to_string((addr >> 16) & 0xFF) + "." +
                                std::to_string((addr >> 8) & 0xFF) + "." +
                                std::to_string(addr & 0xFF);
                                
            return {true, ipStr, port};
        }
        
        offset += attrLen;
        // Padding? STUN attributes are 4-byte aligned
        // But 0x0020 is usually 8 bytes value (family(2)+port(2)+addr(4)), total 12 bytes total attr
        // Let's keep it simple.
    }
    
    return {false, "", 0};
}
