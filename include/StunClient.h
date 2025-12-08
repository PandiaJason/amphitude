#ifndef STUNCLIENT_H
#define STUNCLIENT_H

#include <string>
#include <vector>
#include <SDL2/SDL_net.h>

class StunClient {
public:
    struct StunResult {
        bool success;
        std::string publicIP;
        int publicPort;
    };

    StunClient();
    ~StunClient();

    // Use the Game's existing UDP socket to discover IP
    // This ensures the port we discover is the same one we listen on involved in NAT mapping.
    StunResult getPublicAddress(UDPsocket socket, const std::string& stunServer = "stun.l.google.com", int port = 19302);

private:
    // Helper to send binding request
    void sendBindingRequest(UDPsocket socket, const std::string& serverIP, int serverPort);
    
    // Helper to parse response
    StunResult parseResponse(const Uint8* data, int len);
};

#endif
